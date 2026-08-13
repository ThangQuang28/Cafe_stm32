/**
 *******************************************************************************
 * @auth            : thangquang
 * @day             : 19-Sep-2024
 * @file            : heater.h
 * @dissaption      : FWCore for STM32F0xx MCU designed by RDU
 *******************************************************************************
 */

/*  Indent using spaces
    Tab width: 4 spaces
    Line width: 100 characters
    Line ending: LF (0x0A) */

/* Prevent recursive inclusion ------------------------------------------------*/
#ifndef __HEATER_H_
#define __HEATER_H_

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
 * @brief Open HEATER
 *
 * @retval ZERR_OK if successful
 * @retval ZERR_FALSE if failed
 */
zerr_t
heater_open(void);

/**
 * @brief Turn HEATER on
 *
 * @retval ZERR_OK if successful
 * @retval ZERR_FALSE if failed
 */
zerr_t
heater_on(void);

/**
 * @brief Turn HEATER off
 *
 * @retval ZERR_OK if successful
 * @retval ZERR_FALSE if failed
 */
zerr_t
heater_off(void);

/**
 * @brief Get HEATER state
 *
 * @retval true HEATER on
 * @retval false HEATER off
 */
bool
heater_is_on(void);

#ifdef __cplusplus
}
#endif

#endif /* __HEATER_H_ */

/* END OF FILE ----------------------------------------------------------------*/

