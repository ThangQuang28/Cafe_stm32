/**
 *******************************************************************************
 * @auth            : thangquang
 * @day             : 19-Sep-2024
 * @file            : pid_bsp.c
 * @description     : PUMP BSP for STM32F0xx MCU
 *******************************************************************************
 */
/* Indent using spaces
 * Tab width   : 4 spaces
 * Line width  : 100 characters
 * Line ending : LF (0x0A)
 */

/* Include -------------------------------------------------------------------*/

/* BSP */
#include "pid_bsp.h"

/* Define --------------------------------------------------------------------*/

#define PID_BSP_SAMPLE_TIME_MS			200U
#define PID_BSP_SAMPLE_TIME_S			PID_BSP_SAMPLE_TIME_MS / 1000.0f

/* Private data ----------------------------------------------------------------*/

static volatile bool
pid_bsp_init = false;

static volatile uint32_t
pid_bsp_last_tick = 0U;

static volatile bool
pid_bsp_update_flag = false;

/* Private functions ----------------------------------------------------------*/

/* Public functions -----------------------------------------------------------*/

/**
 * @brief Open PID BSP
 */
zerr_t
pid_bsp_open(void)
{
    pid_bsp_last_tick = HAL_GetTick();

    pid_bsp_update_flag = false;

    pid_bsp_init = true;

    return ZERR_OK;
}

/**
 * @brief Check PID BSP status
 *
 * @retval true if ready
 * @retval false if not ready
 */
bool
pid_bsp_ready(void)
{
	return pid_bsp_init;
}

/**
 * @brief Check PID update period
 */
bool
pid_bsp_update_ready(void)
{
	uint32_t current_tick;
	if(!pid_bsp_init)
	{
		return false;
	}
	current_tick = HAL_GetTick();

	if((current_tick - pid_bsp_last_tick) >=
		PID_BSP_SAMPLE_TIME_MS)
	{
		pid_bsp_last_tick = current_tick;
		pid_bsp_update_flag = true;
	}
	return pid_bsp_update_flag;
}

/**
 * @brief Clear PID update event
 */
void
pid_bsp_clear_update(void)
{
	pid_bsp_update_flag = false;
}

/**
 * @brief Get the PID sample period in seconds
 */
uint32_t
pid_bsp_get_sample_time_s(void)
{
	return PID_BSP_SAMPLE_TIME_S;
}

/* END OF FILE ----------------------------------------------------------------*/
