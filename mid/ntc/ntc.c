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

#define NTC_FILTER_ALPHA			(0.1f)

/* Private data types ----------------------------------------------------------*/

typedef struct
{
	uint16_t adc;
	float temp;
} ntc_lut_entry_t;

/* Public data types -----------------------------------------------------------*/

/* Exported constants ----------------------------------------------------------*/

/* Private data ----------------------------------------------------------------*/

static const ntc_lut_entry_t
ntc_lut[]=
{
		{ 355U, -20.0f },
		{ 466U, -15.0f },
		{ 600U, -10.0f },
		{ 758U,  -5.0f },
		{ 939U,   0.0f },
		{1140U,   5.0f },
		{1357U,  10.0f },
		{1585U,  15.0f },
		{1817U,  20.0f },
		{2048U,  25.0f },
		{2270U,  30.0f },
		{2481U,  35.0f },
		{2676U,  40.0f },
		{2854U,  45.0f },
		{3014U,  50.0f },
		{3155U,  55.0f },
		{3280U,  60.0f },
		{3388U,  65.0f },
		{3482U,  70.0f },
		{3563U,  75.0f },
		{3633U,  80.0f },
		{3694U,  85.0f },
		{3745U,  90.0f },
		{3790U,  95.0f },
		{3828U, 100.0f },
		{3861U, 105.0f },
		{3889U, 110.0f },
		{3914U, 115.0f },
		{3935U, 120.0f },
		{3953U, 125.0f },
		{3969U, 130.0f },
		{3983U, 135.0f },
		{3995U, 140.0f },
		{4006U, 145.0f },
		{4015U, 150.0f }
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
    float *temperature)
{
    uint32_t index;
    float adc1;
    float adc2;
    float temp1;
    float temp2;

    if (temperature == NULL)
    {
        return false;
    }

    if (adc_value < ntc_lut[0U].adc)
    {
        return false;
    }

    if (adc_value > ntc_lut[NTC_LUT_SIZE - 1U].adc)
    {
        return false;
    }

    for (index = 0U;
         index < NTC_LUT_SIZE - 1U;
         index++)
    {
        if (adc_value <= ntc_lut[index + 1U].adc)
        {
            break;
        }
    }

    if (index >= NTC_LUT_SIZE - 1U)
    {
        return false;
    }

    adc1 =
        (float)ntc_lut[index].adc;

    adc2 =
        (float)ntc_lut[index + 1U].adc;

    temp1 =
        ntc_lut[index].temp;

    temp2 =
        ntc_lut[index + 1U].temp;

    *temperature =
        temp1 +
        (((float)adc_value - adc1) *
         (temp2 - temp1) /
         (adc2 - adc1));

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
	float delta;

    if (!ntc_filter_initialized[type])
    {
        ntc_temperature_filtered[type] =
            temperature;

        ntc_filter_initialized[type] =
            true;

        return temperature;
    }

    delta = temperature - ntc_temperature_filtered[type];

    ntc_temperature_filtered[type] += NTC_FILTER_ALPHA * delta;

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
