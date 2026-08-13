/**
 *******************************************************************************
 * @auth            : thangquang
 * @day             : 19-Sep-2024
 * @file            : heater_bsp.c
 * @description     : Heater BSP for STM32F0xx MCU
 *******************************************************************************
 */

/* Include -------------------------------------------------------------------*/

#include "zerr.h"
/* Middleware */
#include "heater.h"
/* BSP */
#include "heater_bsp.h"

/* Define --------------------------------------------------------------------*/

#define HEATER_GPIO_PIN			GPIO_PIN_14
#define HEATER_GPIO_PORT		GPIOB

#define HEATER_GPIO_ON			GPIO_PIN_SET
#define HEATER_GPIO_OFF 		GPIO_PIN_RESET

/* Private data ----------------------------------------------------------------*/

static bool
heater_state = false;

/* Private functions ----------------------------------------------------------*/

/* Public functions -----------------------------------------------------------*/

/**
 * @brief Open heater GPIO
 *
 * @retval ZERR_OK if successful
 * @retval ZERR_FALSE if failed
 */
zerr_t
heater_bsp_open(void)
{
	heater_state = false;
	HAL_GPIO_WritePin(HEATER_GPIO_PORT, HEATER_GPIO_PIN, HEATER_GPIO_OFF);
	return ZERR_OK;
}

/**
 * @brief Turn heater output on
 *
 * @retval ZERR_OK if successful
 * @retval ZERR_FALSE if failed
 */
zerr_t
heater_bsp_on(void)
{
	HAL_GPIO_WritePin(HEATER_GPIO_PORT, HEATER_GPIO_PIN, HEATER_GPIO_ON);
	heater_state = true;
	return ZERR_OK;
}

/**
 * @brief Turn heater output off
 *
 * @retval ZERR_OK if successful
 * @retval ZERR_FALSE if failed
 */
zerr_t
heater_bsp_off(void)
{
	HAL_GPIO_WritePin(HEATER_GPIO_PORT, HEATER_GPIO_PIN, HEATER_GPIO_OFF);
	heater_state = false;
	return ZERR_OK;
}

/**
 * @brief Get heater output state
 *
 * @retval true Heater is on
 * @retval false Heater is off
 */
bool
heater_bsp_is_on(void)
{
	return heater_state;
}

/* END OF FILE ----------------------------------------------------------------*/
