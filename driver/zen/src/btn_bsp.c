/**
 *******************************************************************************
 * @auth            : thangquang
 * @day             : 19-Sep-2024
 * @file            : btn_bsp.c
 * @description     : Button BSP for STM32F0xx MCU
 *******************************************************************************
 */

/* Include -------------------------------------------------------------------*/

#include "btn_bsp.h"

/* Public functions ----------------------------------------------------------*/

/**
 * @brief      Configure button GPIO and EXTI.
 *
 * @retval     ZERR_OK    Configuration successful.
 * @retval     ZERR_FALSE Configuration failed.
 */
zerr_t btn_bsp_open(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Enable GPIOA clock ----------------------------------------------------*/

    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* Configure BTN_UP ------------------------------------------------------*/

    GPIO_InitStruct.Pin = BTN_UP_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(
        BTN_UP_GPIO_Port,
        &GPIO_InitStruct);

    /* Configure BTN_DOWN ----------------------------------------------------*/

    GPIO_InitStruct.Pin = BTN_DOWN_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(
        BTN_DOWN_GPIO_Port,
        &GPIO_InitStruct);

    /*
     * PA8 and PA10 both belong to EXTI4_15 on STM32F0.
     */

    HAL_NVIC_SetPriority(
        EXTI4_15_IRQn,
        2U,
        0U);

    HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);

    return ZERR_OK;
}

/**
 * @brief      Read button GPIO state.
 *
 * @param[in]  channel
 *
 * @retval     GPIO_PIN_RESET if pressed.
 * @retval     GPIO_PIN_SET   if released.
 */
GPIO_PinState btn_bsp_read(uint8_t channel)
{
    switch (channel)
    {
        case BTN_UP_CHANNEL:
        {
            return HAL_GPIO_ReadPin(
                BTN_UP_GPIO_Port,
                BTN_UP_Pin);
        }

        case BTN_DOWN_CHANNEL:
        {
            return HAL_GPIO_ReadPin(
                BTN_DOWN_GPIO_Port,
                BTN_DOWN_Pin);
        }

        default:
        {
            /*
             * Fail-safe:
             * Invalid channel is treated as released.
             */
            return GPIO_PIN_SET;
        }
    }
}

/* END OF FILE ----------------------------------------------------------------*/
