/**
  **************************************************************************************************
  * @file   zlog.c
  * @brief  Starting point of this project.
  * @by     duonghd | 1-Nov-2024 | Launch for WhiteCat-FWCore.
  **************************************************************************************************
  * This file is part of WhiteCat-FWCore.
  **************************************************************************************************
  */

/*  Indent using spaces
    Tab width   : 4 spaces
    Line width  : 100 characters
    Line ending : LF (0x0A) */

/* Includes --------------------------------------------------------------------------------------*/

/* Global configuration. */
#include "zcfg.h"

/* Middleware. */
#include "zlog.h"

/* BSP */
#include "zlog_bsp.h"

/* Private macros --------------------------------------------------------------------------------*/
#define LOG_BAUD                   115200          /* %Error: 0.16%. */
#define LOG_MAX_LEN_CH             50u /* The maximum number of items ch queue can contain. */
#define LOG_TMO_CH                 portMAX_DELAY   /* Time (ms) to wait for ch queue space */

#define LOG_LF_2_CRLF              1u    /* Convert line ending from unix to windows. */

/* Private function prototypes -------------------------------------------------------------------*/

/* Public function bodies ------------------------------------------------------------------------*/

/* State definition */
typedef enum {
	LOG_STATE_NEW,
	LOG_STATE_OPEN
} zlog_state_t;

/* Log internal handle definition. */
typedef struct
{
    zlog_state_t        	state;      /* Keep track state of this module. */
    SemaphoreHandle_t   sem_mtx; /* Ensure a string is not be messed with another. */
    SemaphoreHandle_t   sem_bin; /* Ensure a string is not be messed with another, use in isr
                                    context. */
    QueueHandle_t       q_ch;   /* Contains list of formatted strings. */
    TickType_t          tmo_ch; /* Time (tick) to wait for ch queue space is available. */
} zlog_h_t;

/* Private variables -----------------------------------------------------------------------------*/
static zlog_h_t gh_log = {LOG_STATE_NEW, NULL, NULL, NULL, 0};

/* Private function prototypes -------------------------------------------------------------------*/
static void
log_vprintf (QueueHandle_t q_ch, TickType_t tmo_ch, const signed char* p_fmt, va_list args);

static void
log_puts (QueueHandle_t q_ch, TickType_t tmo_ch, const signed char* p_str);

static void
log_putc (QueueHandle_t q_ch, TickType_t tmo_ch, signed char c);

/**
  * @brief  Initialize zlog module.
  * @param  None.
  * @retval zerr_t.
  */
zerr_t
zlog_open (void) {
	zerr_t ret = ZERR_OK;
	
	struct { /* 1 : need to clean up; 0 : don't need. */
		uint8_t b_sem_mtx;
		uint8_t b_sem_bin;
		uint8_t b_q_ch;
	} cleanup = {0, 0, 0};
	
	/* vlog module has been already initialized. */
	if(LOG_STATE_OPEN == gh_log.state) { ret = ZERR_OPEN_ALREADY;}
	
	if(ZERR_OK == ret) {
		gh_log.sem_mtx = xSemaphoreCreateMutex();
		if(0 == gh_log.sem_mtx) {
			ret = ZERR_MEM_OUT;
		}
	}
	if (ZERR_OK == ret) {
		gh_log.tmo_ch = pdMS_TO_TICKS(LOG_TMO_CH); /* Converts milliseconds to ticks. */
		/* Create ch queue, calling task will push a formatted string to this queue and interrupt
			handler will pop them from this queue. */
		gh_log.q_ch = xQueueCreate(LOG_MAX_LEN_CH, sizeof(signed char));
		if (NULL == gh_log.q_ch) { /* Didn't enough memmory */
			ret = ZERR_MEM_OUT;
			cleanup.b_sem_bin = cleanup.b_sem_mtx = 1; /* Mark for deallocating later */
		}
	}
	if (ZERR_OK == ret) {
		ret = log_bsp_open(&gh_log.q_ch, LOG_BAUD);
		if (ZERR_OK != ret) {
			/* Mark for deallocating later */
			cleanup.b_sem_bin = cleanup.b_sem_mtx = cleanup.b_q_ch = 1;
		}
	}
	if (ZERR_OK != ret) { /* deallocation all resource in case false */
		if (cleanup.b_sem_bin) { vSemaphoreDelete(gh_log.sem_bin); }
		if (cleanup.b_sem_mtx) { vSemaphoreDelete(gh_log.sem_mtx); }
		if (cleanup.b_q_ch) { vQueueDelete(gh_log.q_ch); }
	} 
	else { gh_log.state = LOG_STATE_OPEN; }
	return ret;
}

/**
  * @brief      Push a formatted string to ch queue.
  * @param[in]  p_fmt: pointer to format string.
  * @param[in]  ...: optional arguments.
  * @retval     None.
  */
void
zlog_printf (const signed char* p_fmt, ...) {
		
		ZTRACE_ASSERT(NULL != p_fmt);
		ZTRACE_ASSERT(LOG_STATE_OPEN == gh_log.state);
		
		/* A block time of zero is used here to ensure the real time operation of the sytem, so the
    string may not be pushed to ch queue. */
		if (pdTRUE == xSemaphoreTake(gh_log.sem_mtx, 0)) {
				va_list args;
				va_start(args, p_fmt);
				log_vprintf(gh_log.q_ch, gh_log.tmo_ch, p_fmt, args);
				va_end(args);
				xSemaphoreGive(gh_log.sem_mtx);
		}
		return;
}

