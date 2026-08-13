/**
  **************************************************************************************************
  * @file   led.h
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

#ifndef _LED_H
#define _LED_H

#ifdef __cplusplus
extern "C" {
#endif

/* Public macros ---------------------------------------------------------------------------------*/
/* Public data types -----------------------------------------------------------------------------*/

/* LED definition. */
typedef enum
{
    LED_ERROR,
    LED_HEARTBEAT,
	BUZZER
} led_type_t;

/* Public function prototypes --------------------------------------------------------------------*/

/**
  * @brief  Configures all LEDs.
  * @param  None.
  * @retval None.
  */
void
led_open (void);

/**
  * @brief      Turns the LED on.
  * @param[in]  type: type to be set on.
  * @retval     None.
  */
void
led_on (led_type_t type);

/**
  * @brief      Turns the LED off.
  * @param[in]  type: type to be set off.
  * @retval     None.
  */
void
led_off (led_type_t type);

/**
  * @brief      Toggles the LED.
  * @param[in]  type: type to be toggled.
  * @retval     None.
  */
void
led_toggle (led_type_t type);

#ifdef __cplusplus
}
#endif

#endif /* _LED_H */

/* END OF FILE ************************************************************************************/
