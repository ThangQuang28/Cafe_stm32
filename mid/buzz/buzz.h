/**
 *******************************************************************************
 * @auth            : teebee
 * @day             : 19-Sep-2024
 * @file            : buzz.h
 *******************************************************************************
 */

/*  Indent using spaces
    Tab width: 4 spaces
    Line width: 100 characters
    Line ending: LF (0x0A) */

#ifndef __BUZZ_H_
#define __BUZZ_H_
#ifdef __cplusplus
extern "C" {
#endif

#include	"zerr.h"


/**
 * @brief	Open buzzer
 * @retval	ZERR_OK if successful
 * @retval	ZERR_FALSE if failed
 */
zerr_t
buzz_open(void);

/**
 * @brief	Turn on buzzer
 * @retval	None
 */
void
buzz_on(void);

/**
 * @brief	Turn off buzzer
 * @retval	None
 */
void
buzz_off(void);


#ifdef __cplusplus
}
#endif

#endif /* __BUZZ_H_ */

/* END OF FILE ************************************************************************************/
