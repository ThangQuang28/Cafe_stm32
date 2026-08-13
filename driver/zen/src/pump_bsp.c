/**
 *******************************************************************************
 * @auth            : thangquang
 * @day             : 19-Sep-2024
 * @file            : pump_bsp.c
 * @description     : PUMP BSP for STM32F0xx MCU
 *******************************************************************************
 */

/* Include -------------------------------------------------------------------*/

#include "zerr.h"
/* BSP */
#include "pump_bsp.h"
/* Middleware */
#include "pump.h"

/* Define --------------------------------------------------------------------*/

#define PUMP_GPIO_PIN		GPIO_PIN_13
#define PUMP_GPIO_PORT		GPIOB

#define PUMP_GPIO_ON		GPIO_PIN_SET
#define PUMP_GPIO_OFF		GPIO_PIN_RESET

/* Private data ----------------------------------------------------------------*/

static bool
pump_state = false;

/* Private functions ----------------------------------------------------------*/

/* Public functions -----------------------------------------------------------*/

/**
 * @brief Open pump GPIO
 *
 * @retval ZERR_OK if successful
 * @retval ZERR_FALSE if failed
 */
zerr_t
pump_bsp_open(void)
{
	pump_state = false;
	HAL_GPIO_WritePin(PUMP_GPIO_PORT, PUMP_GPIO_PIN, PUMP_GPIO_OFF);
	return ZERR_OK;
}

/**
 * @brief Turn pump output on
 *
 * @retval ZERR_OK if successful
 * @retval ZERR_FALSE if failed
 */
zerr_t
pump_bsp_on(void)
{
	HAL_GPIO_WritePin(PUMP_GPIO_PORT, PUMP_GPIO_PIN, PUMP_GPIO_ON);
	pump_state = true;
	return ZERR_OK;
}

/**
 * @brief Turn pump output off
 *
 * @retval ZERR_OK if successful
 * @retval ZERR_FALSE if failed
 */
zerr_t
pump_bsp_off(void)
{
	HAL_GPIO_WritePin(PUMP_GPIO_PORT, PUMP_GPIO_PIN, PUMP_GPIO_OFF);
	pump_state = false;
	return ZERR_OK;
}

/**
 * @brief Get pump output state
 *
 * @retval true	PUMP is on
 * @retval false PUMP is off
 */
bool
pump_bsp_is_on(void)
{
	return pump_state;
}

/* END OF FILE ----------------------------------------------------------------*/
