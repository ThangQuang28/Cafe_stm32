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
#include "zerodet_bsp.h"

/* Define --------------------------------------------------------------------*/

#define ZERODET_BSP_GPIO_PIN		GPIO_PIN_1
#define ZERODET_BSP_GPIO_PORT		GPIOA

/* Private data ----------------------------------------------------------------*/

static volatile uint32_t
zerodet_bsp_event_count = 0U;

static volatile bool
zerodet_bsp_initialized = 0U;

/* Private functions ----------------------------------------------------------*/

/* Public functions -----------------------------------------------------------*/

/**
 * @brief Open zerodet
 *
 * @retval ZERR_OK if successful
 * @retval ZERR_FALSE if failed
 */
zerr_t
zerodet_bsp_open(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitStruct.Pin = ZERODET_BSP_GPIO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(ZERODET_BSP_GPIO_PORT, &GPIO_InitStruct);

	__HAL_GPIO_EXTI_CLEAR_IT(ZERODET_BSP_GPIO_PIN);

	HAL_NVIC_SetPriority(EXTI0_1_IRQn, 0U, 0U);
	HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);

	zerodet_bsp_event_count = 0U;
	zerodet_bsp_initialized = true;

	return ZERR_OK;
}

/**
 * @brief Check zerodet event
 *
 * @retval true if event available
 * @retval false if no event
 */
bool
zerodet_bsp_ready(void)
{
	return zerodet_bsp_initialized;
}

/**
 * @brief Clear Zerodet event
 */
uint32_t
zerodet_bsp_get_event_count(void)
{
	return zerodet_bsp_event_count;
}

/**
 * @brief Clear zerodet event counter
 */
void
zerodet_bsp_clear_event(void)
{
	zerodet_bsp_event_count = 0U;
}

/**
 * @brief Get current Zerodet GPIO state
 *
 * @retval GPIO state
 */
bool
zerodet_bsp_get_state(void)
{
	return(HAL_GPIO_ReadPin(ZERODET_BSP_GPIO_PORT, ZERODET_BSP_GPIO_PIN) == GPIO_PIN_SET);
}

/**
 * @brief Handle Zerodet EXTI event
 * @param[in] gpio_pin: GPIO pin reported by HAL
 */
void
zerodet_bsp_isr(uint16_t gpio_pin)
{
	if(gpio_pin != ZERODET_BSP_GPIO_PIN)
	{
		return;
	}
	zerodet_bsp_event_count++;
}

/* END OF FILE ----------------------------------------------------------------*/
