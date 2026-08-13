/**
 *******************************************************************************
 * @auth            : thangquang
 * @day             : 19-Sep-2024
 * @file            : btn_bsp.c
 * @description     : Button BSP for STM32F0xx MCU
 *******************************************************************************
 */

/* Include -------------------------------------------------------------------*/

/* Middleware */
#include "btn.h"

/* BSP */
#include "btn_bsp.h"
#include "zerr.h"

/* Define --------------------------------------------------------------------*/

/* Private data ----------------------------------------------------------------*/

/* Private functions ----------------------------------------------------------*/

/* Public functions -----------------------------------------------------------*/

/**
 * @brief      Configure button GPIO.
 * @retval     ZERR_OK       Configuration successful.
 * @retval     ZERR_FALSE    Configuration failed.
 */
zerr_t
btn_bsp_cfg(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /*
     * Configure BTN_UP.
     */
    GPIO_InitStruct.Pin = BTN_UP_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(BTN_UP_GPIO_Port, &GPIO_InitStruct);

    /*
     * Configure BTN_DOWN.
     */
    GPIO_InitStruct.Pin = BTN_DOWN_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(BTN_DOWN_GPIO_Port, &GPIO_InitStruct);

    return ZERR_OK;
}

/**
 * @brief      Enable GPIO clock used by button.
 * @retval     None.
 */
void
btn_bsp_enable(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
}

/**
 * @brief      Read button state.
 * @param[in]  channel: button channel.
 * @retval     GPIO pin state.
 */
GPIO_PinState
btn_bsp_read(uint8_t channel)
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
            return GPIO_PIN_SET;
        }
    }
}

/* END OF FILE ----------------------------------------------------------------*/
