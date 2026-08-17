/**
 *******************************************************************************
 * @auth            : thangquang
 * @day             : 19-Sep-2024
 * @file            : ntc.h
 * @dissaption      : FWCore for STM32F0xx MCU designed by RDU
 *******************************************************************************
 */

/*  Indent using spaces
    Tab width: 4 spaces
    Line width: 100 characters
    Line ending: LF (0x0A) */

/* Prevent recursive inclusion ------------------------------------------------*/
#ifndef __BTN_H_
#define __BTN_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Public macros ---------------------------------------------------------------------------------*/

/* Include ---------------------------------------------------------------------------------------*/

/* Global configuration */
#include "zerr.h"
#include "stdbool.h"

/* Define ----------------------------------------------------------------------------------------*/

/* Private data types ----------------------------------------------------------------------------*/

/* Public data types -----------------------------------------------------------------------------*/

typedef enum
{
	BTN_UP = 0U,
	BTN_DOWN,
	BTN_MAX
} btn_id_t;

/* Exported constants ----------------------------------------------------------------------------*/

/* Private function prototypes -------------------------------------------------------------------*/

/* Public function prototypes --------------------------------------------------------------------*/

/**
 * @brief		Cfg button
 * @retval		ZERR_OK if successful
 * @retval		ZERR_FALSE if failed
 */
zerr_t
btn_open(void);

/**
 * @brief		Enable BLK button
 * @retval		None
 */
void
btn_enable(void);

/**
 * @brief		Update button state
 * @retval		None
 */
void
btn_update(void);

/**
 * @brief		Check button pressed event
 * @param[in]	id: button ID
 * @retval		true if button was pressed
 * @retval		false if no pressed event
 */
bool
btn_pressed(btn_id_t id);

/**
 * @brief		Check button released event
 * @param[in]	id: button ID
 * @retval		true if button was released
 * @retval		false if no released event
 */
bool
btn_released(btn_id_t id);

/**
 * @brief		Check button holding state
 * @param[in]	id: button ID
 * @retval		true if button is holding
 * @retval		false if button is not holding
 */
bool
btn_holding(btn_id_t id);

#ifdef __cplusplus
}
#endif

#endif /* __BTN_H_ */

/* END OF FILE ****************************************************************/
