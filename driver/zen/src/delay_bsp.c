/**
  **************************************************************************************************
  * @file   delay_bsp.c
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

/* Includes --------------------------------------------------------------------------------------*/

/* Global configuration. */
#include "zcfg.h"

#include "delay_bsp.h"

#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_dma.h"
#include "stm32f0xx_hal_tim.h"

/* Private macros --------------------------------------------------------------------------------*/

#define DELAY_BSP_TIM                       TIM1
#define DELAY_BSP_TIM_CLK_ENB()             __TIM1_CLK_ENABLE();
#define DELAY_BSP_TIM_IRQn                  TIM17_IRQn
#define delay_bsp_tim_isr(PARAM)            TIM1_IRQHandler(PARAM)

/* Private data types ----------------------------------------------------------------------------*/
typedef struct {
	uint8_t 						b_is_open; 		/* Flag for initializing */
	TIM_HandleTypeDef		h_tim;				/* Handle to TIM */
	volatile uint32_t		time;					/* Time to be delayed */
	SemaphoreHandle_t		sem_bin_tim_elapsed;	/* Signal to period elapsed */
	
} delay_bsp_h_t;

/* Private variables ----------------------------------------------------------------------------*/
static delay_bsp_h_t gh_delay = {0};

/* Private function prototypes -------------------------------------------------------------------*/
static zerr_t delay_bsp_cfg (TIM_HandleTypeDef* p_h_tim, const delay_bsp_unit_t unit);

/* Public function prototypes --------------------------------------------------------------------*/
zerr_t delay_bsp_open (void) {
	zerr_t ret = ZERR_OK;
	ZTRACE_DBG(TR_DELAY_BSP, ("DEBUG: %s: open delay low level driver .. \n", __FUNCTION__));
	if (0 == gh_delay.sem_bin_tim_elapsed) {
		gh_delay.sem_bin_tim_elapsed = xSemaphoreCreateBinary();
		ZTRACE_ERR(TR_DELAY_BSP, ("ERRPR: %s: out of memmory ...\n", __FUNCTION__),
							(0 != gh_delay.sem_bin_tim_elapsed), return ZERR_MEM_OUT;)
	}
	
		/* Enable TIM clock */
    DELAY_BSP_TIM_CLK_ENB();
		
		/* Configure TIM interrupt */
    HAL_NVIC_SetPriority(DELAY_BSP_TIM_IRQn, HW_PRIO_DELAY_TIM, 0);
    HAL_NVIC_EnableIRQ(DELAY_BSP_TIM_IRQn);
		
		/* Configure the TIM parameters */
    gh_delay.h_tim.Instance             = DELAY_BSP_TIM;
    gh_delay.h_tim.Init.ClockDivision   = 0;
    gh_delay.h_tim.Init.CounterMode     = TIM_COUNTERMODE_UP;
	gh_delay.h_tim.Init.ClockDivision	 	= TIM_CLOCKDIVISION_DIV1;
	gh_delay.h_tim.Init.AutoReloadPreload 		= TIM_AUTORELOAD_PRELOAD_DISABLE;
		
    gh_delay.b_is_open = 1;
		
		return ret;
}

/**
  * @brief      Execute delay base on unit.
  * @param[in]  time: time to be delayed
  * @param[in]  unit: delay unit
  * @retval     err_t
  */
zerr_t delay_bsp_exec (const uint32_t time, const delay_bsp_unit_t unit) {
		zerr_t ret = ZERR_OK;	
		ZTRACE_ERR(TR_DELAY_BSP, ("ERROR : %s: must open delay bsp first .. \n", __FUNCTION__),
							(1 == gh_delay.b_is_open ), return ZERR_NOT_OPEN);
		ZTRACE_ERR(TR_DELAY_BSP, ("ERROR : %s: invalid time \n", __FUNCTION__),
						(0 != time), return ZERR_INVALID_PARAM);
		gh_delay.time = time;
		ret = delay_bsp_cfg(&gh_delay.h_tim, unit);
		ZTRACE_ERR(TR_DELAY_BSP, ("ERROR : %s: time : %lu, uint: %u", __FUNCTION__, time, unit),
						(ZERR_OK == ret), return ret;);
	
		xSemaphoreTake(gh_delay.sem_bin_tim_elapsed, portMAX_DELAY);
		
	return ret;
}

/**
  * @brief  Handles Delay TIM interrupt request.
  * @param  None
  * @retval None
  */
void
delay_bsp_tim_isr (void)
{
    /* TIM Update event */
    if (RESET != __HAL_TIM_GET_FLAG(&gh_delay.h_tim, TIM_FLAG_UPDATE))
    {
        if (RESET != __HAL_TIM_GET_ITSTATUS(&gh_delay.h_tim, TIM_IT_UPDATE))
        {
            __HAL_TIM_CLEAR_IT(&gh_delay.h_tim, TIM_IT_UPDATE);

            if (gh_delay.time > 0)
            {
                --gh_delay.time;
            }
            else
            {
                BaseType_t b_is_yield = pdFALSE;

                HAL_TIM_Base_Stop_IT(&gh_delay.h_tim);
                xSemaphoreGiveFromISR(gh_delay.sem_bin_tim_elapsed, &b_is_yield);
                portYIELD_FROM_ISR(b_is_yield);
            }
        }
    }
}

/* Private function bodies -----------------------------------------------------------------------*/
/**
  * @brief      Configures TIM for executing.
  * @param[in]  *p_h_tim: TIM handle
  * @param[in]  unit: delay unit
  * @retval     zerr_t
  */
static zerr_t
delay_bsp_cfg (TIM_HandleTypeDef* p_h_tim, const delay_bsp_unit_t unit)
{
		zerr_t ret = ZERR_OK;

    /*
    Prescaler = (TIM2CLK / TIM2 counter clock) - 1 = ((SystemCoreClock/2) / TIM2 counter clock) - 1
    To get TIM2 counter clock at 8 MHz, the prescaler is computed as follows:
    Prescaler = ((SystemCoreClock/2) / 8000000) - 1
    */
    p_h_tim->Init.Prescaler = (uint16_t) (((SystemCoreClock / 2) / 8000000) - 1);
	
	  if (DELAY_BSP_US == unit)
    {
        /* Update rate = TIM2 counter clock / (Period + 1)
                       = 8000000 / 8 = 1000000
        So the TIM5 generates an interrupt each 1 us */
        p_h_tim->Init.Period = 11;
    }
    else /* DELAY_BSP_MS */
    {
        /* Update rate = TIM2 counter clock / (Period + 1)
                       = 8000000 / 8000 = 1000
        So the TIM2 generates an interrupt each 1 ms */
        p_h_tim->Init.Period = 11999;
    }
		
		if (HAL_OK != HAL_TIM_Base_Init(p_h_tim)) {
				ZTRACE_ERR(TR_DELAY_BSP, ("ERROR: delay_bsp_cfg: fail(1)\n"), 0, return ZERR_NOT_OPEN);
		}
		if (HAL_OK != HAL_TIM_Base_Start_IT(p_h_tim))
    {
        ZTRACE_ERR(TR_DELAY_BSP, ("ERROR: delay_bsp_cfg: fail(2)\n"), 0, return ZERR_NOT_OPEN);
    }
		
		return ret;
}
/* END OF FILE ************************************************************************************/
