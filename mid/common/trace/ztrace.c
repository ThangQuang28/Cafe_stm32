/**
  **************************************************************************************************
  * @file   wtrace.c
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

/* Middleware. */
#include "led.h"
#include "zlog.h"

/* ST driver. */
#include "stm32f0xx_hal.h"

/* Private macros --------------------------------------------------------------------------------*/
/* Private data types ----------------------------------------------------------------------------*/
/* Private variables -----------------------------------------------------------------------------*/
/* Private function prototypes -------------------------------------------------------------------*/
/* Public function bodies ------------------------------------------------------------------------*/

/**
  * @brief  Initialize ztrace module.
  * @param  None.
  * @retval zerr_t.
  */
zerr_t
ztrace_open (void) {
	return zlog_open();
}

/**
  * @brief  This function is called by ZTRACE_ASSERT() macro throughout the source files to check
  *         how the application is using FreeRTOS. It is highly recommended to develop FreeRTOS
  *         applications with ZTRACE_ASSERT() defined. An assertion is triggered if the parameter
  *         passed into ZTRACE_ASSERT() is zero. It is normal to define ZTRACE_ASSERT() in such a
  *         way that it will prevent the application from executing any further. This if for
  *         two reasons; stopping the application at the point of the assertion allows the cause of
  *         the assertion to be debugged, and executing past a triggered assertion will probably
  *         result in a crash anyway.
  * @note   Defining ZTRACE_ASSERT() will increase both the application code size and execution
  *         time. When the application is stable the additional overhead can be removed by
  *         simply modify the ZASSERT macro to 0 value in zcfg.h.
  * @param  None.
  * @retval None.
  */
void
ztrace_assert (void)
{
    taskDISABLE_INTERRUPTS();
    led_on(LED_HEARTBEAT);
    
    /* rester device */
    HAL_NVIC_SystemReset();
/*    while (1 == b_loop)
//    {
//        Set the b_loop variable to a zero value in the debugger to step out of this function to
//        the point that caused the assertion. */
/*    } */
}

/* Private function bodies -----------------------------------------------------------------------*/

/* END OF FILE ************************************************************************************/
