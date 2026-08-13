/**
  **************************************************************************************************
  * @file   delay_bsp.h
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

#ifndef _DELAY_BSP_H
#define _DELAY_BSP_H

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
/**
	Delay uint defintion
*/
typedef enum {
		DELAY_BSP_MS = 0u, /* Milisecond */
		DELAY_BSP_US			 /* Microsecond */
} delay_bsp_unit_t;

/* Public function prototypes --------------------------------------------------------------------*/
zerr_t delay_bsp_open (void);
zerr_t delay_bsp_exec (const uint32_t time, const delay_bsp_unit_t unit);

#ifdef __cplusplus
}
#endif

#endif /* _DELAY_BSP_H */

/* END OF FILE ************************************************************************************/
