
/**
  **************************************************************************************************
  * @file   zapp.h
  * @brief  Starting point of this project.
  * @by     duonghd | 23-Dec-2024 | Launch for WhiteCat-FWCore.
  **************************************************************************************************
  * This file is part of WhiteCat-FWCore.
  **************************************************************************************************
  */

/*  Indent using spaces
    Tab width   : 4 spaces
    Line width  : 100 characters
    Line ending : LF (0x0A) */

#ifndef _ZAPP_H
#define _ZAPP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes --------------------------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
#include "pid.h"
/* Private macros --------------------------------------------------------------------------------*/
/* Private data types ----------------------------------------------------------------------------*/
/* Private variables -----------------------------------------------------------------------------*/
/* Private function prototypes -------------------------------------------------------------------*/
/* Public function bodies ------------------------------------------------------------------------*/
/**
  * @brief  Starting point of the application.
  * @note   If a FreeRTOS API function is called before the scheduler has been started then
  *         interrupts will deliberately be left disabled, and not re-enable again until the first
  *         task starts to execute. (http://www.freertos.org/FAQHelp.html)
  * @param  None.
  * @retval None.
  */
void
zapp_open (void);

/* Private function bodies -----------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* _ZAPP_H */

/* END OF FILE ************************************************************************************/
