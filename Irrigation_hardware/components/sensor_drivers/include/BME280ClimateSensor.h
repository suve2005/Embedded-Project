#pragma once
#include "IClimateSensor.h"
#include "driver/i2c_master.h"

class BME280ClimateSensor : public IClimateSensor {
private:
    i2c_master_dev_handle_t i2c_dev;
    float temperature;
    float humidity;

public:
    BME280ClimateSensor(i2c_master_dev_handle_t i2c_device_handle);
    ~BME280ClimateSensor() override;

    float getTemperature() override;
    float getHumidity() override;
};