/**
 * -----------------------------------------------------------------------------
 * @auth            : thangquang
 * @day             : 19-Sep-2024
 * @file            : pump_bsp.h
 * @description     : PUMP BSP for STM32F0xx MCU
 * -----------------------------------------------------------------------------
 */

/* Indent using spaces
 * Tab width   : 4 spaces
 * Line width  : 100 characters
 * Line ending : LF (0x0A)
 */

/* Prevent recursive inclusion ------------------------------------------------*/

#ifndef __ZERODET_BSP_H_
#define __ZERODET_BSP_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Public macros ----------------------------------------------------------------*/

/* Include ----------------------------------------------------------------------*/

/* Global */
#include "zerr.h"

#include "stdbool.h"
#include "stdint.h"
/* HAL Library */
#include "stm32f0xx_hal.h"

/* Define -----------------------------------------------------------------------*/

/* Private data types -----------------------------------------------------------*/

/* Public data types ------------------------------------------------------------*/

/* Exported constants -----------------------------------------------------------*/

/* Private function prototypes -------------------------------------------------*/

/* Public function prototypes --------------------------------------------------*/

/**
 * @brief Open Zerodet GPIO and EXTI
 *
 * @retval ZERR_OK if successful
 * @retval ZERR_FALSE if failed
 */
zerr_t
zerodet_bsp_open(void);

/**
 * @brief Check wether a Zerodet event has occurred
 *
 * @retval true if event is available
 * @retval false if no event
 */
bool
zerodet_bsp_ready(void);

/**
 * @brief Get number of Zerodet events
 *
 * @retval Number of detected Zerodet events
 */
uint32_t
zerodet_bsp_get_event_count(void);

/**
 * @brief Clear number of Zerodet events
 * @param[in] count: Number of events to clear
 */
void
zerodet_bsp_clear_event(void);

/**
 * @brief Get current zerodet GPIO state
 *
 * @retval GPIO state
 */
bool
zerodet_bsp_get_state(void);

/**
 * @brief Process Zerodet EXTI interrupt.
 *
 * @param[in] GPIO_Pin: GPIO pin that generated the EXTI event.
 *
 * @retval None.
 */
void
zerodet_bsp_isr(uint16_t GPIO_Pin);

#ifdef __cplusplus
}
#endif

#endif /* __ZERODET_BSP_H_ */

/* END OF FILE ----------------------------------------------------------------*/

