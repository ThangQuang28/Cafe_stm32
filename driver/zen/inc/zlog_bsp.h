/**
  **************************************************************************************************
  * @file   zlog_bsp.h
  * @brief  Starting point of this project.
  * @by     duonghd | 1-Nov-2024 | Launch for WhiteCat-FWCore.
  **************************************************************************************************
  * Fiverr: https://www.fiverr.com/duonghd_catw
  **************************************************************************************************
  */

/*  Indent using spaces
    Tab width   : 4 spaces
    Line width  : 100 characters
    Line ending : LF (0x0A) */

#ifndef _ZLOG_BSP_H
#define _ZLOG_BSP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes --------------------------------------------------------------------------------------*/

/* Global configuration. */
#include "zerr.h"

/* FreeRTOS */
#include "FreeRTOS.h"
#include "queue.h"

/* Public macros ---------------------------------------------------------------------------------*/

/* Public data types -----------------------------------------------------------------------------*/

/* Public function prototypes --------------------------------------------------------------------*/

/**
  * @brief      Initialize logger bsp.
  * @param[in]  p_q_tx: user transmit queue.
  * @param[in]  baud: transfer baud rate.
  * @retval     zerr_t.
  */
zerr_t
log_bsp_open (const QueueHandle_t* p_q_tx, const uint32_t baud);

/**
  * @brief  Enable UART TXE interrupt for interrupt process.
  * @param  None.
  * @retval None.
  */
void log_bsp_enb_tx (void);

/**
  * @brief  Handles LOG UART interrupt request.
  * @param  None.
  * @retval None.
  */
void
log_bsp_uart_isr (void);

#ifdef __cplusplus
}
#endif

#endif /* _ZLOG_BSP_H */

/* END OF FILE ************************************************************************************/
