/**
 *******************************************************************************
 * @auth            : manhtd
 * @day             : 19-Sep-2024
 * @file            : buzzer_bsp.h
 * @dissaption       : FWCore for STM32F0xx MCU designed by RDU
 *******************************************************************************
 */

/*  Indent using spaces
    Tab width: 4 spaces
    Line width: 100 characters
    Line ending: LF (0x0A) */

/* Prevent recursive inclusion ------------------------------------------------*/

#ifndef __BUZZ_BSP_H_
#define __BUZZ_BSP_H_
#ifdef __cplusplus
extern "C" {
#endif

/* Public macros ---------------------------------------------------------------------------------*/

/* Include ---------------------------------------------------------------------------------------*/

/* HAL Library */
#include "stm32f0xx_hal.h"

/* Global */
#include "zerr.h"

/* Define ----------------------------------------------------------------------------------------*/

/* Private data types ----------------------------------------------------------------------------*/

/* Public data types -----------------------------------------------------------------------------*/

/* Exported constants ----------------------------------------------------------------------------*/

/* Private function prototypes -------------------------------------------------------------------*/

/* Public function prototypes --------------------------------------------------------------------*/

/**
 * @brief	Open buzzer GPIO
 * @retbal	ZERR_OK if successful
 * @retval	ZERR_FALSE if failed
 */
zerr_t
buzz_bsp_open(void);


/**
 * @brief	Turn on buzzer
 * @retval	None
 */
void
buzz_bsp_on(void);

/**
 * @brief	Turn off buzzer
 * @retval	None
 */
void
buzz_bsp_off(void);


#ifdef	__cplusplus
}
#endif

#endif /* __BUZZ_BSP_H_ */

/* END OF FILE ****************************************************************/

