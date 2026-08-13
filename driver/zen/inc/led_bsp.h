/**
  **************************************************************************************************
  * @file   led_bsp.h
  * @brief  Starting point of this project.
  * @by     duonghd | 31-Dec-2024 | Launch for WhiteCat-FWCore.
  **************************************************************************************************
  * Fiverr: https://www.fiverr.com/duonghd_catw
  **************************************************************************************************
  */

/*  Indent using spaces
    Tab width   : 4 spaces
    Line width  : 100 characters
    Line ending : LF (0x0A) */

#ifndef _LED_BSP_H
#define _LED_BSP_H

/* Includes --------------------------------------------------------------------------------------*/
/* Middleware. */
#include "led.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Public macros ---------------------------------------------------------------------------------*/

/* Public data types -----------------------------------------------------------------------------*/

/* Public function prototypes --------------------------------------------------------------------*/

/**
  * @brief      Configures GPIO relating to LED.
  * @param[in]  type: type to be configured.
  * @retval     None.
  */
void
led_bsp_open (led_type_t type);

/**
  * @brief      Turns the LED on.
  * @param[in]  type: type to be set on.
  * @retval     None.
  */
void
led_bsp_on (led_type_t type);

/**
  * @brief      Turns the LED off.
  * @param[in]  type: type to be set off.
  * @retval     None.
  */
void
led_bsp_off (led_type_t type);

/**
  * @brief      Toggles the LED.
  * @param[in]  type: type to be toggled.
  * @retval     None.
  */
void
led_bsp_toggle (led_type_t type);

#ifdef __cplusplus
}
#endif

#endif /* _LED_BSP_H */
/* END OF FILE ************************************************************************************/
