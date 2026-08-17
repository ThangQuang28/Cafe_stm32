/**
  **************************************************************************************************
  * @file   zapp.c
  * @brief  Starting point of this project.
  * @by     duonghd | 31-Dec-2024 | Launch for WhiteCat-FWCore.
  * @update thangquang | 14-Aug-2026 | Add brew state machine wiring ntc + pid + heater + pump +
  *         btn + ac_det_signal + zerodet, with FAULT handling on sensor/AC loss (see review notes).
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
 #include <pid_.h>
#include "zcfg.h"

/* Application. */
#include "zapp.h"

/* ST driver. */
#include "hw.h"

/* Middle. */
#include "led.h"
#include "buzz.h"
#include "ntc.h"
#include "btn.h"
#include "heater.h"
#include "pump.h"
#include "zerodet.h"
#include "ac_det_signal.h"
#include "pid.h"

/* Private macros --------------------------------------------------------------------------------*/

#define ZAPP_TASK_DELAY				500U	/* ms. Also used as PID sample time (ts_ms). */

/* --- Brew process parameters -------------------------------------------------------------------
 * NOTE: placeholders - MUST be tuned on the real machine (boiler thermal mass, mains voltage,
 * actual desired shot volume/time). Kept here, not buried inside logic, so they are easy to find.
 * --------------------------------------------------------------------------------------------- */
#define BOILER_TARGET_TEMP_X10			900		/* 90.0 C target boiler temperature. */
#define BOILER_HEATING_TIMEOUT_MS		120000U	/* Max time allowed to reach target before FAULT. */
#define BREW_DURATION_MS				25000U	/* Pump-on duration for one shot. */

/* PID gains, Q12 fixed-point (see pid.h : real_gain * 4096). Placeholder - tune on hardware,
 * start with Ki = Kd = 0 and raise Kp until the boiler oscillates gently around setpoint. */
#define BOILER_PID_KP_Q12				(2L * 4096L)
#define BOILER_PID_KI_Q12				(0L * 4096L)
#define BOILER_PID_KD_Q12				(0L * 4096L)

/* Heater output stage is ON/OFF only (see heater_bsp.c, plain GPIO), so it is driven here with
 * software time-proportioning control: PID output (0-100 %) sets how many ticks, out of every
 * HEATER_PWM_WINDOW_STEPS ticks, the heater stays on. window = ZAPP_TASK_DELAY * this value. */
#define HEATER_PWM_WINDOW_STEPS		4U		/* 4 * 500 ms = 2000 ms window. */

/* btn.h only defines BTN_UP / BTN_DOWN - map them to app-level meaning here in one place. */
#define START_BTN						BTN_UP
#define FAULT_RESET_BTN				BTN_DOWN

/* Private data types ----------------------------------------------------------------------------*/

PID_TypeDef PID_Controller;

typedef struct {
	uint8_t b_is_open;

} sys_h_t;

typedef enum
{
	BREW_ST_IDLE = 0,
	BREW_ST_HEATING,
	BREW_ST_BREWING,
	BREW_ST_DONE,
	BREW_ST_FAULT
} brew_state_t;

/* Private variables -----------------------------------------------------------------------------*/
static sys_h_t gh_sys = {0};

static brew_state_t
gs_brew_state = BREW_ST_IDLE;

static pid_t
gh_boiler_pid;

static uint8_t
g_heater_window_step = 0U;

static uint32_t
g_state_enter_tick = 0U;

/* Private function prototypes -------------------------------------------------------------------*/

static void
zapp_task (void* p_para);

static bool
zapp_sensors_valid (void);

static void
zapp_enter_fault (const char* p_reason);

static void
zapp_heater_time_proportioning (int32_t duty_percent);

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
  * @brief      Check whether both NTC channels currently hold valid readings and AC is present.
  * @note       This is the single choke point every state must go through before energizing the
  *             heater - do not duplicate this check inline elsewhere.
  * @retval     true  if safe to drive the heater.
  * @retval     false otherwise.
  */
static bool
zapp_sensors_valid (void)
{
	if (ntc_error(NTC_TEMP_T) || ntc_error(NTC_BOILER_T))
	{
		return false;
	}

	if (ac_det_signal_ready() && !ac_det_signal_is_present())
	{
		return false;
	}

	return true;
}

/**
  * @brief      Force heater/pump to a known-safe OFF state and move to FAULT.
  * @note       FAULT never auto-recovers - see BREW_ST_FAULT below, it requires an explicit
  *             long-press of FAULT_RESET_BTN while sensors are valid again.
  * @param[in]  p_reason: text reason, for trace only.
  * @retval     None.
  */
static void
zapp_enter_fault (const char* p_reason)
{
	heater_off();
	pump_off();
	g_heater_window_step = 0U;

	ZTRACE_ERR(TR_APP, ("ERROR: zapp: entering FAULT (%s)\r\n", p_reason), false, (void)0);

	gs_brew_state = BREW_ST_FAULT;
	g_state_enter_tick = HAL_GetTick();
}

/**
  * @brief      Drive the ON/OFF heater output using software time-proportioning control.
  * @param[in]  duty_percent: desired ON ratio, 0-100 (clamped internally).
  * @retval     None.
  */
static void
zapp_heater_time_proportioning (int32_t duty_percent)
{
	uint8_t on_steps;

	if (duty_percent < 0)
	{
		duty_percent = 0;
	}
	if (duty_percent > 100)
	{
		duty_percent = 100;
	}

	on_steps = (uint8_t)(((uint32_t)duty_percent * HEATER_PWM_WINDOW_STEPS) / 100U);

	if (g_heater_window_step < on_steps)
	{
		heater_on();
	}
	else
	{
		heater_off();
	}

	g_heater_window_step = (uint8_t)((g_heater_window_step + 1U) % HEATER_PWM_WINDOW_STEPS);
}

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
	int16_t pv_x10;
	int32_t pid_out;
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

#if (DRV_OPEN == BUTTON_DRV_DRV)
	ZTRACE_DBG(TR_APP, ("DEBUG: zapp_task: open button..\n"));
    ret = btn_open();
    ZTRACE_ERR(TR_APP, ("ERROR: zapp_task: fail\n"), (ZERR_OK == ret), vTaskSuspend(NULL););
#endif
	buzz_open();


	buzz_on();
	vTaskDelay(ZAPP_TASK_DELAY / portTICK_PERIOD_MS);
	buzz_off();

	gs_brew_state = BREW_ST_IDLE;
	g_state_enter_tick = HAL_GetTick();

	for (;;)
	{
		float temp = read_temp_boiler();

		int32_t temp_int = (int32_t)temp;
		uint32_t temp_dec = (uint32_t)((temp - (float)temp_int) * 100.0f);

		ZTRACE_DBG(TR_APP,
				("STATE=%u BOILER_=%d TEMP_ERR=%u BOILER_ERR=%u\r\n",
				 (long)temp_int,
				 (unsigned long)temp_dec));


		vTaskDelay(ZAPP_TASK_DELAY / portTICK_PERIOD_MS);
	}

	vTaskDelete(NULL);
}

/* END OF FILE ----------------------------------------------------------------------------------*/
