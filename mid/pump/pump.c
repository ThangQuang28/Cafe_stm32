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
#include "pump.h"
/* BSP */
#include "pump_bsp.h"

/* Private data ----------------------------------------------------------------------------------*/

static bool
pump_state = false;

/* Private function ------------------------------------------------------------------------------*/

/**
 * @brief Open Pump
 *
 * @retval ZERR_OK if successful
 * @retval ZERR_FALSE if failed
 */
zerr_t
pump_open(void)
{
	zerr_t ret;
	pump_state = false;
	ret = pump_bsp_open();
	if(ret != ZERR_OK)
	{
		return ret;
	}

	return ZERR_OK;
}

/**
 * @brief Turn pump on
 *
 * @retval ZERR_OK if successful
 * @retval ZERR_FALSE if failed
 */
zerr_t
pump_on(void)
{
	zerr_t ret;
	ret = pump_bsp_on();
	if(ret != ZERR_OK)
	{
		return ret;
	}
	pump_state = true;
	return ZERR_OK;
}

/**
 * @brief Turn pump off
 *
 * @retval ZERR_OK if successful
 * @retval ZERR_FALSE if failed
 */
zerr_t
pump_off(void)
{
	zerr_t ret;
	ret = pump_bsp_off();
	if(ret != ZERR_OK)
	{
		return ret;
	}
	pump_state = false;
	return ZERR_OK;
}

/**
 * @brief Get pump state
 *
 * @retval true		Pump is on
 * @retval false	Pump is off
 */
bool
pump_is_on(void)
{
	return pump_state;
}

/* END OF FILE ----------------------------------------------------------------*/
