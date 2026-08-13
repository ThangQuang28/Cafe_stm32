 /**
 *******************************************************************************
 * @auth            : manhtd
 * @day             : 19-Sep-2024
 * @file            : btn.c
 * @dissaption       : FWCore for STM32F0xx MCU designed by RDU
 *******************************************************************************
 */

/*  Indent using spaces
    Tab width: 4 spaces
    Line width: 100 characters
    Line ending: LF (0x0A) */

/* Public macros ---------------------------------------------------------------------------------*/

/* Include ---------------------------------------------------------------------------------------*/

/* Middleware */
#include "btn.h"
#include "zerr.h"

/* BSP */
#include "btn_bsp.h"

/* C Library */
#include "stdbool.h"

/* Define ----------------------------------------------------------------------------------------*/

#define BTN_DEBOUNCE_TIME_MS        30U
#define BTN_HOLDING_TIME_MS         1000U

/* Private data types ----------------------------------------------------------------------------*/

typedef struct
{
    bool current_state;
    bool last_state;

    bool pressed_event;
    bool released_event;

    bool holding_event;

    uint32_t debounce_tick;
    uint32_t press_tick;

} btn_object_t;

/* Public data types -----------------------------------------------------------------------------*/

/* Exported constants ----------------------------------------------------------------------------*/

/* Private data ----------------------------------------------------------------*/

static btn_object_t btn_object[BTN_MAX];

/* Private function prototypes -------------------------------------------------------------------*/

/**
 * @brief      Read button state.
 * @param[in]  id: button ID.
 * @retval     true if pressed.
 * @retval     false if released.
 */
static bool
btn_read(btn_id_t id)
{
    if (id >= BTN_MAX)
    {
        return false;
    }

    /*
     * Button is active-low.
     *
     * GPIO_PIN_RESET -> pressed
     * GPIO_PIN_SET   -> released
     */
    return (
        btn_bsp_read((uint8_t)id) == GPIO_PIN_RESET);
}

/* Public function prototypes --------------------------------------------------------------------*/

/**
 * @brief      Cfg button.
 * @retval     ZERR_OK if successful.
 * @retval     ZERR_FALSE if failed.
 */
zerr_t
btn_cfg(void)
{
    zerr_t ret;

    ret = btn_bsp_cfg();

    for (uint8_t i = 0U; i < BTN_MAX; i++)
    {
        btn_object[i].current_state = false;
        btn_object[i].last_state = false;

        btn_object[i].pressed_event = false;
        btn_object[i].released_event = false;
        btn_object[i].holding_event = false;

        btn_object[i].debounce_tick = 0U;
        btn_object[i].press_tick = 0U;
    }

    return ret;
}

/**
 * @brief      Enable CLK button.
 * @retval     None.
 */
void
btn_enable(void)
{
    btn_bsp_enable();
}

/**
 * @brief      Update button state.
 * @retval     None.
 */
void
btn_update(void)
{
    uint32_t tick;
    bool state;

    tick = HAL_GetTick();

    for (btn_id_t id = BTN_UP; id < BTN_MAX; id++)
    {
        state = btn_read(id);

        /*
         * Reset one-shot events.
         */
        btn_object[id].pressed_event = false;
        btn_object[id].released_event = false;

        /*
         * Detect state change.
         */
        if (state != btn_object[id].last_state)
        {
            btn_object[id].debounce_tick = tick;
            btn_object[id].last_state = state;
        }

        /*
         * Wait until debounce time has elapsed.
         */
        if ((tick - btn_object[id].debounce_tick) <
            BTN_DEBOUNCE_TIME_MS)
        {
            continue;
        }

        /*
         * Update stable state.
         */
        if (state != btn_object[id].current_state)
        {
            btn_object[id].current_state = state;

            if (state)
            {
                /*
                 * Button has been pressed.
                 */
                btn_object[id].pressed_event = true;
                btn_object[id].holding_event = false;
                btn_object[id].press_tick = tick;
            }
            else
            {
                /*
                 * Button has been released.
                 */
                btn_object[id].released_event = true;
                btn_object[id].holding_event = false;
                btn_object[id].press_tick = 0U;
            }
        }

        /*
         * Check holding state.
         */
        if (btn_object[id].current_state)
        {
            if ((tick - btn_object[id].press_tick) >=
                BTN_HOLDING_TIME_MS)
            {
                btn_object[id].holding_event = true;
            }
        }
    }
}

/**
 * @brief      Check button pressed event.
 * @param[in]  id: button ID.
 * @retval     true if button was pressed.
 * @retval     false if no pressed event.
 */
bool
btn_pressed(btn_id_t id)
{
    if (id >= BTN_MAX)
    {
        return false;
    }

    return btn_object[id].pressed_event;
}

/**
 * @brief      Check button released event.
 * @param[in]  id: button ID.
 * @retval     true if button was released.
 * @retval     false if no released event.
 */
bool
btn_released(btn_id_t id)
{
    if (id >= BTN_MAX)
    {
        return false;
    }

    return btn_object[id].released_event;
}

/**
 * @brief      Check button holding state.
 * @param[in]  id: button ID.
 * @retval     true if button is holding.
 * @retval     false if button is not holding.
 */
bool
btn_holding(btn_id_t id)
{
    if (id >= BTN_MAX)
    {
        return false;
    }

    return btn_object[id].holding_event;
}

/* END OF FILE ----------------------------------------------------------------*/
