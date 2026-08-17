/**
 * ----------------------------------------------------------------------------
 * @auth            : manhtd
 * @day             : 19-Sep-2024
 * @file            : ntc.h
 * @description     : NTC middleware
 * ----------------------------------------------------------------------------
 */

#ifndef __NTC_H
#define __NTC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Include ---------------------------------------------------------------------*/

#include "zerr.h"
#include "stdbool.h"
#include "stdint.h"

/* Public data types -----------------------------------------------------------*/

typedef enum
{
    NTC_TEMP_T = 0U,
    NTC_BOILER_T,
    NTC_MAX_T

} ntc_type_t;

/* Public function prototypes --------------------------------------------------*/

/**
 * @brief      Open NTC sensor.
 * @retval     ZERR_OK       if successful.
 * @retval     ZERR_FALSE    if failed.
 */
zerr_t
ntc_open(void);

/**
 * @brief      Start NTC ADC DMA.
 * @retval     ZERR_OK       if successful.
 * @retval     ZERR_FALSE    if failed.
 */
zerr_t
ntc_start(void);

/**
 * @brief      Update NTC temperature.
 * @retval     None.
 */
void
ntc_update(void);

/**
 * @brief      Get boiler temperature.
 * @retval     Temperature in Celsius.
 */
float
read_temp_boiler(void);

/**
 * @brief      Get temperature sensor value.
 * @retval     Temperature in Celsius.
 */
float
read_temp_sensor(void);

/**
 * @brief      Get raw ADC value.
 * @param[in]  type: NTC type.
 * @retval     Raw ADC value.
 */
uint16_t
ntc_get_raw(ntc_type_t type);

/**
 * @brief      Check NTC error.
 * @param[in]  type: NTC type.
 * @retval     true  if error.
 * @retval     false if normal.
 */
bool
ntc_error(ntc_type_t type);

#ifdef __cplusplus
}
#endif

#endif

/* END OF FILE ----------------------------------------------------------------*/
