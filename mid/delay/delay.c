/**
  **************************************************************************************************
  * @file   delay.c
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

#include "delay.h"
#include "delay_bsp.h"

/* Private macros --------------------------------------------------------------------------------*/
/* Private data types ----------------------------------------------------------------------------*/
typedef struct {
		uint8_t 				b_is_open; 	/* Flag for initializing */
		SemaphoreHandle_t		sem_mtx;		/* Signal to period elapsed */
} delay_h_t;

/* Private variables ----------------------------------------------------------------------------*/
static delay_h_t gh_delay = {0};

/* Private function bodies -----------------------------------------------------------------------*/

/* Public function bodies ------------------------------------------------------------------------*/
zerr_t delay_open (void) {
	zerr_t ret = ZERR_OK;
	ZTRACE_DBG(TR_DELAY, ("DEBUG: %s: open delay driver .. \n", __FUNCTION__));
	if (0 == gh_delay.sem_mtx) {
			gh_delay.sem_mtx = xSemaphoreCreateMutex();
			ZTRACE_ERR(TR_DELAY, ("ERROR: %s: out of memmory .. \n", __FUNCTION__), 
													(0 != gh_delay.sem_mtx), return ZERR_MEM_OUT;);
	}
	
	ret = delay_bsp_open();
	ZTRACE_ERR(TR_DELAY, ("ERR : %s: fail \n"), (ZERR_OK == ret), return ret);
	
	gh_delay.b_is_open = 1;
	return ret;
}
zerr_t delay_ms (uint32_t num_ms) {
		
	zerr_t ret = ZERR_OK;
	ZTRACE_ERR(TR_DELAY, ("ERROR : %s: must open delay first \n", __FUNCTION__), 
										(1 == gh_delay.b_is_open), return ZERR_NOT_OPEN);
	ZTRACE_ERR(TR_DELAY, ("ERROR : %s: must open delay first \n", __FUNCTION__), 
										(0 != num_ms), return ZERR_NOT_OPEN);
	
    xSemaphoreTake(gh_delay.sem_mtx, portMAX_DELAY);
    ZTRACE_DBG(TR_DELAY, ("DEBUG: delay_ms: delay %lu ms..\n", num_ms));
    ret = delay_bsp_exec(num_ms, DELAY_BSP_MS);
    ZTRACE_ERR(TR_DELAY, ("ERROR: delay_ms: fail\n"), (ZERR_OK == ret), return ret;);
    xSemaphoreGive(gh_delay.sem_mtx);
		return ret;
}
zerr_t delay_us (uint32_t num_us) {
  	
	zerr_t ret = ZERR_OK;
	ZTRACE_ERR(TR_DELAY, ("ERROR : %s: must open delay first \n", __FUNCTION__), 
										(1 == gh_delay.b_is_open), return ZERR_NOT_OPEN);
	ZTRACE_ERR(TR_DELAY, ("ERROR : %s: must open delay first \n", __FUNCTION__), 
										(0 != num_us), return ZERR_NOT_OPEN);
	
    xSemaphoreTake(gh_delay.sem_mtx, portMAX_DELAY);
    ZTRACE_DBG(TR_DELAY, ("DEBUG: delay_us: delay %lu ms..\n", num_us));
    ret = delay_bsp_exec(num_us, DELAY_BSP_US);
    ZTRACE_ERR(TR_DELAY, ("ERROR: delay_us: fail\n"), (ZERR_OK == ret), return ret;);
    xSemaphoreGive(gh_delay.sem_mtx);
		return ret;
}

/* END OF FILE ************************************************************************************/
