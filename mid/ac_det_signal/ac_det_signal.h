/**
 * -------------------------------------------------------------------------------
 * @auth            : thangquang
 * @day             : 12-Aug-2026
 * @file            : ac_det_signal.h
 * @description     : AC Detection Signal Middleware
 * -------------------------------------------------------------------------------
 */

/* Indent using spaces
 * Tab width   : 4 spaces
 * Line width  : 100 characters
 * Line ending : LF (0x0A)
 */

/* Prevent recursive inclusion ------------------------------------------------ */

#ifndef __AC_DET_SIGNAL_H_
#define __AC_DET_SIGNAL_H_

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

typedef enum
{
	AC_DET_SIGNAL_ABSENT = 0U,
	AC_DET_SIGNAL_PRESENT
} ac_det_signal_state_t;

/* Exported constants ----------------------------------------------------------------------------*/

/* Private function prototypes -------------------------------------------------------------------*/

/* Public function prototypes --------------------------------------------------------------------*/

/**
 * @brief Open AC detection signal module
 *
 * @retval ZERR_OK if successful
 * @retval ZERR_FALSE if failed
 */
zerr_t
ac_det_signal_open(void);

/**
 * @brief Update AC detection signal
 */
void
ac_det_signal_update(void);

/**
 * @brief Check AC detection signal status
 *
 * @retval true if AC is present
 * @retval flase if AC is absent
 */
bool
ac_det_signal_ready(void);

/**
 * @brief Get AC detection signal state
 *
 * @retval AC_DET_SIGNAL_PRESENT if AC is detected
 * @retval AC_DET_SIGNAL_ABSENT if AC is not detected
 */
ac_det_signal_state_t
ac_det_signal_get_state(void);

/**
 * @brief Check whether AC is currently present
 *
 * @retval true if AC is present
 * @retval false if AC is absent
 */
bool
ac_det_signal_is_present(void);

/**
 * @brief Get raw AC detection state
 *
 * @retval Raw GPIO detection state
 */
bool
ac_det_signal_get_raw(void);

#ifdef __cplusplus
}
#endif

#endif /* __AC_DET_SIGNAL_H_ */

/* END OF FILE ----------------------------------------------------------------*/
