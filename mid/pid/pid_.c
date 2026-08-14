/**
 *******************************************************************************
 * @auth            : thangquang
 * @day             : 19-Sep-2024
 * @file            : pid.c
 * @description     : PUMP BSP for STM32F0xx MCU
 *******************************************************************************
 */
/* Indent using spaces
 * Tab width   : 4 spaces
 * Line width  : 100 characters
 * Line ending : LF (0x0A)
 */

/* Include -------------------------------------------------------------------*/

/* Middleware */
#include <pid_.h>
#include "pid_bsp.h"
/* C library */
#include "stdbool.h"
#include "stdint.h"

/* Define --------------------------------------------------------------------*/

#define PID_DEFAULT_D_FILTER_ALPHA_Q12		410L
#define PID_MIN_TS_MS						1U

#define PID_GAIN_SCALE						4096L
#define PID_GAIN(real)						((int32_t)((real) * (float)PID_GAIN_SCALE))

#define PID_FILTER_SCALE					256L

#define PID_P_DIVISOR						(PID_GAIN_SCALE * 10L)
#define PID_I_DIVISOR						(PID_GAIN_SCALE * 10L * 1000L)
#define PID_D_DIVISOR_BASE					(PID_GAIN_SCALE * 10L * PID_FILTER_SCALE)
/* Private data types -------------------------------------------------------- */

/* Public data types --------------------------------------------------------- */

/* Exported constants -------------------------------------------------------- */

/* Private data -------------------------------------------------------------- */

/* Private function prototypes ---------------------------------------------- */

static int32_t
pid_clamp(int32_t value,
		  int32_t min,
		  int32_t max);

static int32_t
pid_calculate_derivative(pid_t *pid,
						 int16_t pv_x10);

static int32_t
pid_apply_anti_windup(pid_t *pid,
					  int32_t error_x10,
					  int32_t p_term,
					  int32_t d_term,
					  int32_t integral_candidate);

/* Private functions ----------------------------------------------------------*/

/**
 * @brief Calculate derivative using a filter PV
 */
static int32_t
pid_calculate_derivative(pid_t *pid,
						 int16_t pv_x10)
{
	int32_t raw_scaled;
	int32_t delta;
	int32_t old_last_pv_accum;
	int64_t d_term;

	if(!pid->init)
	{
		pid->filtered_pv_accum = (int32_t) pv_x10 * PID_FILTER_SCALE;
		pid->last_pv_accum = pid->filtered_pv_accum;
		return 0;
	}

	raw_scaled = (int32_t) pv_x10 * PID_FILTER_SCALE;

	delta = raw_scaled - pid->filtered_pv_accum;

	pid->filtered_pv_accum += (int32_t)(((int64_t)delta * pid->d_filter_alpha_q12)/PID_GAIN_SCALE);

	old_last_pv_accum = pid->last_pv_accum;

	d_term = -((int64_t)pid->kd_q12 * (pid->filtered_pv_accum - old_last_pv_accum) * 1000LL) /
			((int64_t)PID_D_DIVISOR_BASE * (int64_t)pid->ts_ms);

	pid->last_pv_accum = pid->filtered_pv_accum;

	return (int32_t) d_term;
}

/**
 * @brief Apply conditional-integration anti-windup and return the final output.
 */
static int32_t
pid_apply_anti_windup(pid_t *pid,
					  int32_t error_x10,
					  int32_t p_term,
					  int32_t d_term,
					  int32_t integral_candidate)
{
	int64_t i_term_candidate;
	int64_t i_term_final;
	int32_t output_candidate;
	int32_t output;

	i_term_candidate = ((int64_t)pid->ki_q12 * integral_candidate) / PID_I_DIVISOR;
	output_candidate = p_term + (int32_t)i_term_candidate + d_term;

	if(!((output_candidate < pid->out_max) && (error_x10 > 0)))
	{
		if(!((output_candidate < pid->out_min) && (error_x10 < 0)))
		{
			pid->integral_sum = integral_candidate;
		}
	}

	i_term_final = ((int64_t)pid->ki_q12 * pid->integral_sum) / PID_I_DIVISOR;

	output = p_term + (int32_t)i_term_final + d_term;

	return pid_clamp(output, pid->out_min, pid->out_max);
}


/* Public functions -----------------------------------------------------------*/

/**
 * @brief Open PID Controller
 */
