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

/* Private data ----------------------------------------------------------------------------------*/

static bool
heater_state = false;

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
