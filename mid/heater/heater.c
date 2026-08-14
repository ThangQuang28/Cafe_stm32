 /**
 *******************************************************************************
 * @auth            : manhtd
 * @day             : 19-Sep-2024
 * @file            : btn.c
 * @dissaption       : FWCore for STM32F0xx MCU designed by RDU
 *******************************************************************************
 */

/*  Indent using spaces
    Tab width: 4 spaces
    Line width: 100 characters
    Line ending: LF (0x0A) */

/* Public macros ---------------------------------------------------------------------------------*/

/* Include ---------------------------------------------------------------------------------------*/

/* Middleware */
#include "heater.h"

/* BSP */
#include "heater_bsp.h"

/* Define ----------------------------------------------------------------------------------------*/

#define HEATER_MAX_ON_MS				180000U

/* Private data ----------------------------------------------------------------------------------*/

static bool
heater_state = false;

static uint32_t
heater_on_tick = 0U;

/* Private function ------------------------------------------------------------------------------*/

/**
 * @brief Open heater
 *
 * @retval ZERR_OK if successful
 * @retval ZERR_FALSE if failed
 */
zerr_t
heater_open(void)
{
	zerr_t ret;
	heater_state = false;
	heater_on_tick = 0U;
	ret = heater_bsp_open();
	if(ret != ZERR_OK)
	{
		return ret;
	}

	return ZERR_OK;
}

/**
 * @brief Turn heater on
 *
 * @retval ZERR_OK if successful
 * @retval ZERR_FALSE if failed
 */
zerr_t
heater_on(void)
{
	zerr_t ret;
	ret = heater_bsp_on();
	if(ret != ZERR_OK)
	{
		return ret;
	}

	if(!heater_state)
	{
		heater_on_tick = HAL_GetTick();
	}
	heater_state = true;

	return ZERR_OK;
}

/**
 * @brief Turn heater off
 *
 * @retval ZERR_OK if successful
 * @retval ZERR_FALSE if failed
 */
zerr_t
heater_off(void)
{
	zerr_t ret;
	ret = heater_bsp_off();
	if(ret != ZERR_OK)
	{
		return ret;
	}
	heater_state = false;
	return ZERR_OK;
}

void
heater_watchdog_check(void)
{
	if(!heater_state && ((HAL_GetTick() - heater_on_tick) > HEATER_MAX_ON_MS))
	{
		(void)heater_bsp_off();
		heater_state = false;
	}
}

/**
 * @brief Get heater state
 *
 * @retval true		Heater is on
 * @retval false	Heater is off
 */
bool
heater_is_on(void)
{
	return heater_state;
}

/* END OF FILE ----------------------------------------------------------------*/
