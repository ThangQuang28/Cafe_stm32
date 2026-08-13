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

/* BSP */
#include "ntc_bsp.h"

/* C Library */
#include "stdbool.h"
#include "stdint.h"

/* Define ----------------------------------------------------------------------*/

#define NTC_TEMP_DMA_CHANNEL		0U
#define NTC_BOILER_DMA_CHANNEL		1U

#define NTC_ADC_MAX_VALUE          4095U
#define NTC_RESISTANCE             100000.0f

#define NTC_MIN_TEMPERATURE        (-20.0f)
#define NTC_MAX_TEMPERATURE        (150.0f)

/*
 * First-order IIR filter.
 *
 * T_filtered[k] =
 *
 * T_filtered[k-1] +
 * alpha * (T[k] - T_filtered[k-1])
 */
#define NTC_FILTER_ALPHA           	0.1f


#define NTC_TEMPERATURE_SCALE		10

/* Private data types ----------------------------------------------------------*/

typedef struct
{
	uint16_t adc;
	int16_t temperature_x10;
} ntc_lut_entry_t;

/* Public data types -----------------------------------------------------------*/

/* Exported constants ----------------------------------------------------------*/

/* Private data ----------------------------------------------------------------*/

static const ntc_lut_entry_t
ntc_lut[]=
{
	    { 355U, -200},
	    { 466U, -150},
	    { 600U, -100},
	    { 758U,  -50},
	    { 939U,    0},
	    {1140U,   50},
	    {1357U,  100},
	    {1585U,  150},
	    {1817U,  200},
	    {2048U,  250},
	    {2270U,  300},
	    {2481U,  350},
	    {2676U,  400},
	    {2854U,  450},
	    {3014U,  500},
	    {3155U,  550},
	    {3280U,  600},
	    {3388U,  650},
	    {3482U,  700},
	    {3563U,  750},
	    {3633U,  800},
	    {3694U,  850},
	    {3745U,  900},
	    {3790U,  950},
	    {3828U, 1000},
	    {3861U, 1050},
	    {3889U, 1100},
	    {3914U, 1150},
	    {3935U, 1200},
	    {3953U, 1250},
	    {3969U, 1300},
	    {3983U, 1350},
	    {3995U, 1400},
	    {4006U, 1450},
	    {4015U, 1500}
};

#define NTC_LUT_SIZE \
	(sizeof(ntc_lut) / sizeof(ntc_lut[0]))

static float
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

/* Private function prototypes -------------------------------------------------*/

static bool
ntc_lookup_temperature(
    uint16_t adc_value,
    float *temp)
{
    uint32_t index;
    int32_t adc_1;
    int32_t adc_2;
    int32_t temp_1;
    int32_t temp_2;
    int32_t temp_x10;
    int32_t adc_delta;
    int32_t temp_delta;

    if (temp == NULL)
    {
        return false;
    }

    /*
     * NOTE (fix): ntc_lut[] is now sorted with ADC ASCENDING as
     * temperature increases (matches this board's topology: NTC on
     * the 3V3 side, pull-down to GND - see comment above ntc_lut[]).
     */

    /*
     * ADC below the lowest LUT value means temperature
     * is below the supported LUT temperature range.
     */
    if (adc_value < ntc_lut[0U].adc)
    {
        return false;
    }

    /*
     * ADC above the highest LUT value means temperature
     * is above the supported LUT temperature range.
     */
    if (adc_value > ntc_lut[NTC_LUT_SIZE - 1U].adc)
    {
        return false;
    }

    /*
     * Find the two LUT points surrounding the ADC value.
     */
    for (index = 0U;
         index < (NTC_LUT_SIZE - 1U);
         index++)
    {
        if ((adc_value >= ntc_lut[index].adc) &&
            (adc_value <= ntc_lut[index + 1U].adc))
        {
            break;
        }
    }

    if (index >= (NTC_LUT_SIZE - 1U))
    {
        return false;
    }

    /*
     * Get LUT points.
     */
    adc_1 =
        (int32_t)ntc_lut[index].adc;

    adc_2 =
        (int32_t)ntc_lut[index + 1U].adc;

    temp_1 =
        (int32_t)ntc_lut[index].temperature_x10;

    temp_2 =
        (int32_t)ntc_lut[index + 1U].temperature_x10;

    /*
     * Calculate interpolation deltas.
     */
    adc_delta =
        (int32_t)adc_value -
        adc_1;

    temp_delta =
        temp_2 -
        temp_1;

    /*
     * Linear interpolation.
     *
     * T = T1 +
     *     (ADC - ADC1) * (T2 - T1)
     *     ---------------------------
     *             (ADC2 - ADC1)
     */
    temp_x10 =
        temp_1 +
        (
            (adc_delta * temp_delta) /
            (adc_2 - adc_1)
        );

    /*
     * Convert temperature x10 to Celsius.
     */
    *temp =
        (float)temp_x10 /
        (float)NTC_TEMPERATURE_SCALE;

    return true;
}


/**
 * @brief      Filter NTC temperature using first-order IIR filter.
 * @param[in]  type: NTC type.
 * @param[in]  temperature: New temperature.
 * @retval     Filtered temperature.
 */
static float
ntc_filter_temperature(
    ntc_type_t type,
    float temperature)
{
    if (!ntc_filter_initialized[type])
    {
        ntc_temperature_filtered[type] =
            temperature;

        ntc_filter_initialized[type] =
            true;

        return temperature;
    }

    ntc_temperature_filtered[type] +=
        NTC_FILTER_ALPHA *
        (temperature -
         ntc_temperature_filtered[type]);

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
    float temperature;

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
