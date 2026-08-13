
/**
  **************************************************************************************************
  * @file   led_bsp.c
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
/* #include "cfg.h" */

/* Middleware. */

/* BSP. */
#include "led_bsp.h"

/* ST driver. */
#include "stm32f0xx_hal.h"

/* Private macros --------------------------------------------------------------------------------*/

#define LED_ERROR_BSP_GPIO_CLK_ENB()        __GPIOC_CLK_ENABLE();
#define LED_ERROR_BSP_PIN                   GPIO_PIN_14
#define LED_ERROR_BSP_PORT                  GPIOC

#define LED_HEARTBEAT_BSP_GPIO_CLK_ENB()    __GPIOC_CLK_ENABLE();
#define LED_HEARTBEAT_BSP_PIN               GPIO_PIN_13
#define LED_HEARTBEAT_BSP_PORT              GPIOC

#define BUZZER_BSP_GPIO_CLK_ENB()    __GPIOB_CLK_ENABLE();
#define BUZZER_BSP_PIN               GPIO_PIN_5
#define BUZZER_BSP_PORT              GPIOB

/* Private data types ----------------------------------------------------------------------------*/
/* Private variables -----------------------------------------------------------------------------*/
/* Private function prototypes -------------------------------------------------------------------*/
/* Public function bodies ------------------------------------------------------------------------*/

/**
  * @brief      Configures GPIO relating to LED.
  * @param[in]  type: type to be configured.
  * @retval     None.
  */
void
led_bsp_open (led_type_t type)
{
    GPIO_InitTypeDef  GPIO_InitStruct;

    switch (type)
    {
        case LED_ERROR:
            LED_ERROR_BSP_GPIO_CLK_ENB(); /* Enable LED1 GPIO clock. */

            /* Configure LED1 pin. */
            GPIO_InitStruct.Pin = LED_ERROR_BSP_PIN;
            GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
            GPIO_InitStruct.Pull = GPIO_PULLDOWN;
            GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
            HAL_GPIO_Init(LED_ERROR_BSP_PORT, &GPIO_InitStruct);
            led_bsp_off(LED_ERROR);
        break;

        case LED_HEARTBEAT:
            LED_HEARTBEAT_BSP_GPIO_CLK_ENB(); /* Enable LED2 GPIO clock. */

            /* Configure LED2 pin. */
            GPIO_InitStruct.Pin = LED_HEARTBEAT_BSP_PIN;
            GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
            GPIO_InitStruct.Pull = GPIO_PULLUP;
            GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
            HAL_GPIO_Init(LED_HEARTBEAT_BSP_PORT, &GPIO_InitStruct);
            led_bsp_off(LED_HEARTBEAT);
        break;
		
		case BUZZER:
			BUZZER_BSP_GPIO_CLK_ENB(); /* Enable LED2 GPIO clock. */
            /* Configure LED2 pin. */
            GPIO_InitStruct.Pin = BUZZER_BSP_PIN;
            GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
            GPIO_InitStruct.Pull = GPIO_PULLUP;
            GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
            HAL_GPIO_Init(BUZZER_BSP_PORT, &GPIO_InitStruct);
            led_bsp_off(BUZZER);
			break;

        default:
            /*TRACE_ASSERT(0);  Unknown type. */
        break;
    }
}

/**
  * @brief      Turns the LED on.
  * @param[in]  type: type to be set on.
  * @retval     None.
  */
void
led_bsp_on (led_type_t type)
{
    switch (type)
    {
        case LED_ERROR:
            HAL_GPIO_WritePin(LED_ERROR_BSP_PORT, LED_ERROR_BSP_PIN, GPIO_PIN_SET);
        break;

        case LED_HEARTBEAT:
            HAL_GPIO_WritePin(LED_HEARTBEAT_BSP_PORT, LED_HEARTBEAT_BSP_PIN, GPIO_PIN_SET);
        break;
		
		case BUZZER:
			HAL_GPIO_WritePin(BUZZER_BSP_PORT, BUZZER_BSP_PIN, GPIO_PIN_SET);
		break;

        default:
            /* TRACE_ASSERT(0);  Unknown type. */
        break;
    }
}

/**
  * @brief      Turns the LED off.
  * @param[in]  type: type to be set off.
  * @retval     None.
  */
void
led_bsp_off (led_type_t type)
{
    switch (type)
    {
        case LED_ERROR:
            HAL_GPIO_WritePin(LED_ERROR_BSP_PORT, LED_ERROR_BSP_PIN, GPIO_PIN_RESET);
        break;

        case LED_HEARTBEAT:
            HAL_GPIO_WritePin(LED_HEARTBEAT_BSP_PORT, LED_HEARTBEAT_BSP_PIN, GPIO_PIN_RESET);
        break;

		case BUZZER:
			HAL_GPIO_WritePin(BUZZER_BSP_PORT, BUZZER_BSP_PIN, GPIO_PIN_RESET);
		break;
        default:
            /* TRACE_ASSERT(0);  Unknown type. */
        break;
    }
}

/**
  * @brief      Toggles the LED.
  * @param[in]  type: type to be toggled.
  * @retval     None.
  */
void
led_bsp_toggle (led_type_t type)
{
    switch (type)
    {
        case LED_ERROR:
            HAL_GPIO_TogglePin(LED_ERROR_BSP_PORT, LED_ERROR_BSP_PIN);
        break;

        case LED_HEARTBEAT:
            HAL_GPIO_TogglePin(LED_HEARTBEAT_BSP_PORT, LED_HEARTBEAT_BSP_PIN);
        break;

        default:
            /* TRACE_ASSERT(0);  Unknown type. */
        break;
    }
}

/* Private function bodies -----------------------------------------------------------------------*/

/* END OF FILE ************************************************************************************/
