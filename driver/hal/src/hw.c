/**
  **************************************************************************************************
  * @file   hw.c
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

/* Middleware */
#include "led.h"

/* ST driver */
#include "hw.h"

#include "stm32f0xx_hal.h"

/* Private function prototypes -------------------------------------------------------------------*/

static void hw_sysclk_hse_config (void);

static void hw_gpio_init (void);

/* Public function bodies ------------------------------------------------------------------------*/

/**
  * @brief  Initialize all of hardware components.
  *         This function will loop until they are initialized successfully.
  * @param  None
  * @retval None
  */
void
hw_open (void)
{
	__HAL_RCC_SYSCFG_CLK_ENABLE();
	__HAL_RCC_PWR_CLK_ENABLE();

	/* System interrupt init*/
	/* PendSV_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(PendSV_IRQn, 3, 0);

	HAL_Init();

	hw_sysclk_hse_config();
	
	hw_gpio_init();
	
	led_open();

}

/* Private function prototypes -------------------------------------------------------------------*/

static void
hw_sysclk_hse_config (void)
{
	  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

	  /** Initializes the RCC Oscillators according to the specified parameters
	  * in the RCC_OscInitTypeDef structure.
	  */
	  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSI14;
	  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	  RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
	  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	  RCC_OscInitStruct.HSI14CalibrationValue = 16;
	  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL8;
	  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
	  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	  {
		  for(;;);
	  }

	  /** Initializes the CPU, AHB and APB buses clocks
	  */
	  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
	                              |RCC_CLOCKTYPE_PCLK1;
	  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

	  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
	  {
		  for(;;);
	  }
	  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
	  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
	  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	  {
		  for(;;);
	  }
}

/**
  * @brief  Set all pins (except PA13, PA14, PD11, PD12) as Analog to optimize the power
  *         consumption. The used pins will be reconfigured after that.
  * @param  None
  * @retval None
  */
static void
hw_gpio_init (void)
{
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Pin = (GPIO_PIN_All & ~(GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_13 | GPIO_PIN_14));
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = (GPIO_PIN_All & ~(GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_13 | GPIO_PIN_14));
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* Initialize PB13 (Pump) and PB14 (Heater) as GPIO Output */
	GPIO_InitStruct.Pin = GPIO_PIN_13 | GPIO_PIN_14;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = (GPIO_PIN_All & ~(GPIO_PIN_13 | GPIO_PIN_14));
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}


/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}


/* END OF FILE ************************************************************************************/
