/**
  **************************************************************************************************
  * @file   err.c
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

/* Includes --------------------------------------------------------------------------------------*/

/* Global configuration. */
#include "zcfg.h"

/* C Library */
#include <stdint.h>

/* Private macros --------------------------------------------------------------------------------*/
//#define ERR_MAX_LEN_STR_ERR          8 /* Maximum length of error string displayed on LCD. */

/* Private data types ----------------------------------------------------------------------------*/

/* Error information definition. */
typedef struct
{
    uint32_t offset;    /* Position of the string representation. */
    zerr_t   code;      /* Error code. */
} zerr_info_t;

/* Private variables -----------------------------------------------------------------------------*/

const uint32_t g_num_err = 14;    /* The number of elements in g_err_info. */

/* Error information. */
const zerr_info_t g_err_info[] = {
    {0, ZERR_OK},
    {1, ZERR_OPEN_ALREADY},
    {2, ZERR_INVALID_PARAM},
    {3, ZERR_MEM_OUT},
    {4, ZERR_TIME_OUT},
    {5, ZERR_NOT_OPEN},
    {6, ZERR_STORAGE_OUT},
    {7, ZERR_EXIST},
	{8, ZERR_SD_UNLINK},
	{9, ZERR_OLED_DISCONNECT},
	
    /* Modify g_num_err variable if add more any elements. */

};

/* String representation for error. */
static const char* gp_str_err[] = {
    /*  0  */ "ok.",
    /*  1  */ "has been already opened.",
    /*  2  */ "invalid parameter!",
    /*  3  */ "out of memory!",
    /*  4  */ "out of time!",
    /*  5  */ "not opened yet!",
    /*  6  */ "out of storage!",
    /*  7  */ "exist!",
    /*  8  */ "sd card unlink",
    /*  9  */ "oled could not connect",
    /*  10 */ "",
    /*  11 */ "",
    /*  12 */ "",
    /*  13 */ "",
    /*  14 */ "",
    /*  15 */ "",
    /*  16 */ "",
    /*  17 */ "",
    /*  18 */ "",
    /*  19 */ "",

    /* Add a string representation for an appropriate error with respect for it's order. */
};

/* Private function prototypes -------------------------------------------------------------------*/
/* Public function bodies ------------------------------------------------------------------------*/

/**
  * @brief      Convert an error to a string representation.
  * @param[in]  error_code: an integer indicates an error.
  * @retval     A string representation for such error.
  */
const char*
err_get_str_err (zerr_t error_code)
{
    uint32_t i = 0;
    const char* str_err = "unknown error!";

    /* Look up the array of error information. */
    for (; i < g_num_err; ++i)
    {
        if (g_err_info[i].code == error_code)
        {
            /* Found error code. */
            str_err = gp_str_err[g_err_info[i].offset];
            break;
        }
    }

    return str_err;
}
/* Private function bodies -----------------------------------------------------------------------*/

/* END OF FILE ************************************************************************************/
