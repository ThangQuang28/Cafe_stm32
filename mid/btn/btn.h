/**
 *******************************************************************************
 * @auth            : thangquang
 * @day             : 19-Sep-2024
 * @file            : btn.h
 * @description     : Button middleware for STM32F0xx MCU
 *******************************************************************************
 */

#ifndef __BTN_H
#define __BTN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Include -------------------------------------------------------------------*/

#include "zerr.h"
#include "stdbool.h"
#include "stdint.h"

/* Public data types ---------------------------------------------------------*/

/**
 * @brief Button ID.
 */
typedef enum
{
    BTN_UP = 0U,
    BTN_DOWN,
    BTN_MAX

} btn_id_t;

/**
 * @brief Button event.
 */
typedef enum
{
    BTN_EVENT_NONE = 0U,
    BTN_EVENT_PRESSED,
    BTN_EVENT_RELEASED,
    BTN_EVENT_HELD

} btn_event_t;

/**
 * @brief Button stable state.
 */
typedef enum
{
    BTN_STATE_RELEASED = 0U,
    BTN_STATE_PRESSED

} btn_state_t;

/* Public function prototypes ------------------------------------------------*/

/**
 * @brief      Initialize button module.
 *
 * @retval     ZERR_OK    Initialization successful.
 * @retval     ZERR_FALSE Initialization failed.
 */
zerr_t
btn_open(void);

/**
 * @brief      Update button state machine.
 *
 * This function must be called periodically.
 *
 * Recommended period:
 *     1 ~ 5 ms
 */
void
btn_update(void);

/**
 * @brief      EXTI callback handler.
 *
 * This function is called from HAL_GPIO_EXTI_Callback().
 *
 * @param[in] GPIO_Pin
 */
void
btn_exti_callback(uint16_t GPIO_Pin);

/**
 * @brief      Check current stable button state.
 *
 * @param[in] id Button ID.
 *
 * @retval true  Button is pressed.
 * @retval false Button is released.
 */
bool
btn_is_pressed(btn_id_t id);

/**
 * @brief      Check pressed event.
 *
 * @param[in] id Button ID.
 *
 * @retval true  Button has just been pressed.
 * @retval false No pressed event.
 */
bool
btn_pressed(btn_id_t id);

/**
 * @brief      Check released event.
 *
 * @param[in] id Button ID.
 *
 * @retval true  Button has just been released.
 * @retval false No released event.
 */
bool
btn_released(btn_id_t id);

/**
 * @brief      Check holding event.
 *
 * HOLD is generated once after the button has been
 * continuously pressed for BTN_HOLDING_TIME_MS.
 *
 * @param[in] id Button ID.
 *
 * @retval true  HOLD event occurred.
 * @retval false No HOLD event.
 */
bool
btn_holding(btn_id_t id);

#ifdef __cplusplus
}
#endif

#endif /* __BTN_H */

/* END OF FILE ----------------------------------------------------------------*/
