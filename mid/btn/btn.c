/**
 *******************************************************************************
 * @auth            : thangquang
 * @day             : 19-Sep-2024
 * @file            : btn.c
 * @description     : Button middleware for STM32F0xx MCU
 *******************************************************************************
 */

/* Include -------------------------------------------------------------------*/

#include "btn.h"
#include "btn_bsp.h"
#include "zerr.h"

#include "stdbool.h"
#include "stdint.h"

/* Define --------------------------------------------------------------------*/

/*
 * Mechanical button debounce time.
 *
 * 10 ms gives faster response than the previous 30 ms,
 * while still providing basic switch bounce protection.
 */
#define BTN_DEBOUNCE_TIME_MS       5U

/*
 * HOLD event time.
 */
#define BTN_HOLDING_TIME_MS        1000U

/* Private data types --------------------------------------------------------*/

/**
 * @brief Button internal object.
 */
typedef struct
{
    /*
     * Raw GPIO logical state.
     *
     * true  -> pressed
     * false -> released
     */
    volatile bool raw_state;

    /*
     * Debounced stable state.
     */
    bool stable_state;

    /*
     * Debounce control.
     */
    volatile bool debounce_pending;
    volatile uint32_t debounce_tick;

    /*
     * Press timing.
     */
    uint32_t press_tick;

    /*
     * HOLD event has already been generated.
     */
    bool hold_reported;

    /*
     * One-shot events.
     */
    volatile bool pressed_event;
    volatile bool released_event;
    volatile bool holding_event;

} btn_object_t;

/* Private data --------------------------------------------------------------*/

static btn_object_t btn_object[BTN_MAX];

/* Private functions ---------------------------------------------------------*/

/**
 * @brief      Read logical button state.
 *
 * Hardware:
 *
 *     GPIO HIGH -> released
 *     GPIO LOW  -> pressed
 *
 * @param[in]  id Button ID.
 *
 * @retval     true  if pressed.
 * @retval     false if released.
 */
static bool btn_read(btn_id_t id)
{
    if (id >= BTN_MAX)
    {
        return false;
    }

    return (
        btn_bsp_read((uint8_t)id) == GPIO_PIN_RESET
    );
}

/**
 * @brief      Start debounce process.
 *
 * Called from EXTI context.
 *
 * @param[in] id   Button ID.
 * @param[in] tick Current system tick.
 */
static void btn_start_debounce(
    btn_id_t id,
    uint32_t tick)
{
    if (id >= BTN_MAX)
    {
        return;
    }

    /*
     * Capture GPIO state immediately.
     */
    btn_object[id].raw_state = btn_read(id);

    /*
     * Start debounce timer.
     */
    btn_object[id].debounce_tick = tick;
    btn_object[id].debounce_pending = true;
}

/* Public functions ----------------------------------------------------------*/

/**
 * @brief      Initialize button module.
 *
 * @retval     ZERR_OK if successful.
 */
zerr_t btn_open(void)
{
    zerr_t ret;
    uint32_t tick;

    /*
     * Initialize hardware.
     */
    ret = btn_bsp_open();

    if (ret != ZERR_OK)
    {
        return ret;
    }

    tick = HAL_GetTick();

    /*
     * Initialize software state from actual GPIO state.
     */
    for (btn_id_t id = BTN_UP; id < BTN_MAX; id++)
    {
        bool state;

        state = btn_read(id);

        btn_object[id].raw_state = state;
        btn_object[id].stable_state = state;

        btn_object[id].debounce_pending = false;
        btn_object[id].debounce_tick = tick;

        /*
         * If the button is already pressed during startup,
         * start HOLD timing from startup.
         */
        if (state)
        {
            btn_object[id].press_tick = tick;
        }
        else
        {
            btn_object[id].press_tick = 0U;
        }

        btn_object[id].hold_reported = false;

        btn_object[id].pressed_event = false;
        btn_object[id].released_event = false;
        btn_object[id].holding_event = false;
    }

    return ZERR_OK;
}

/**
 * @brief      EXTI callback.
 *
 * ISR responsibilities:
 *
 *     1. Identify button.
 *     2. Capture GPIO state.
 *     3. Start debounce.
 *
 * No delay.
 * No application logic.
 */
