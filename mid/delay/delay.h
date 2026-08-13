/**
  **************************************************************************************************
  * @file   delay.h
  * @brief  Starting point of this project.
  * @by     phucdh141 | 31-Dec-2024 | Launch for FWCore.
  **************************************************************************************************
  * This file is part of FWCore.
  **************************************************************************************************
  */


/*  Indent using spaces
    Tab width: 4 spaces
    Line width: 100 characters
    Line ending: LF (0x0A) */

#ifndef _DELAY_H
#define _DELAY_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes --------------------------------------------------------------------------------------*/

/* Global configuration. */
#include "zerr.h"

/* C standard library. */
#include <stdint.h>

/* Public macros ---------------------------------------------------------------------------------*/

/* Public data types -----------------------------------------------------------------------------*/

/* Public function prototypes --------------------------------------------------------------------*/

zerr_t delay_open (void);
zerr_t delay_ms (uint32_t num_ms);
zerr_t delay_us (uint32_t num_us);

#ifdef __cplusplus
}
#endif

#endif /* _DELAY_H */

/* END OF FILE ************************************************************************************/
