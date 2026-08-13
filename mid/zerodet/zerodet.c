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

/* Include --------------------------------------------------------------------*/

/* Middleware */
#include "zerodet.h"

/* BSP */
#include "zerodet_bsp.h"

/* Define ---------------------------------------------------------------------*/

/* Private data types ----------------------------------------------------------*/

/* Public data types -----------------------------------------------------------*/

/* Exported constants ----------------------------------------------------------*/

/* Private data ----------------------------------------------------------------*/
static uint32_t
zerodet_event_count_previous = 0U;

static bool
zerodet_event_available_flag = false;
/* Private function prototypes -------------------------------------------------*/

/* Public function bodies -----------------------------------------------------*/

/**
 * @brief      Open ZeroDet module.
 * @retval     ZERR_OK if successful.
 */
zerr_t
zerodet_open(void)
{
    zerr_t ret;
    zerodet_event_count_previous = 0U;
    zerodet_event_available_flag = false;
    ret = zerodet_bsp_open();

    return ret;
}

/**
 * @brief      Update ZeroDet middleware.
 * @retval     None.
 */
void
zerodet_update(void)
{
	uint32_t event_count;
	if(!zerodet_bsp_ready())
	{
		return;
	}

	event_count = zerodet_bsp_get_event_count();

	if(event_count != zerodet_event_count_previous)
	{
		zerodet_event_available_flag = true;
		zerodet_event_count_previous = event_count;
	}
	else
	{
		zerodet_event_available_flag = false;
	}
}

/**
 * @brief      Check ZeroDet status.
 * @retval     true if ready.
 */
bool
zerodet_ready(void)
{
    return zerodet_bsp_ready();
}

/**
 * @brief      Get ZeroDet event counter.
 * @retval     Number of detected edges.
 */
uint32_t
zerodet_get_event_count(void)
{
    return zerodet_bsp_get_event_count();
}

/**
 * @brief Get current zerodet GPIO state
 *
 * @retval true if HIGH
 * @retval false if LOW
 */
bool
zerodet_get_state(void)
{
	return zerodet_bsp_get_state();
}

/**
 * @brief Check whether a new zerodet event occured
 *
 * @retval true if new event detected
 * @retval false if no new event
 */
bool
zerodet_event_available(void)
{
	return zerodet_event_available_flag;
}

/**
 * @brief      Clear ZeroDet event counter.
 * @retval     None.
 */
void
zerodet_clear_event(void)
{
	zerodet_bsp_clear_event();
	zerodet_event_count_previous = 0U;
	zerodet_event_available_flag = false;
}

/* END OF FILE ----------------------------------------------------------------*/
