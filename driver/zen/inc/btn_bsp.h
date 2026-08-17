/**
 *******************************************************************************
 * @auth            : teebee
 * @day             : 19-Sep-2024
 * @file            : btn_bsp.h
 * @dissaption       : FWCore for STM32F0xx MCU designed by RDU
 *******************************************************************************
 */

/*  Indent using spaces
    Tab width: 4 spaces
    Line width: 100 characters
    Line ending: LF (0x0A) */

/* Prevent recursive inclusion ------------------------------------------------*/

#ifndef __BSP_H_
#define __BSP_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Public macros ---------------------------------------------------------------------------------*/

/* Include ---------------------------------------------------------------------------------------*/

/* HAL Library */
#include "stm32f0xx_hal.h"

/* Global */
#include "zerr.h"

/* Define ---------------------------------------------------------------------------------------*/

#define BTN_UP_Pin			GPIO_PIN_8
#define BTN_UP_GPIO_Port	GPIOA

#define BTN_DOWN_Pin		GPIO_PIN_10
#define BTN_DOWN_GPIO_Port	GPIOA

#define NUMBER_BTN			2U

#define BTN_UP_CHANNEL		0U
#define BTN_DOWN_CHANNEL	1u

/* Private data types ----------------------------------------------------------------------------*/

/* Public data types -----------------------------------------------------------------------------*/

/* Exported constants ----------------------------------------------------------------------------*/

/* Private function prototypes -------------------------------------------------------------------*/

/* Public function prototypes --------------------------------------------------------------------*/

/**
 * @brief		Cfg button GPIO
 * @retval		ZERR_OK if successful
 * @retval		ZERR_FAILED if failed
 */
zerr_t
btn_bsp_open(void);

/**
 * @brief		Enabled CLK button GPIO
 * @retval		None
 */
void
btn_bsp_enable(void);

/**
 * @brief		Read button state
 * @param[in]	channel: button channel
 * @retval		GPIO_Pin_RESET if pressed
 * @retval		GPIO_PIN_SET if released
 */
GPIO_PinState
btn_bsp_read(uint8_t channel);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_H_ */

/* END OF FILE ****************************************************************/
