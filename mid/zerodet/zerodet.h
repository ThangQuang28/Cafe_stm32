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

#ifndef __ZERODET_H_
#define __ZERODET_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Public macros ---------------------------------------------------------------------------------*/

/* Include ---------------------------------------------------------------------------------------*/

/* C library */
#include "stdbool.h"
#include "stdint.h"
/* Global */
#include "zerr.h"

/* Define ----------------------------------------------------------------------------------------*/

/* Private data types ----------------------------------------------------------------------------*/

/* Public data types -----------------------------------------------------------------------------*/

/* Exported constants ----------------------------------------------------------------------------*/

/* Private function prototypes -------------------------------------------------------------------*/

/* Public function prototypes --------------------------------------------------------------------*/

/**
 * @brief Open zerodet module
 *
 * @retval ZERR_OK if successful
 * @retval ZERR_FLASE if failed
 */
zerr_t
zerodet_open(void);

/**
 * @brief Update zerodet middleware
 */
void
zerodet_update(void);

/**
 * @brief Check zerodet status
 *
 * @retval true if ready
 * @retval false if not ready
 */
bool
zerodet_ready(void);

/**
 * @brief Get number of detected zerodet edges
 *
 * @retval Number of detected edges
 */
uint32_t
zerodet_get_event_count(void);

/**
 * @brief Get current Zerodet logic state
 *
 * @retval true if HIGH
 * @retval flase if HIGH
 */
bool
zerodet_get_state(void);

/**
 * @brief Check whether a new Zerodet event occured
 *
 * @retval true if new event detected
 * @retval false if no new event
 */
bool
zerodet_event_available(void);

/**
 * @brief Clear zerodet event counter
 */
void
zerodet_clear_event(void);

#ifdef __cplusplus
}
#endif

#endif /* __ZERODET_H_ */

/* END OF FILE ----------------------------------------------------------------*/
