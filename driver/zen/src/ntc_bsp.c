/**
 * -----------------------------------------------------------------------------
 * @auth            : manhtd
 * @day             : 19-Sep-2024
 * @file            : ntc_bsp.c
 * @description     : NTC BSP for STM32F0xx MCU
 * -----------------------------------------------------------------------------
 */

/* Indent using spaces
 * Tab width   : 4 spaces
 * Line width  : 100 characters
 * Line ending : LF (0x0A)
 */

/* Public macros ----------------------------------------------------------------*/

/* Include ----------------------------------------------------------------------*/

/* Global configuration */
#include "zcfg.h"

/* Middleware */
#include "ntc.h"

/* BSP */
#include "ntc_bsp.h"

/* C Library */
#include "stdbool.h"
#include "stdint.h"

/* ST Driver */
#include "stm32f0xx_hal.h"

/* Define -----------------------------------------------------------------------*/

#define NUMBER_BSP_NTC                  2U

#define NTC_TEMP_BSP_CHANNEL            0U
#define NTC_BOILER_BSP_CHANNEL          1U

#define NTC_TEMP_BSP_PIN                GPIO_PIN_0
#define NTC_TEMP_BSP_GPIO_PORT          GPIOB
#define NTC_TEMP_BSP_ADC_CHANNEL        ADC_CHANNEL_8

#define NTC_BOILER_BSP_PIN              GPIO_PIN_1
#define NTC_BOILER_BSP_GPIO_PORT        GPIOB
#define NTC_BOILER_BSP_ADC_CHANNEL      ADC_CHANNEL_9

#define NTC_BSP_GPIO_CLK_ENB()          __HAL_RCC_GPIOB_CLK_ENABLE()
#define NTC_BSP_DMA_CLK_ENB()           __HAL_RCC_DMA1_CLK_ENABLE()
#define NTC_BSP_ADC_CLK_ENB()           __HAL_RCC_ADC1_CLK_ENABLE()

#define NTC_BSP_DMA_IRQn                DMA1_Channel1_IRQn

/* Private data types -----------------------------------------------------------*/

typedef struct
{
    uint8_t             b_is_open;
    ADC_HandleTypeDef   hadc;
    DMA_HandleTypeDef   hdma;
    uint16_t            val_temp[NUMBER_BSP_NTC];

} ntc_bsp_h_t;

/* Public data types ------------------------------------------------------------*/

/* Exported constants -----------------------------------------------------------*/

/* Private data -----------------------------------------------------------------*/

static ntc_bsp_h_t gh_ntc = {0};

static volatile bool ntc_dma_ready = false;

/* Private function prototypes -------------------------------------------------*/

/* Public function bodies ------------------------------------------------------*/

/**
 * @brief      Open NTC ADC and DMA.
 * @retval     ZERR_OK    if successful.
 * @retval     ZERR_FALSE if failed.
 */
