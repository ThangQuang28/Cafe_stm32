/**
 * -----------------------------------------------------------------------------
 * @file        zerodet.c
 * @author      thangquang
 * @day         12-Aug-2026
 * @description ZeroDet middleware.
 *
 *              Middleware khong truy cap truc tiep GPIO/EXTI.
 *              Moi thao tac phan cung duoc thuc hien thong qua BSP.
 * -----------------------------------------------------------------------------
 */

/* Include -------------------------------------------------------------------- */

/* Middleware */
#include "ac_det_signal.h"

/* BSP */
#include "ac_det_signal_bsp.h"

/* STM32 HAL */
#include "stm32f0xx_hal.h"

/* Define --------------------------------------------------------------------- */

#define AC_DET_SIGNAL_FILTER_TIME_MS       20U

/* Private data types ---------------------------------------------------------- */

/* Public data types ----------------------------------------------------------- */

/* Exported constants ---------------------------------------------------------- */

/* Private data ---------------------------------------------------------------- */

static volatile bool
ac_det_signal_initialized = false;

static volatile bool
ac_det_signal_raw_state = false;

static volatile bool
ac_det_signal_state = false;

static volatile uint32_t
ac_det_signal_change_tick = 0U;

/* Private function prototypes ------------------------------------------------- */

/* Public function bodies ----------------------------------------------------- */

/**
 * @brief Open AC detection module.
 *
 * @retval ZERR_OK    if successful.
 * @retval ZERR_FALSE if failed.
 */
zerr_t
ac_det_signal_open(void)
{
    zerr_t ret;

    ac_det_signal_initialized = false;

    ac_det_signal_raw_state = false;

    ac_det_signal_state = false;

    ac_det_signal_change_tick = 0U;

    ret =
        ac_det_signal_bsp_open();

    if (ret != ZERR_OK)
    {
        return ret;
    }

    /*
     * Lay trang thai GPIO ban dau.
     */
    ac_det_signal_raw_state =
        ac_det_signal_bsp_read();

    ac_det_signal_state =
        ac_det_signal_raw_state;

    ac_det_signal_change_tick =
        HAL_GetTick();

    ac_det_signal_initialized = true;

    return ZERR_OK;
}

/**
 * @brief Update AC detection state.
 *
 * @note Must be called periodically.
 */
void
ac_det_signal_update(void)
{
    bool raw_state;
    uint32_t current_tick;

    if (!ac_det_signal_initialized)
    {
        return;
    }

    raw_state =
        ac_det_signal_bsp_read();

    current_tick =
        HAL_GetTick();

    /*
     * Raw state has changed.
     */
    if (raw_state != ac_det_signal_raw_state)
    {
        ac_det_signal_raw_state =
            raw_state;

        ac_det_signal_change_tick =
            current_tick;

        return;
    }

    /*
     * Raw state remains stable.
     *
     * Confirm the new state only after the configured
     * filter time has elapsed.
     */
    if (ac_det_signal_state != ac_det_signal_raw_state)
    {
        if ((uint32_t)(
                current_tick -
                ac_det_signal_change_tick) >=
            AC_DET_SIGNAL_FILTER_TIME_MS)
        {
            ac_det_signal_state =
                ac_det_signal_raw_state;
        }
    }
}

/**
 * @brief Check AC detection module status.
 *
 * @retval true  if module is ready.
 * @retval false if module is not ready.
 */
bool
ac_det_signal_ready(void)
{
    return ac_det_signal_initialized;
}

/**
 * @brief Get confirmed AC state.
 *
 * @retval AC_DET_SIGNAL_PRESENT if AC is present.
 * @retval AC_DET_SIGNAL_ABSENT  if AC is absent.
 */
ac_det_signal_state_t
ac_det_signal_get_state(void)
{
    if (!ac_det_signal_initialized)
    {
        return AC_DET_SIGNAL_ABSENT;
    }

    if (ac_det_signal_state)
    {
        return AC_DET_SIGNAL_PRESENT;
    }

    return AC_DET_SIGNAL_ABSENT;
}

/**
 * @brief Check whether AC is currently present.
 *
 * @retval true  if AC is present.
 * @retval false if AC is absent.
 */
bool
ac_det_signal_is_present(void)
{
    return ac_det_signal_state;
}

/**
 * @brief Get raw AC detection state.
 *
 * @retval Raw GPIO detection state.
 */
bool
ac_det_signal_get_raw(void)
{
    return ac_det_signal_raw_state;
}

/* END OF FILE ---------------------------------------------------------------- */
