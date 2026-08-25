/**
 * ----------------------------------------------------------------------------
 * @auth            : manhtd
 * @day             : 19-Sep-2024
 * @file            : ntc.c
 * @description     : NTC middleware
 * ----------------------------------------------------------------------------
 */

/* Indent using spaces
 * Tab width   : 4 spaces
 * Line width  : 100 characters
 * Line ending : LF (0x0A)
 */

/* Include ---------------------------------------------------------------------*/

/* Middleware */
#include "ntc.h"
#include "zerr.h"
#include "zcfg.h"

/* BSP */
#include "ntc_bsp.h"

/* C Library */
#include "stdbool.h"
#include "stdint.h"
#include <math.h>

/* Define ----------------------------------------------------------------------*/

#define NTC_TEMP_DMA_CHANNEL		0U
#define NTC_BOILER_DMA_CHANNEL		1U

#define NTC_ADC_MAX_VALUE          	4095U
#define NTC_R_DIV                  	100000.0f

#define NTC_R0 						100000			// Điện trở R0 trên PCB = 100.000
#define NTC_T0 						273.15f			// Nhiệt độ Kelvin
#define Beta 						3950			// Hệ số Beta

#define NTC_MIN_TEMPERATURE        (-200)
#define NTC_MAX_TEMPERATURE        (1500)


/*
 * First-order IIR filter.
 *
 * T_filtered[k] =
 *
 * T_filtered[k-1] +
 * alpha * (T[k] - T_filtered[k-1])
 */
#define NTC_FILTER_ALPHA_Q8         26


#define NTC_FILTER_SCALE_Q8			256

/* Private data types ----------------------------------------------------------*/

typedef struct
{
	uint16_t adc;
	int16_t temperature_x10;
} ntc_lut_entry_t;

/* Public data types -----------------------------------------------------------*/

/* Exported constants ----------------------------------------------------------*/

/* Private data ----------------------------------------------------------------*/



static int16_t
ntc_temperature[NTC_MAX_T] =
{
    0.0f,
    0.0f
};

static float
ntc_temperature_filtered[NTC_MAX_T] =
{
    0.0f,
    0.0f
};

static bool
ntc_filter_initialized[NTC_MAX_T] =
{
    false,
    false
};

static bool
ntc_err_flag[NTC_MAX_T] =
{
    true,
    true
};
static float NTC_A = 0.0f;
static float NTC_B = 0.0f;
static float NTC_C = 0.0f;
static bool coe_calculated = false;

/* Private function prototypes -------------------------------------------------*/

static bool
ntc_lookup_temperature(
    uint16_t adc_value,
    int16_t *temp)
{
    if (temp == NULL || adc_value == 0 || adc_value >= NTC_ADC_MAX_VALUE)
    {
        return false;
    }

    // Nhiệt độ tại 0, 25, 100
    float NTC_T1 = NTC_T0 + 0.0f;
    float NTC_T2 = NTC_T0 + 25.0f;		// Nhiệt độ tiêu chuẩn
    float NTC_T3 = NTC_T0 + 100.0f;

    /*
     * Công thức tính điện trở NTC
     * V_ADC = VCC * R_DIV / (R_NTC + R_DIV)
     * ADC = 4095 * R_DIV / (R_NTC + R_DIV)
     * R_NTC = R_DIV * (4095 - ADC) / ADC
     */
    float r_ntc = NTC_R_DIV * ((float)NTC_ADC_MAX_VALUE - (float)adc_value) / (float)adc_value;

    /*
     * Công thức tính hệ số A, B, C
     * A = Y1 - (B + L1^2*C)*L1
     * B = gamma2 - C*(L1^2 +L1*L2 + L2^2)
     * C = ((gamma3 - gamma2)/(L3-L2))*(L1+L2+L3)^-1
     */
    if (!coe_calculated) {
        // Tính Điện trở tại 0, 25, 100
        float NTC_R1 = NTC_R0 * expf(Beta * ((1.0f / NTC_T1) - (1.0f / NTC_T2)));
        float NTC_R2 = NTC_R0 * expf(Beta * ((1.0f / NTC_T2) - (1.0f / NTC_T2)));
        float NTC_R3 = NTC_R0 * expf(Beta * ((1.0f / NTC_T3) - (1.0f / NTC_T2)));

    	float L1 = logf(NTC_R1);
    	float L2 = logf(NTC_R2);
    	float L3 = logf(NTC_R3);

    	float Y1 = 1/NTC_T1;
    	float Y2 = 1/NTC_T2;
    	float Y3 = 1/NTC_T3;

    	float gamma2 = (Y2 - Y1)/(L2 - L1);
    	float gamma3 = (Y3 - Y1)/(L3 - L1);

    	NTC_C = ((gamma3 - gamma2)/(L3 - L2))/(L1 + L2 + L3);
    	NTC_B = gamma2 - NTC_C*(L1*L1 + L1*L2 + L2*L2);
    	NTC_A = Y1 - (NTC_B + L1*L1*NTC_C)*L1;

    	coe_calculated = true;
    }

    /*
     * Phương trình Steinhart-Hart
     * 1/T = A + B*ln(R_NTC) + C*(ln(R_NTC))^3
     */
//    float ln_r = logf(r_ntc);
//    float temp_K = 1.0f / (ntc_sh_a + ntc_sh_b * ln_r + ntc_sh_c * ln_r * ln_r * ln_r);

    float temp_K = 1.0f / (NTC_A + NTC_B * logf(r_ntc) + NTC_C * logf(r_ntc) * logf(r_ntc) * logf(r_ntc));

    // Chuyển sang nhiệt độ C
    float temp_C = temp_K - 273.15f;

    *temp = (int16_t)(temp_C * 10.0f);

    return true;
}


