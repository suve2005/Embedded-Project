#pragma once

class IClimateSensor {
public:
    virtual ~IClimateSensor() = default;
    virtual float getTemperature() = 0;
    virtual float getHumidity() = 0;
};