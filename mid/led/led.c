/**
  **************************************************************************************************
  * @file   led.c
  * @brief  Starting point of this project.
  * @by     duonghd | 31-Dec-2024 | Launch for WhiteCat-FWCore.
  **************************************************************************************************
  * This file is part of WhiteCat-FWCore.
  **************************************************************************************************
  */

/*  Indent using spaces
    Tab width   : 4 spaces
    Line width  : 100 characters
    Line ending : LF (0x0A) */

/* Includes --------------------------------------------------------------------------------------*/

/* Global configuration. */
/* #include "cfg.h" */

/* BSP. */
#include "led_bsp.h"

/* Private macros --------------------------------------------------------------------------------*/
/* Private data types ----------------------------------------------------------------------------*/
/* Private variables -----------------------------------------------------------------------------*/
/* Private function prototypes -------------------------------------------------------------------*/
/* Public function bodies ------------------------------------------------------------------------*/

/**
  * @brief  Configures all LEDs.
  * @param  None.
  * @retval None.
  */
void
led_open (void)
{
    led_bsp_open(LED_ERROR);
    led_bsp_open(LED_HEARTBEAT);
	led_bsp_open(BUZZER);
    return;
}

/**
  * @brief      Turns the LED on.
  * @param[in]  type: type to be set on.
  * @retval     None.
  */
void
led_on (led_type_t type)
{
    led_bsp_on(type);
    return;
}

/**
  * @brief      Turns the LED off.
  * @param[in]  type: type to be set off.
  * @retval     None.
  */
void
led_off (led_type_t type)
{
    led_bsp_off(type);
    return;
}

/**
  * @brief      Toggles the LED.
  * @param[in]  type: type to be toggled.
  * @retval     None.
  */
void
led_toggle (led_type_t type)
{
    led_bsp_toggle(type);
    return;
}

/* Private function bodies -----------------------------------------------------------------------*/

/* END OF FILE ************************************************************************************/
