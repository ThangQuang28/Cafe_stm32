/**
 * -----------------------------------------------------------------------------
 * @auth            : thangquang
 * @day             : 19-Sep-2024
 * @file            : ac_det_signal_bsp.h
 * @description     : PUMP BSP for STM32F0xx MCU
 * -----------------------------------------------------------------------------
 */

/* Indent using spaces
 * Tab width   : 4 spaces
 * Line width  : 100 characters
 * Line ending : LF (0x0A)
 */

/* Prevent recursive inclusion ------------------------------------------------*/
#ifndef __AC_DET_SIGNAL_BSP_H_
#define __AC_DET_SIGNAL_BSP_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Public macros ----------------------------------------------------------------*/

/* Include ----------------------------------------------------------------------*/

/* Global */
#include "zerr.h"
/* C library */
#include "stdbool.h"
#include "stdint.h"
/* HAL library */
#include "stm32f0xx_hal.h"

/* Define -----------------------------------------------------------------------*/

/* Private data types -----------------------------------------------------------*/

/* Public data types ------------------------------------------------------------*/

/* Exported constants -----------------------------------------------------------*/

/* Private function prototypes -------------------------------------------------*/

/* Public function prototypes --------------------------------------------------*/

/**
 * @brief Open AC detection signal BSP
 *
 * @retval ZERR_OK if successful
 * @retval ZERR_FLASE if failed
 */
zerr_t
ac_det_signal_bsp_open(void);

/**
 * @brief Check AC detection signal BSP status
 *
 * @retval true if ready
 * @retval false if not ready
 */
bool
ac_det_signal_bsp_ready(void);

/**
 * @brief Read raw AC detection GPIO state
 *
 * @retval GPIO logic state
 */
bool
ac_det_signal_bsp_read(void);

#ifdef __cplusplus
}
#endif

#endif /* __AC_DET_SIGNAL_BSP_H_ */

/* END OF FILE ----------------------------------------------------------------*/
