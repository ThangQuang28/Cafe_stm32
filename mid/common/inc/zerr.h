/**
  **************************************************************************************************
  * @file   err.h
  * @brief  Starting point of this project.
  * @by     duonghd | 31-Dec-2024 | Launch for WhiteCat-FWCore.
  **************************************************************************************************
  * This file is part of WhiteCat-FWCore.
  **************************************************************************************************
  */
/*  Indent using spaces
    Tab width   : 4 spaces
    Line width  : 100 characters
    Line ending : LF (0x0A) */

#ifndef _ZERR_H
#define _ZERR_H

#ifdef __cplusplus
extern "C" {
#endif

/* Public macros ---------------------------------------------------------------------------------*/
#define ZERR(x)             err_get_str_err(x)

/* Public data types -----------------------------------------------------------------------------*/

/* Error code definition. */
typedef enum
{
    /* Common error code. */
    ZERR_OK                         = 0u,   /* Success. */
    ZERR_OPEN_ALREADY               = 1u,   /* Has been already opened. */
    ZERR_INVALID_PARAM              = 2u,   /* Parameter is NULL. */
    ZERR_MEM_OUT,                           /* Out of memory. */
    ZERR_TIME_OUT,                          /* Out of time. */
    ZERR_NOT_OPEN,                          /* Has not been opened yet or unable to open. */
    ZERR_STORAGE_OUT,                       /* Out of storage. */
    ZERR_EXIST,                             /* Item exists. */
	ZERR_FALSE,
    
    ZERR_READ,                              /**< Read fail */
    ZERR_WRITE,                             /**< Write fail */
    ZERR_AUTHEN,                            /**< Not authenticate */
    ZERR_CRC,                               /**< Checksum error */
    
    ZERR_SD_UNLINK,							/* SD Card un link */
    
    ZERR_LCD_DISCONNECT,						/* LCD could not connect */
    ZERR_OLED_DISCONNECT,					/* OLED could not connect */
    
    ZERR_NOT_FOUND_DEVICE,					/* Could not found adrress of devices */
	
} zerr_t;

/* Public function prototypes --------------------------------------------------------------------*/

/**
     * @brief      Convert an error to a string representation.
     * @param[in]  error_code: an integer indicates an error.
     * @retval     A string representation for such error.
     */
const char*
err_get_str_err (zerr_t error_code);

#ifdef __cplusplus
}
#endif

#endif /* _VERR_H */

/* END OF FILE ************************************************************************************/
