/**
  **************************************************************************************************
  * @file   main.c
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

/* Application */
#include "zapp.h"

#include "cmsis_os.h"

#include "led.h"

/* ST driver */
#include "hw.h"

/* Private function prototypes -----------------------------------------------*/

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
	hw_open();

	zapp_open();
	/* Start scheduler */
	osKernelStart();

	while (1)
	{}
}

/* END OF FILE ************************************************************************************/
