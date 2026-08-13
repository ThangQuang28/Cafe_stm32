/**
 *******************************************************************************
 * @auth            : teebee
 * @day             : 19-Sep-2024
 * @file            : buzz_bsp.c
 * @description     : BUZZER BSP for STM32F0xx MCU
 *******************************************************************************
 */

/* Include ---------------------------------------------------------------------------------------*/

/* MIddleware */

/* BSP */
#include "buzz_bsp.h"
#include "zerr.h"

/* Define ----------------------------------------------------------------------------------------*/

#define BUZZER_BSP_GPIO_CLK_ENB()		__HAL_RCC_GPIOA_CLK_ENABLE();
#define BUZZ_BSP_PIN					GPIO_PIN_2
#define BUZZ_BSP_GPIO_PORT				GPIOA

/**
 * @brief 	Configure buzzer GPIO
 * @retval	ZERR_OK		Configuration successful
 * @retval	ZERR_FALSE	Configuration failed
 */
zerr_t
buzz_bsp_open(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	BUZZER_BSP_GPIO_CLK_ENB();
	/**
	 * Configure buzzer GPIO
	 */
	GPIO_InitStruct.Pin = BUZZ_BSP_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(BUZZ_BSP_GPIO_PORT, &GPIO_InitStruct);

	/*
	 * Buzzer is OFF after configuration
	 */
	HAL_GPIO_WritePin(BUZZ_BSP_GPIO_PORT, BUZZ_BSP_PIN, GPIO_PIN_RESET);

	return ZERR_OK;
}

/**
 * @brief	Turn on buzzer
 * @retval	None
 */
void
buzz_bsp_on(void)
{
	HAL_GPIO_WritePin(BUZZ_BSP_GPIO_PORT, BUZZ_BSP_PIN, GPIO_PIN_SET);
}

/**
 * @brief	Turn off buzzer
 * @retval	None
 */
void
buzz_bsp_off(void)
{
	HAL_GPIO_WritePin(BUZZ_BSP_GPIO_PORT, BUZZ_BSP_PIN, GPIO_PIN_RESET);
}



/* END OF FILE ************************************************************************************/