/* Private function prototypes -------------------------------------------------------------------*/
/**
  * @brief      Put a formatted string to a queue.
  *             vlog_vprintf("%d", 1234);               "1234"
  *             vlog_vprintf("%6d,%3d%%", -200, 5);     "  -200,  5%"
  *             vlog_vprintf("%-6u", 100);              "100   "
  *             vlog_vprintf("%ld", 12345678L);         "12345678"
  *             vlog_vprintf("%04x", 0xA3);             "00a3"
  *             vlog_vprintf("%08LX", 0x123ABC);        "00123ABC"
  *             vlog_vprintf("%016b", 0x550F);          "0101010100001111"
  *             vlog_vprintf("%s", "String");           "String"
  *             vlog_vprintf("%-4s", "abc");            "abc "
  *             vlog_vprintf("%4s", "abc");             " abc"
  *             vlog_vprintf("%c", 'a');                "a"
  *             vlog_vprintf("%f", 10.0);               <zlog_vprintf lacks floating point support>
  * @param[in]  p_fmt: pointer to a format string.
  * @param[in]  tmo_ch: time out for sending to ch queue.
  * @param[in]  args: arguments.
  * @param[out] q_ch: contains list of formatted strings.
  * @retval     None.
  */
static void
log_vprintf (QueueHandle_t q_ch, TickType_t tmo_ch, const signed char* p_fmt, va_list args) {
		
		uint16_t r, i, j, w, f;
    uint32_t v;
    signed char s[16], c, d, *p;

    ZTRACE_ASSERT(NULL != q_ch);
    ZTRACE_ASSERT(NULL != p_fmt);

    for (;;) {
        c = *p_fmt++;               /* Get a char */
        if (!c) break;              /* End of format? */
        if (c != '%') {             /* Pass through it if not a % sequense */
            log_putc(q_ch, tmo_ch, c); continue;
        }
        f = 0;
        c = *p_fmt++;               /* Get first char of the sequense */
        if (c == '0') {             /* Flag: '0' padded */
            f = 1; c = *p_fmt++;
        } else {
            if (c == '-') {         /* Flag: left justified */
                f = 2; c = *p_fmt++;
            }
        }
        for (w = 0; c >= '0' && c <= '9'; c = *p_fmt++)   /* Minimum width */
            w = w * 10 + c - '0';
        if (c == 'l' || c == 'L') { /* Prefix: Size is long int */
            f |= 4; c = *p_fmt++;
        }
        if (!c) break;              /* End of format? */
        d = c;
        if (d >= 'a') d -= 0x20;
        switch (d) {                /* Type is... */
        case 'S' :                  /* String */
            p = va_arg(args, signed char*);
            for (j = 0; p[j]; j++) ;
            while (!(f & 2) && j++ < w) log_putc(q_ch, tmo_ch, ' ');
            log_puts(q_ch, tmo_ch, p);
            while (j++ < w) log_putc(q_ch, tmo_ch, ' ');
            continue;
        case 'C' :                  /* Character */
            log_putc(q_ch, tmo_ch, (signed char)va_arg(args, int32_t)); continue;
        case 'B' :                  /* Binary */
            r = 2; break;
        case 'O' :                  /* Octal */
            r = 8; break;
        case 'D' :                  /* Signed decimal */
        case 'U' :                  /* Unsigned decimal */
            r = 10; break;
        case 'X' :                  /* Hexdecimal */
            r = 16; break;
        default:                    /* Unknown type (passthrough) */
            log_putc(q_ch, tmo_ch, c); continue;
        }

        /* Get an argument and put it in numeral */
        v = (f & 4) ? va_arg(args, int32_t) : ((d == 'D') ? (int32_t)va_arg(args, int32_t) : \
                                                            (int32_t)va_arg(args, uint32_t));
        if (d == 'D' && (v & 0x80000000)) {
            v = 0 - v;
            f |= 8;
        }
        i = 0;
        do {
            d = (signed char)(v % r); v /= r;
            if (d > 9) d += (c == 'x') ? 0x27 : 0x07;
            s[i++] = d + '0';
        } while (v && i < sizeof(s));
        if (f & 8) s[i++] = '-';
        j = i; d = (f & 1) ? '0' : ' ';
        while (!(f & 2) && j++ < w) log_putc(q_ch, tmo_ch, d);
        do log_putc(q_ch, tmo_ch, s[--i]); while(i);
        while (j++ < w) log_putc(q_ch, tmo_ch, ' ');
    }
}

static void
log_puts (QueueHandle_t q_ch, TickType_t tmo_ch, const signed char* p_str) {
		
	ZTRACE_ASSERT(NULL != q_ch);
    ZTRACE_ASSERT(NULL != p_str);

    while (*p_str)
    {
        if (LOG_LF_2_CRLF && ('\n' == *p_str)) /* LF -> CRLF */
        {
            signed char c = '\r';
            xQueueSendToBack(q_ch, (const void*)&c, tmo_ch);
        }
				
        xQueueSendToBack(q_ch, (const void*)p_str, tmo_ch);

        ++p_str;
    }
    log_bsp_enb_tx(); /* Enable UART Transmit IT. */
}

static void
log_putc (QueueHandle_t q_ch, TickType_t tmo_ch, signed char c) {
		
		ZTRACE_ASSERT(NULL != q_ch);
		if (LOG_LF_2_CRLF && ('\n' == c)) {
				signed char cc = '\r';
				xQueueSendToBack(q_ch, (const void *)&cc, tmo_ch);
		}
		xQueueSendToBack(q_ch, (const void *)&c, tmo_ch);
		log_bsp_enb_tx();  /* Enable UART Transmit IT. */
}

/* END OF FILE ************************************************************************************/
