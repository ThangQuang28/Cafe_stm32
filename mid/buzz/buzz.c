/**
 *******************************************************************************
 * @auth            : manhtd
 * @day             : 19-Sep-2024
 * @file            : ntc.c
 * @dissaption      : FWCore for STM32F0xx MCU designed by RDU
 *******************************************************************************
 */

/*  Indent using spaces
    Tab width: 4 spaces
    Line width: 100 characters
    Line ending: LF (0x0A) */

/* Public macros ---------------------------------------------------------------------------------*/

/* Include ---------------------------------------------------------------------------------------*/

/* Middleware */
#include "buzz.h"
#include "zerr.h"

/* BSP */
#include "buzz_bsp.h"

/* Define ----------------------------------------------------------------------------------------*/

/* Private data types ----------------------------------------------------------------------------*/

/* Public data types -----------------------------------------------------------------------------*/

/* Exported constants ----------------------------------------------------------------------------*/

/* Private data ----------------------------------------------------------------*/

/* Private function prototypes -------------------------------------------------------------------*/

/* Public function prototypes --------------------------------------------------------------------*/

/**
 * @brief	Open buzzer
 * @retval	ZERR_OK is successful
 * @retval	ZERR_FALSE if failed
 */
zerr_t
buzz_open(void)
{
	buzz_bsp_open();
	return ZERR_OK;
}

/**
 * @brief	Enable CLK buzzer
 * @retval	None
 */

/**
 * @brief	Turn on buzzer
 * @retval	None
 */
void
buzz_on(void)
{
	buzz_bsp_on();
}

/**
 * @brief	Turn off buzzer
 * @retval	None
 */
void
buzz_off(void)
{
	buzz_bsp_off();
}


/* END OF FILE ************************************************************************************/