zerr_t
ntc_bsp_open(void)
{
    zerr_t ret = ZERR_OK;

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    ADC_ChannelConfTypeDef sConfig = {0};

    if (gh_ntc.b_is_open != 0U)
    {
        return ZERR_OPEN_ALREADY;
    }

    /*
     * Reset BSP state.
     */
    gh_ntc.val_temp[NTC_TEMP_BSP_CHANNEL] = 0U;
    gh_ntc.val_temp[NTC_BOILER_BSP_CHANNEL] = 0U;

    ntc_dma_ready = false;

    /*
     * Enable peripheral clocks.
     */
    NTC_BSP_GPIO_CLK_ENB();
    NTC_BSP_DMA_CLK_ENB();
    NTC_BSP_ADC_CLK_ENB();

    /*
     * Configure NTC GPIO.
     *
     * PB0 -> ADC1_IN8 -> NTC temperature sensor.
     * PB1 -> ADC1_IN9 -> NTC boiler sensor.
     */
    GPIO_InitStruct.Pin =
        NTC_TEMP_BSP_PIN |
        NTC_BOILER_BSP_PIN;

    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;

    HAL_GPIO_Init(
        NTC_TEMP_BSP_GPIO_PORT,
        &GPIO_InitStruct);

    /*
     * Configure DMA.
     *
     * ADC1
     *   |
     *   +--> DMA1_Channel1
     *           |
     *           +--> gh_ntc.val_temp[]
     */
    gh_ntc.hdma.Instance = DMA1_Channel1;

    gh_ntc.hdma.Init.Direction = DMA_PERIPH_TO_MEMORY;
    gh_ntc.hdma.Init.PeriphInc = DMA_PINC_DISABLE;
    gh_ntc.hdma.Init.MemInc = DMA_MINC_ENABLE;
    gh_ntc.hdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    gh_ntc.hdma.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    gh_ntc.hdma.Init.Mode = DMA_CIRCULAR;
    gh_ntc.hdma.Init.Priority = DMA_PRIORITY_LOW;

    if (HAL_DMA_Init(&gh_ntc.hdma) != HAL_OK)
    {
        return ZERR_FALSE;
    }

    /*
     * Link DMA with ADC.
     */
    __HAL_LINKDMA(
        &gh_ntc.hadc,
        DMA_Handle,
        gh_ntc.hdma);

    /*
     * Configure DMA interrupt.
     */
    HAL_NVIC_SetPriority(
        NTC_BSP_DMA_IRQn,
        6U,
        0U);

    HAL_NVIC_EnableIRQ(
        NTC_BSP_DMA_IRQn);

    /*
     * Configure ADC.
     *
     * NOTE (fix): ContinuousConvMode MUST be ENABLE here.
     * The DMA channel is configured as DMA_CIRCULAR, which expects the
     * ADC to keep re-triggering conversions on its own. With
     * ContinuousConvMode = DISABLE and ExternalTrigConv = SOFTWARE_START,
     * the ADC performs the 2-channel scan exactly once after
     * HAL_ADC_Start_DMA() and then stops permanently, so ntc_bsp_ready()
     * only ever becomes true a single time and temperature readings
     * never refresh again. Enabling ContinuousConvMode makes the ADC
     * free-run the scan sequence continuously, matching the circular
     * DMA buffer, while the ISR itself stays cheap (it only raises a
     * flag) - the actual LUT lookup + IIR filtering is still done only
     * when ntc_update() is called from the application loop.
     */
    gh_ntc.hadc.Instance = ADC1;

    gh_ntc.hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
    gh_ntc.hadc.Init.Resolution = ADC_RESOLUTION_12B;
    gh_ntc.hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    gh_ntc.hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
    gh_ntc.hadc.Init.EOCSelection = ADC_EOC_SEQ_CONV;
    gh_ntc.hadc.Init.LowPowerAutoWait = DISABLE;
    gh_ntc.hadc.Init.LowPowerAutoPowerOff = DISABLE;
    gh_ntc.hadc.Init.ContinuousConvMode = ENABLE;
    gh_ntc.hadc.Init.DiscontinuousConvMode = DISABLE;
    gh_ntc.hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    gh_ntc.hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    gh_ntc.hadc.Init.DMAContinuousRequests = ENABLE;
    gh_ntc.hadc.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;

    if (HAL_ADC_Init(&gh_ntc.hadc) != HAL_OK)
    {
        return ZERR_FALSE;
    }

    /*
     * Configure NTC temperature channel.
     *
     * PB0 -> ADC_CHANNEL_8
     */
    sConfig.Channel = NTC_TEMP_BSP_ADC_CHANNEL;
    sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
    sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;

    if (HAL_ADC_ConfigChannel(
            &gh_ntc.hadc,
            &sConfig) != HAL_OK)
    {
        return ZERR_FALSE;
    }

    /*
     * Configure NTC boiler channel.
     *
     * PB1 -> ADC_CHANNEL_9
     */
    sConfig.Channel = NTC_BOILER_BSP_ADC_CHANNEL;
    sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
    sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;

    if (HAL_ADC_ConfigChannel(
            &gh_ntc.hadc,
            &sConfig) != HAL_OK)
    {
        return ZERR_FALSE;
    }

    /*
     * Calibrate ADC before first conversion.
     */
    if (HAL_ADCEx_Calibration_Start(&gh_ntc.hadc) != HAL_OK)
    {
        return ZERR_FALSE;
    }

    gh_ntc.b_is_open = 1U;

    return ret;
}

/**
 * @brief      Start ADC DMA conversion.
 * @retval     ZERR_OK    if successful.
 * @retval     ZERR_FALSE if failed.
 */
zerr_t
ntc_bsp_start(void)
{
    if (gh_ntc.b_is_open == 0U)
    {
        return ZERR_NOT_OPEN;
    }

    ntc_dma_ready = false;

    if (HAL_ADC_Start_DMA(
            &gh_ntc.hadc,
            (uint32_t *)gh_ntc.val_temp,
            NUMBER_BSP_NTC) != HAL_OK)
    {
        return ZERR_FALSE;
    }

    return ZERR_OK;
}

/**
 * @brief      Check ADC DMA data ready.
 * @retval     true  if data is ready.
 * @retval     false if data is not ready.
 */
bool
ntc_bsp_ready(void)
{
    return ntc_dma_ready;
}

/**
 * @brief      Get raw ADC value.
 * @param[in]  channel: NTC DMA channel index.
 * @retval     Raw ADC value.
 */
uint16_t
ntc_bsp_get_raw(uint8_t channel)
{
    if (channel >= NUMBER_BSP_NTC)
    {
        return 0U;
    }

//    return gh_ntc.val_temp[channel];
    return gh_ntc.val_temp[0];
}

/**
 * @brief      Clear ADC DMA ready flag.
 * @retval     None.
 */
void
ntc_bsp_clear_ready(void)
{
    ntc_dma_ready = false;
}

/**
 * @brief      ADC conversion complete callback.
 * @param[in]  hadc: ADC handle.
 * @retval     None.
 */
void
HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc == &gh_ntc.hadc)
    {
        ntc_dma_ready = true;
    }
}

/**
 * @brief      NTC DMA interrupt service routine.
 * @retval     None.
 */
void
ntc_bsp_dma_isr(void)
{
    HAL_DMA_IRQHandler(&gh_ntc.hdma);
}

/* END OF FILE ----------------------------------------------------------------*/
