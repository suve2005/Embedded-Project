#include "AnalogSoilMoistureSensor.h"
#include "esp_log.h"

static const char* TAG = "SOIL_SENSOR";

AnalogSoilMoistureSensor::AnalogSoilMoistureSensor(adc_channel_t adc_channel, int dry_calibration, int wet_calibration)
    : channel(adc_channel), dry_adc_val(dry_calibration), wet_adc_val(wet_calibration) 
{
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
        .clk_src = ADC_RTC_CLK_SRC_DEFAULT,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &this->adc_handle));

    adc_oneshot_chan_cfg_t chan_config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(this->adc_handle, this->channel, &chan_config));

    ESP_LOGI(TAG, "Soil sensor initialized on ADC1 Channel %d", this->channel);
}

AnalogSoilMoistureSensor::~AnalogSoilMoistureSensor() {
    adc_oneshot_del_unit(this->adc_handle);
}

int AnalogSoilMoistureSensor::getRawADCValue() {
    int raw_value = 0;
    ESP_ERROR_CHECK(adc_oneshot_read(this->adc_handle, this->channel, &raw_value));
    return raw_value;
}

float AnalogSoilMoistureSensor::getVolumetricWaterContent() {
    int raw = this->getRawADCValue();
    float percentage = (float)(this->dry_adc_val - raw) * 100.0f / (float)(this->dry_adc_val - this->wet_adc_val);

    if (percentage < 0.0f) percentage = 0.0f;
    else if (percentage > 100.0f) percentage = 100.0f;

    return percentage;
}