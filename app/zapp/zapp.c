/**
  **************************************************************************************************
  * @file   zapp.c
  * @brief  Starting point of this project.
  * @by     duonghd | 31-Dec-2024 | Launch for WhiteCat-FWCore.
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

/* Application. */
#include "zapp.h"

/* Middle. */
#include "led.h"
#include "buzz.h"
#include "ntc.h"

/* Private macros --------------------------------------------------------------------------------*/

#define ZAPP_TASK_DELAY				500

/* Private data types ----------------------------------------------------------------------------*/

typedef struct {
	uint8_t b_is_open;
	
} sys_h_t;

/* Private variables -----------------------------------------------------------------------------*/
static sys_h_t gh_sys = {0};

/* Private function prototypes -------------------------------------------------------------------*/
		
static void
zapp_task (void* p_para);

/* Public function bodies ------------------------------------------------------------------------*/

/**
  * @brief  Starting point of the application.
  * @note   If a FreeRTOS API function is called before the scheduler has been started then
  *         interrupts will deliberately be left disabled, and not re-enable again until the first
  *         task starts to execute. (http://www.freertos.org/FAQHelp.html)
  * @param  None.
  * @retval None.
  */
void
zapp_open (void)
{
	xTaskCreate(zapp_task,         		/* Function that implements the task. */
				"zapp task",            /* Text name for the task. */
				ZAPP_STACK_DEEP_START,  /* Stack deep in words, NOT bytes. */
				NULL,    				/* Parameter passed into the task. */
				ZAPP_PRIO_START,        /* Priority at which the task is created. */
				NULL);              	/* Used to pass out the created task's handle.*/
    return;
}

/* Private function bodies -----------------------------------------------------------------------*/

/**
  * @brief      Initialize peripherals and create tasks relating to them.
  * @param[in]  p_para: not use.
  * @retval     None.
  */
static void
zapp_task (void* p_para)
{
#if defined (ZDEBUG) && (TR_HEAP & ZTRACE_ON)
    UBaseType_t uxHighWaterMark;
#endif
	zerr_t ret = ZERR_OK;
/*	char sd_path[4];   		 SD logical drive path */

#if defined (ZERROR) || defined (ZDEBUG)
    ret = ztrace_open();
    if (ZERR_OK != ret)  /* Can not print here, just turn led on */
    {
        led_on(LED_ERROR);
        vTaskSuspend(NULL);
    }
#endif

	ZTRACE_DBG(TR_HEAP, ("DEBUG: %s: priority: %u, stack deep: %u byte, free RAM: %u byte\n",
			__FUNCTION__, ZAPP_PRIO_START, ZAPP_STACK_DEEP_START,
			xPortGetFreeHeapSize()));

#if defined (ZDEBUG) && (TR_HEAP & ZTRACE_ON)
    /* Inspect our own high water mark on entering the task. */
    uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
	(void) uxHighWaterMark;
#endif

#if (DRV_OPEN == NTC10K_DRV) /* NTC 10K driver  */
	ZTRACE_DBG(TR_APP, ("DEBUG: zapp_task: open ntc..\n"));
    ret = ntc_open();
    ZTRACE_ERR(TR_APP, ("ERROR: zapp_task: fail\n"), (ZERR_OK == ret), vTaskSuspend(NULL););

	ZTRACE_DBG(TR_APP, ("DEBUG: zapp_task: start ntc..\n"));
    ret = ntc_start();
    ZTRACE_ERR(TR_APP, ("ERROR: zapp_task: fail\n"), (ZERR_OK == ret), vTaskSuspend(NULL););
#endif

	buzz_open();
	buzz_on();
	vTaskDelay(ZAPP_TASK_DELAY / portTICK_PERIOD_MS);
	buzz_off();
	
#if 1
	for (;;)
	{
		float temp_sensor;
		float temp_boiler;
		uint32_t temp_sensor_x100;
		uint32_t temp_boiler_x100;
		uint16_t raw_sensor;
		uint16_t raw_boiler;

		ntc_update();

		raw_sensor = ntc_get_raw(NTC_TEMP_T);
		raw_boiler = ntc_get_raw(NTC_BOILER_T);
		temp_sensor = read_temp_sensor();
		temp_boiler = read_temp_boiler();

		temp_sensor_x100 = (uint32_t)(temp_sensor * 100.0f);
		temp_boiler_x100 = (uint32_t)(temp_boiler * 100.0f);

		ZTRACE_DBG(
		    TR_APP,
		    ("NTC TEMP: %ld.%02ld C ERR=%u | "
		     "NTC BOILER: %ld.%02ld C ERR=%u\r\n",
		     temp_sensor_x100 / 100,
		     (temp_sensor_x100 < 0 ? -temp_sensor_x100 : temp_sensor_x100) % 100,
		     ntc_error(NTC_TEMP_T),
		     temp_boiler_x100 / 100,
		     (temp_boiler_x100 < 0 ? -temp_boiler_x100 : temp_boiler_x100) % 100,
		     ntc_error(NTC_BOILER_T))
		);
		ZTRACE_DBG(TR_APP,
				("NTC RAW: sensor=%u boiler=%u\r\n", raw_sensor, raw_boiler));
		vTaskDelay(ZAPP_TASK_DELAY / portTICK_PERIOD_MS);
	}

#endif

	vTaskDelete(NULL);
}
