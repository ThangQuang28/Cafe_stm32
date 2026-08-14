/**
 *******************************************************************************
 * @auth            : thangquang
 * @day             : 19-Sep-2024
 * @file            : pid.h
 * @dissaption      : FWCore for STM32F0xx MCU designed by RDU
 *******************************************************************************
 */

/*  Indent using spaces
    Tab width: 4 spaces
    Line width: 100 characters
    Line ending: LF (0x0A) */

/* Prevent recursive inclusion ------------------------------------------------*/
#ifndef __PID_H_
#define __PID_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Public macros ---------------------------------------------------------------------------------*/

/* Include ---------------------------------------------------------------------------------------*/

/* C library */
#include "stdbool.h"
#include "stdint.h"
/* Global */
#include "zerr.h"

/* Define ----------------------------------------------------------------------------------------*/

/* Private data types ----------------------------------------------------------------------------*/

/* Public data types -----------------------------------------------------------------------------*/
typedef struct
{
    int32_t kp_q12;
    int32_t ki_q12;
    int32_t kd_q12;
    uint32_t ts_ms;
    int32_t integral_sum;          /* raw sum(error_x10 * ts_ms), Ki applied at output */
    int32_t filtered_pv_accum;     /* filtered PV, x10 C * internal sub-scale */
    int32_t last_pv_accum;         /* filtered_pv_accum from the previous update */
    int32_t d_filter_alpha_q12;
    int16_t out_min;               /* percent, e.g. 0 */
    int16_t out_max;               /* percent, e.g. 100 */
    int32_t last_p_term;
    int32_t last_i_term;
    int32_t last_d_term;
    int32_t last_output;
    bool init;
} pid_t;

typedef struct
{
	int32_t p_term;
	int32_t i_term;
	int32_t d_term;
	int32_t output;
	int32_t integral_sum;
} pid_debug_t;
/* Exported constants ----------------------------------------------------------------------------*/

/* Private function prototypes -------------------------------------------------------------------*/

/* Public function prototypes --------------------------------------------------------------------*/

/**
 * @brief Open PID control
 */
zerr_t
pid_open(pid_t *pid,
		 int32_t kp_q12,
		 int32_t ki_q12,
		 int32_t kd_q12,
		 int32_t ts_ms,
		 int16_t out_min,
		 int16_t out_max);

/**
 * @brief Update PID controller
 *
 * @param[in,out] pid	 	PID controller
 * @param[in]	  setpoint 	Desired temp
 * @param[in]	  pv		Measured temp
 *
 * @retval PID output within configured output range
 */
int32_t
pid_update(pid_t *pid,
		   int16_t setpoint_x10,
		   int16_t pv_x10);

/**
 * @brief Reset PID controller state
 */
void
pid_reset(pid_t *pid);

/**
 * @brief Configure derivative filter
 *
 * @param[in,out] pid	PID controller
 * @param[in]	  alpha	Filter coefficient, range (0,1). Lower = heavier filtering
 */
zerr_t
pid_set_d_filter(pid_t *pid,
				 int32_t alpha_q12);

/**
 * @brief Check PID controller status
 */
bool
pid_ready(const pid_t *pid);

/**
 * @brief Read back the P/I/D
 */
void
pid_get_debug(const pid_t *pid,
			  pid_debug_t *debug);

#ifdef __cplusplus
}
#endif

#endif /* __PID_H_ */

/* END OF FILE ----------------------------------------------------------------*/