/**
 * @brief      Filter NTC temperature using first-order IIR filter.
 * @param[in]  type: NTC type.
 * @param[in]  temperature: New temperature.
 * @retval     Filtered temperature.
 */
static int16_t
ntc_filter_temperature(
    ntc_type_t type,
    int16_t temperature)
{
	int32_t delta;

    if (!ntc_filter_initialized[type])
    {
        ntc_temperature_filtered[type] =
            temperature;

        ntc_filter_initialized[type] =
            true;

        return temperature;
    }

    delta = (int32_t)temperature - (int32_t)ntc_temperature_filtered[type];

    ntc_temperature_filtered[type] =
         (int16_t)(
             (int32_t)ntc_temperature_filtered[type] +
             ((NTC_FILTER_ALPHA_Q8 * delta) / NTC_FILTER_SCALE_Q8));

    return ntc_temperature_filtered[type];
}

/**
 * @brief      Update one NTC temperature.
 * @param[in]  type: NTC type.
 * @param[in]  channel: BSP ADC channel index.
 * @retval     None.
 */
static void
ntc_update_temperature(
    ntc_type_t type,
    uint8_t channel)
{
    uint16_t adc_value;
    int16_t temperature;

    adc_value =
        ntc_bsp_get_raw(channel);

    /*
     * Check ADC raw value.
     */
    if ((adc_value == 0U) ||
        (adc_value >= NTC_ADC_MAX_VALUE))
    {
        ntc_err_flag[type] = true;
        return;
    }

    /*
     * Convert temperature using LUT
     */
    if(!ntc_lookup_temperature(adc_value, &temperature))
    {
    	ntc_err_flag[type] = true;
    	return;
    }


    /*
     * Check temperature range
     */
    if ((temperature < NTC_MIN_TEMPERATURE) ||
    	(temperature > NTC_MAX_TEMPERATURE))
    {
        ntc_err_flag[type] = true;
        return;
    }

    /*
     * Apply IIR filter.
     */
    temperature =
        ntc_filter_temperature(
            type,
            temperature);

    /*
     * Store valid temperature.
     */
    ntc_temperature[type] =
        temperature;

    ntc_err_flag[type] =
        false;
}

/* Public function bodies ------------------------------------------------------*/

/**
 * @brief      Open NTC sensor.
 * @retval     ZERR_OK       if successful.
 * @retval     ZERR_FALSE    if failed.
 */
zerr_t
ntc_open(void)
{
    zerr_t ret;

    /*
     * Reset temperature state.
     */
    ntc_temperature[NTC_TEMP_T] =
        0.0f;

    ntc_temperature[NTC_BOILER_T] =
        0.0f;

    /*
     * Reset filter state.
     */
    ntc_temperature_filtered[NTC_TEMP_T] =
        0.0f;

    ntc_temperature_filtered[NTC_BOILER_T] =
        0.0f;

    ntc_filter_initialized[NTC_TEMP_T] =
        false;

    ntc_filter_initialized[NTC_BOILER_T] =
        false;

    /*
     * NTC is invalid until
     * the first valid ADC sample.
     */
    ntc_err_flag[NTC_TEMP_T] =
        true;

    ntc_err_flag[NTC_BOILER_T] =
        true;

    /*
     * Open BSP.
     */
    ret = ntc_bsp_open();

    if (ret != ZERR_OK)
    {
        return ret;
    }

    return ZERR_OK;
}

/**
 * @brief      Start NTC ADC DMA.
 * @retval     ZERR_OK       if successful.
 * @retval     ZERR_FALSE    if failed.
 */
zerr_t
ntc_start(void)
{
    return ntc_bsp_start();
}

/**
 * @brief      Update NTC temperature.
 * @retval     None.
 */
void
ntc_update(void)
{
    if (!ntc_bsp_ready())
    {
        return;
    }

    /*
     * ADC_CHANNEL_8
     * PB0
     * NTC_TEMP
     */
    ntc_update_temperature(
        NTC_TEMP_T,
        NTC_TEMP_DMA_CHANNEL);

    /*
     * ADC_CHANNEL_9
     * PB1
     * NTC_BOILER
     */
    ntc_update_temperature(
        NTC_BOILER_T,
        NTC_BOILER_DMA_CHANNEL);

    /*
     * ADC DMA data has been processed.
     */
    ntc_bsp_clear_ready();
}

/**
 * @brief      Get boiler temperature.
 * @retval     Temperature in Celsius.
 */
float
read_temp_boiler(void)
{
    return ntc_temperature[NTC_BOILER_T];
}

/**
 * @brief      Get temperature sensor value.
 * @retval     Temperature in Celsius.
 */
float
read_temp_sensor(void)
{
    return ntc_temperature[NTC_TEMP_T];
}

/**
 * @brief      Get raw ADC value.
 * @param[in]  type: NTC type.
 * @retval     Raw ADC value.
 */
uint16_t
ntc_get_raw(ntc_type_t type)
{
    if (type >= NTC_MAX_T)
    {
        return 0U;
    }

    if (type == NTC_TEMP_T)
    {
        return ntc_bsp_get_raw(NTC_TEMP_DMA_CHANNEL);
    }

    return ntc_bsp_get_raw(NTC_BOILER_DMA_CHANNEL);
}

/**
 * @brief      Check NTC error.
 * @param[in]  type: NTC type.
 * @retval     true  if error.
 * @retval     false if normal.
 */
bool
ntc_error(ntc_type_t type)
{
    if (type >= NTC_MAX_T)
    {
        return true;
    }

    return ntc_err_flag[type];
}

/* END OF FILE ----------------------------------------------------------------*/
