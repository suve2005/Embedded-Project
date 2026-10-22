#pragma once
#include "ISoilMoistureSensor.h"
#include "esp_adc/adc_oneshot.h"

class AnalogSoilMoistureSensor : public ISoilMoistureSensor {
private:
    adc_oneshot_unit_handle_t adc_handle;
    adc_channel_t channel;
    int dry_adc_val;
    int wet_adc_val;

    int getRawADCValue();

public:
    AnalogSoilMoistureSensor(adc_channel_t adc_channel, int dry_calibration, int wet_calibration);
    ~AnalogSoilMoistureSensor() override;

    float getVolumetricWaterContent() override;
};