void btn_exti_callback(uint16_t GPIO_Pin)
{
    uint32_t tick;

    tick = HAL_GetTick();

    switch (GPIO_Pin)
    {
        case BTN_UP_Pin:
        {
            btn_start_debounce(
                BTN_UP,
                tick);

            break;
        }

        case BTN_DOWN_Pin:
        {
            btn_start_debounce(
                BTN_DOWN,
                tick);

            break;
        }

        default:
        {
            break;
        }
    }
}

/**
 * @brief      HAL EXTI callback.
 *
 * This function is called by HAL when EXTI interrupt occurs.
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    btn_exti_callback(GPIO_Pin);
}

/**
 * @brief      Update button state machine.
 *
 * Recommended call period:
 *
 *     1 ~ 5 ms
 *
 * The button response time is approximately:
 *
 *     debounce time + update period
 *
 * With:
 *
 *     debounce = 10 ms
 *     update   = 5 ms
 *
 * Response is approximately:
 *
 *     10 ~ 15 ms
 */
void btn_update(void)
{
    uint32_t tick;

    tick = HAL_GetTick();

    for (btn_id_t id = BTN_UP; id < BTN_MAX; id++)
    {
        bool state;

        /*
         * ---------------------------------------------------------------
         * Clear one-shot events from previous update cycle.
         * ---------------------------------------------------------------
         */
        btn_object[id].pressed_event = false;
        btn_object[id].released_event = false;
        btn_object[id].holding_event = false;

        /*
         * ---------------------------------------------------------------
         * DEBOUNCE
         * ---------------------------------------------------------------
         */
        if (btn_object[id].debounce_pending)
        {
            /*
             * Wait until debounce period expires.
             */
            if ((tick - btn_object[id].debounce_tick)
                < BTN_DEBOUNCE_TIME_MS)
            {
                continue;
            }

            /*
             * Read GPIO after debounce period.
             */
            state = btn_read(id);

            /*
             * GPIO changed during debounce.
             *
             * Restart debounce timer.
             */
            if (state != btn_object[id].raw_state)
            {
                btn_object[id].raw_state = state;
                btn_object[id].debounce_tick = tick;

                continue;
            }

            /*
             * GPIO remained stable.
             *
             * Debounce complete.
             */
            btn_object[id].debounce_pending = false;

            /*
             * Check whether stable state changed.
             */
            if (state != btn_object[id].stable_state)
            {
                btn_object[id].stable_state = state;

                /*
                 * -------------------------------------------------------
                 * PRESSED
                 * -------------------------------------------------------
                 */
                if (state)
                {
                    btn_object[id].pressed_event = true;

                    btn_object[id].press_tick = tick;

                    btn_object[id].hold_reported = false;
                }

                /*
                 * -------------------------------------------------------
                 * RELEASED
                 * -------------------------------------------------------
                 */
                else
                {
                    btn_object[id].released_event = true;

                    btn_object[id].press_tick = 0U;

                    btn_object[id].hold_reported = false;
                }
            }
        }

        /*
         * ---------------------------------------------------------------
         * HOLD
         * ---------------------------------------------------------------
         *
         * Generate HOLD only once for each press.
         */
        if (btn_object[id].stable_state &&
            !btn_object[id].hold_reported)
        {
            if ((tick - btn_object[id].press_tick)
                >= BTN_HOLDING_TIME_MS)
            {
                btn_object[id].holding_event = true;

                btn_object[id].hold_reported = true;
            }
        }
    }
}

/**
 * @brief      Get current stable button state.
 */
bool btn_is_pressed(btn_id_t id)
{
    if (id >= BTN_MAX)
    {
        return false;
    }

    return btn_object[id].stable_state;
}

/**
 * @brief      Check pressed event.
 */
bool btn_pressed(btn_id_t id)
{
    if (id >= BTN_MAX)
    {
        return false;
    }

    return btn_object[id].pressed_event;
}

/**
 * @brief      Check released event.
 */
bool btn_released(btn_id_t id)
{
    if (id >= BTN_MAX)
    {
        return false;
    }

    return btn_object[id].released_event;
}

/**
 * @brief      Check holding event.
 */
bool btn_holding(btn_id_t id)
{
    if (id >= BTN_MAX)
    {
        return false;
    }

    return btn_object[id].holding_event;
}

/* END OF FILE ----------------------------------------------------------------*/
