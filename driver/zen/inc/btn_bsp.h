/**
 *******************************************************************************
 * @auth            : thangquang
 * @day             : 19-Sep-2024
 * @file            : btn_bsp.h
 * @description     : Button BSP for STM32F0xx MCU
 *******************************************************************************
 */

#ifndef __BTN_BSP_H
#define __BTN_BSP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Include -------------------------------------------------------------------*/

#include "stm32f0xx_hal.h"
#include "zerr.h"

/* Define --------------------------------------------------------------------*/

/*
 * Button hardware mapping
 *
 * Active LOW:
 *     GPIO HIGH -> released
 *     GPIO LOW  -> pressed
 */

#define BTN_UP_Pin              GPIO_PIN_8
#define BTN_UP_GPIO_Port        GPIOA

#define BTN_DOWN_Pin            GPIO_PIN_10
#define BTN_DOWN_GPIO_Port      GPIOA

#define NUMBER_BTN              2U

#define BTN_UP_CHANNEL          0U
#define BTN_DOWN_CHANNEL        1U

/* Public function prototypes ------------------------------------------------*/

/**
 * @brief      Initialize button GPIO and EXTI.
 *
 * @retval     ZERR_OK    Initialization successful.
 * @retval     ZERR_FALSE Initialization failed.
 */
zerr_t
btn_bsp_open(void);

/**
 * @brief      Read button GPIO state.
 *
 * @param[in]  channel
 *             BTN_UP_CHANNEL
 *             BTN_DOWN_CHANNEL
 *
 * @retval     GPIO_PIN_RESET if button is pressed.
 * @retval     GPIO_PIN_SET   if button is released.
 */
GPIO_PinState
btn_bsp_read(uint8_t channel);

#ifdef __cplusplus
}
#endif

#endif /* __BTN_BSP_H */

/* END OF FILE ----------------------------------------------------------------*/