zerr_t
pid_open(pid_t *pid,
		 int32_t kp_q12,
		 int32_t ki_q12,
		 int32_t kd_q12,
		 int32_t ts_ms,
		 int16_t out_min,
		 int16_t out_max)
{
	if(pid == NULL)
	{
		return ZERR_FALSE;
	}

	if(ts_ms < PID_MIN_TS_MS)
	{
		return ZERR_FALSE;
	}

	if(out_min >= out_max)
	{
		return ZERR_FALSE;
	}

	if((kp_q12 < 0) || (ki_q12 < 0) || (kd_q12 < 0))
	{
		return ZERR_FALSE;
	}

	pid->kp_q12 = kp_q12;
	pid->ki_q12 = ki_q12;
	pid->kd_q12 = kd_q12;
	pid->ts_ms = ts_ms;
	pid->out_min = out_min;
	pid->out_max = out_max;
	pid->d_filter_alpha_q12 = PID_DEFAULT_D_FILTER_ALPHA_Q12;

	pid_reset(pid);

	return ZERR_OK;
}

/**
 * @brief Update PID controller
 */
int32_t
pid_update(pid_t *pid,
		   int16_t setpoint_x10,
		   int16_t pv_x10)
{
	int32_t error_x10;
	int32_t p_term;
	int32_t d_term;
	int32_t integral_candidate;
	int32_t output;
	int32_t tmp;

	if(pid == NULL)
	{
		return 0;
	}

	if(pid->ts_ms < PID_MIN_TS_MS)
	{
		return 0;
	}

	error_x10 = (int32_t)setpoint_x10 - (int32_t)pv_x10;
	tmp = ((int64_t)pid->kp_q12 * error_x10) / PID_P_DIVISOR;
	p_term = (int32_t)tmp;
	d_term = pid_calculate_derivative(pid, pv_x10);
	integral_candidate = pid->integral_sum + (error_x10 * (int32_t)pid->ts_ms);
	output = pid_apply_anti_windup(pid, error_x10, p_term, d_term, integral_candidate);
	tmp = ((int64_t)pid->ki_q12 * pid->integral_sum) / PID_I_DIVISOR;

	pid->last_p_term = p_term;
	pid->last_i_term = (int32_t) tmp;
	pid->last_d_term = d_term;
	pid->last_output = output;
	pid->init = true;

	return output;
}

/**
 * @brief Reset PID controller state
 */
void
pid_reset(pid_t *pid)
{
	if(pid == NULL)
	{
		return;
	}

	pid->integral_sum = 0;
	pid->filtered_pv_accum = 0;
	pid->last_pv_accum = 0;
	pid->last_p_term = 0;
	pid->last_i_term = 0;
	pid->last_d_term = 0;
	pid->last_output = 0;
	pid->init = false;
}

/**
 * @brief Configure derivative filter
 */
zerr_t
pid_set_d_filter(pid_t *pid,
				 int32_t alpha_q12)
{
	if(pid == NULL)
	{
		return ZERR_FALSE;
	}
	if((alpha_q12 <= 0) || (alpha_q12 > PID_GAIN_SCALE))
	{
		return ZERR_FALSE;
	}

	pid->d_filter_alpha_q12 = alpha_q12;
	return ZERR_OK;
}

/**
 * @brief Check PID status
 */
bool
pid_ready(const pid_t *pid)
{
	if(pid == NULL)
	{
		return false;
	}

	return pid->init;
}

/**
 * @brief Read back the P/I/D breakdown and output of the last update.
 */
void
pid_get_debug(
    const pid_t *pid,
    pid_debug_t *debug)
{
    if (debug == NULL)
    {
        return;
    }

    if (pid == NULL)
    {
        debug->p_term = 0;
        debug->i_term = 0;
        debug->d_term = 0;
        debug->output = 0;
        debug->integral_sum = 0;
        return;
    }

    debug->p_term =
        pid->last_p_term;

    debug->i_term =
        pid->last_i_term;

    debug->d_term =
        pid->last_d_term;

    debug->output =
        pid->last_output;

    debug->integral_sum =
        pid->integral_sum;
}

/**
 * @brief Clamp value.
 */
static int32_t
pid_clamp(
    int32_t value,
    int32_t min,
    int32_t max)
{
    if (value > max)
    {
        return max;
    }

    if (value < min)
    {
        return min;
    }

    return value;
}

/* END OF FILE ---------------------------------------------------------------- */



