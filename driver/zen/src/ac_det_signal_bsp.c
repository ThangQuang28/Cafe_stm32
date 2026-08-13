/**
 *******************************************************************************
 * @auth            : thangquang
 * @day             : 19-Sep-2024
 * @file            : pump_bsp.c
 * @description     : PUMP BSP for STM32F0xx MCU
 *******************************************************************************
 */

/* Include -------------------------------------------------------------------*/

/* BSP */
#include"ac_det_signal_bsp.h"

/* Define --------------------------------------------------------------------*/

#define AC_DET_SIGNAL_BSP_GPIO_PIN			GPIO_PIN_0
#define AC_DET_SIGNAL_BSP_GPIO_PORT			GPIOA
#define AC_DET_SIGNAL_BSP_ADC_CHANNEL		ADC_CHANNEL_0

/* Private data ----------------------------------------------------------------*/

static volatile bool
ac_det_signal_bsp_initialized = false;

/* Private functions ----------------------------------------------------------*/

/* Public functions -----------------------------------------------------------*/

/**
 * @brief Open AC detection signal
 *
 * @retval ZERR_OK if successful
 * @retval ZERR_FLASE if failed
 */
zerr_t
ac_det_signal_bsp_open(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitStruct.Pin = AC_DET_SIGNAL_BSP_GPIO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init(AC_DET_SIGNAL_BSP_GPIO_PORT, &GPIO_InitStruct);

	ac_det_signal_bsp_initialized = true;

	return ZERR_OK;
}

/**
 * @brief Check AC detection signal BSP status
 *
 * @retval true if ready
 * @retval false if not ready
 */
bool
ac_det_signal_bsp_ready(void)
{
	return ac_det_signal_bsp_initialized;
}

/**
 * @brief Read raw AC detection GPIO state
 *
 * @retval true if AC detection signal is active
 * @retval false if AC detection signal is inactive
 */
bool
ac_det_signal_bsp_read(void)
{
	GPIO_PinState state;

	if(!ac_det_signal_bsp_initialized)
	{
		return false;
	}

	state = HAL_GPIO_ReadPin(AC_DET_SIGNAL_BSP_GPIO_PORT, AC_DET_SIGNAL_BSP_GPIO_PIN);

	return(state == GPIO_PIN_SET);
}


/* END OF FILE ----------------------------------------------------------------*/

