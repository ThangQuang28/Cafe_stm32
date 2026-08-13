/**
 * -----------------------------------------------------------------------------
 * @auth            : thangquang
 * @day             : 19-Sep-2024
 * @file            : pid_bsp.h
 * @description     :PID BSP for STM32F0xx MCU
 * -----------------------------------------------------------------------------
 */

/* Indent using spaces
 * Tab width   : 4 spaces
 * Line width  : 100 characters
 * Line ending : LF (0x0A)
 */

/* Prevent recursive inclusion ------------------------------------------------*/

#ifndef __PID_BSP_H_
#define __PID_BSP_H_

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
 * @brief Open PID BSP
 */
zerr_t
pid_bsp_open(void);

/**
 * @brief Check PID BSP status
 */
bool
pid_bsp_ready(void);

/**
 * @brief Check whether PID update period has elapsed
 *
 * @retval true if PID tick is available
 * @retval flase if PID tick is not available
 */
bool
pid_bsp_update_ready(void);

/**
 * @brief Clear PID update event
 */
void
pid_bsp_clear_update(void);

/**
 * @brief Get the PID sample period in seconds
 */
uint32_t
pid_bsp_get_sample_time_s(void);


#ifdef __cplusplus
}
#endif

#endif /* __PID_BSP_H_ */

/* END OF FILE ----------------------------------------------------------------*/
