#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/i2c_master.h"

#include "ISoilMoistureSensor.h"
#include "IClimateSensor.h"
#include "AnalogSoilMoistureSensor.h"
#include "BME280ClimateSensor.h"
#include "BleServer.h"

static const char* TAG = "APP_MAIN";

// 1. Task is correctly defined BEFORE app_main
void sensor_task(void *pvParameters) {
    void** sensors = (void**)pvParameters;
    ISoilMoistureSensor* soil = (ISoilMoistureSensor*)sensors[0];
    IClimateSensor* climate = (IClimateSensor*)sensors[1];

    // Static variables for the unbuilt hardware components
    int node_id = 1;
    float mock_soil_temp = 24.2f;
    float mock_battery_volt = 4.1f;

    while (true) {
        float moisture = soil->getVolumetricWaterContent();
        float canopy_temp = climate->getTemperature();
        float canopy_hum = climate->getHumidity();

        ESP_LOGI(TAG, "Moisture: %.1f%% | Canopy Temp: %.1fC | Canopy RH: %.1f%%", moisture, canopy_temp, canopy_hum);
        
        // Push the full schema payload to the Bluetooth radio (All 6 arguments)
        BleServer::updateData(node_id, moisture, mock_soil_temp, canopy_temp, canopy_hum, mock_battery_volt); 

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

// 2. Main Entry Point
extern "C" void app_main(void) {
    ESP_LOGI(TAG, "Starting Smart Agriculture Node...");

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    BleServer::init(); 

    // Build the I2C Highway (Bus)
    i2c_master_bus_config_t i2c_bus_config = {}; 
    i2c_bus_config.i2c_port = -1;
    i2c_bus_config.sda_io_num = GPIO_NUM_21;
    i2c_bus_config.scl_io_num = GPIO_NUM_22;
    i2c_bus_config.clk_source = I2C_CLK_SRC_DEFAULT;
    i2c_bus_config.glitch_ignore_cnt = 7;
    i2c_bus_config.flags.enable_internal_pullup = true;

    i2c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_config, &bus_handle));

    // Add the BME280 Device (House) to the Highway
    i2c_device_config_t bme_config = {}; 
    bme_config.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    bme_config.device_address = 0x76; 
    bme_config.scl_speed_hz = 100000; 
    
    i2c_master_dev_handle_t bme_handle;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &bme_config, &bme_handle));

    ISoilMoistureSensor* soilSensor = new AnalogSoilMoistureSensor(ADC_CHANNEL_6, 3000, 1500); 
    IClimateSensor* climateSensor = new BME280ClimateSensor(bme_handle);

    static void* sensor_ptrs[2] = { soilSensor, climateSensor };
    xTaskCreate(sensor_task, "SensorTask", 4096, sensor_ptrs, 5, NULL);
}