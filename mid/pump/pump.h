/**
 *******************************************************************************
 * @auth            : thangquang
 * @day             : 19-Sep-2024
 * @file            : pump.h
 * @dissaption      : FWCore for STM32F0xx MCU designed by RDU
 *******************************************************************************
 */

/*  Indent using spaces
    Tab width: 4 spaces
    Line width: 100 characters
    Line ending: LF (0x0A) */

/* Prevent recursive inclusion ------------------------------------------------*/
#ifndef __PUMP_H_
#define __PUMP_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Public macros ---------------------------------------------------------------------------------*/

/* Include ---------------------------------------------------------------------------------------*/

/* Global configuration */
#include "zerr.h"
#include "stdbool.h"

/* Define ----------------------------------------------------------------------------------------*/

/* Private data types ----------------------------------------------------------------------------*/

/* Public data types -----------------------------------------------------------------------------*/

/* Exported constants ----------------------------------------------------------------------------*/

/* Private function prototypes -------------------------------------------------------------------*/

/* Public function prototypes --------------------------------------------------------------------*/

/**
 * @brief Open Pump
 *
 * @retval ZERR_OK if successful
 * @retval ZERR_FLASE if failed
 */
zerr_t
pump_open(void);

/**
 * @brief Turn pump on
 *
 * @retval ZERR_OK if successful
 * @retval ZERR_FALSE if failed
 */
zerr_t
pump_on(void);

/**
 * @brief Turn pump off
 *
 * @retval ZERR_OK if successful
 * @retval ZERR_FALSE if failed
 */
zerr_t
pump_off(void);

/**
 * @brief Get pump state
 *
 * @retval true		Pump is on
 * @retval false	Pump is off
 */
bool
pump_is_on(void);

#ifdef __cplusplus
}
#endif

#endif /* __PUMP_H_ */

/* END OF FILE ----------------------------------------------------------------*/
