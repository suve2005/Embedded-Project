idf_component_register(SRCS "AnalogSoilMoistureSensor.cpp" "BME280ClimateSensor.cpp"
                       INCLUDE_DIRS "include"
                       REQUIRES sensor_interfaces esp_adc driver esp_driver_i2c)