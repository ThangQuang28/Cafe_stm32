/**
 * -----------------------------------------------------------------------------
 * @auth            : thangquang
 * @day             : 19-Sep-2024
 * @file            : ntc_bsp.h
 * @description     : NTC BSP for STM32F0xx MCU
 * -----------------------------------------------------------------------------
 */

/* Indent using spaces
 * Tab width   : 4 spaces
 * Line width  : 100 characters
 * Line ending : LF (0x0A)
 */

/* Prevent recursive inclusion ------------------------------------------------*/
#ifndef __NTC_BSP_H
#define __NTC_BSP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Public macros ----------------------------------------------------------------*/

/* Include ----------------------------------------------------------------------*/
#include "stdbool.h"
#include "stdint.h"
/* HAL Library */
#include "stm32f0xx_hal.h"

/* Global */
#include "zerr.h"

/* Define -----------------------------------------------------------------------*/

/* Private data types -----------------------------------------------------------*/

/* Public data types ------------------------------------------------------------*/

/* Exported constants -----------------------------------------------------------*/

/* Private function prototypes -------------------------------------------------*/

/* Public function prototypes --------------------------------------------------*/

/**
 * @brief      Open NTC ADC and DMA.
 * @retval     ZERR_OK    if successful.
 * @retval     ZERR_FALSE if failed.
 */
zerr_t
ntc_bsp_open(void);

/**
 * @brief      Start ADC DMA conversion.
 * @retval     ZERR_OK    if successful.
 * @retval     ZERR_FALSE if failed.
 */
zerr_t
ntc_bsp_start(void);

/**
 * @brief      Check ADC DMA data ready.
 * @retval     true  if data is ready.
 * @retval     false if data is not ready.
 */
bool
ntc_bsp_ready(void);

/**
 * @brief      Get raw ADC value.
 * @param[in]  channel: NTC DMA channel index.
 * @retval     Raw ADC value.
 */
uint16_t
ntc_bsp_get_raw(uint8_t channel);

/**
 * @brief      Clear ADC DMA ready flag.
 * @retval     None.
 */
void
ntc_bsp_clear_ready(void);

/**
 * @brief      Handle NTC DMA interrupt.
 * @retval     None.
 */
void
ntc_bsp_dma_isr(void);

#ifdef __cplusplus
}
#endif

#endif

/* END OF FILE ----------------------------------------------------------------*/
