#include "BME280ClimateSensor.h"
#include "esp_log.h"

static const char* TAG = "BME280";

#define BME280_REG_CTRL_HUM   0xF2
#define BME280_REG_CTRL_MEAS  0xF4
#define BME280_REG_TEMP_MSB   0xFA
#define BME280_REG_HUM_MSB    0xFD

BME280ClimateSensor::BME280ClimateSensor(i2c_master_dev_handle_t i2c_device_handle)
    : i2c_dev(i2c_device_handle), temperature(0.0f), humidity(0.0f)
{
    uint8_t hum_cfg[2] = {BME280_REG_CTRL_HUM, 0x01};
    ESP_ERROR_CHECK(i2c_master_transmit(this->i2c_dev, hum_cfg, sizeof(hum_cfg), 100));

    uint8_t meas_cfg[2] = {BME280_REG_CTRL_MEAS, 0x27};
    ESP_ERROR_CHECK(i2c_master_transmit(this->i2c_dev, meas_cfg, sizeof(meas_cfg), 100));

    ESP_LOGI(TAG, "BME280 sensor initialized successfully");
}

BME280ClimateSensor::~BME280ClimateSensor() {
    i2c_master_bus_rm_device(this->i2c_dev);
}

float BME280ClimateSensor::getTemperature() {
    uint8_t reg_addr = BME280_REG_TEMP_MSB;
    uint8_t raw_bytes[3] = {0};

    ESP_ERROR_CHECK(i2c_master_transmit_receive(this->i2c_dev, &reg_addr, 1, raw_bytes, sizeof(raw_bytes), 100));
    
    int32_t raw_temp = ((int32_t)raw_bytes[0] << 12) | ((int32_t)raw_bytes[1] << 4) | (raw_bytes[2] >> 4);
    this->temperature = (float)raw_temp / 5120.0f;
    return this->temperature;
}

float BME280ClimateSensor::getHumidity() {
    uint8_t reg_addr = BME280_REG_HUM_MSB;
    uint8_t raw_bytes[2] = {0};

    ESP_ERROR_CHECK(i2c_master_transmit_receive(this->i2c_dev, &reg_addr, 1, raw_bytes, sizeof(raw_bytes), 100));
    
    int32_t raw_hum = ((int32_t)raw_bytes[0] << 8) | raw_bytes[1];
    this->humidity = (float)raw_hum / 1024.0f;
    return this->humidity;
}