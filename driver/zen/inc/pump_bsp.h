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

#ifndef __PUMP_BSP_H_
#define __PUMP_BSP_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Public macros ----------------------------------------------------------------*/

/* Include ----------------------------------------------------------------------*/

#include "stdbool.h"
/* HAL Library */
#include "stm32f0xx_hal.h"
/* Global */
#include "zerr.h"

/* Define -----------------------------------------------------------------------*/

/* Private data types -----------------------------------------------------------*/

/* Public data types ------------------------------------------------------------*/

/* Exported constants -----------------------------------------------------------*/

/* Private function prototypes -------------------------------------------------*/

/* Public function --------------------------------------------------------------*/

/**
 * @brief Open pump GPIO
 *
 * @retval ZERR_OK if successful
 * @retval ZERR_FALSE if failed
 */
zerr_t
pump_bsp_open(void);

/**
 * @brief Turn pump output on
 *
 * @retval ZERR_OK if successful
 * @retval ZERR_FALSE if failed
 */
zerr_t
pump_bsp_on(void);

/**
 * @brief Turn pump output off
 *
 * @retval ZERR_OK if successful
 * @retval ZERR_FALSE if failed
 */
zerr_t
pump_bsp_off(void);

/**
 * @brief Get pump output state
 *
 * @retval true	Output is on
 * @retval false Output is off
 */
bool
pump_bsp_is_on(void);

#ifdef __cplusplus
}
#endif

#endif /* __PUMP_BSP_H_ */

/* END OF FILE ----------------------------------------------------------------*/
