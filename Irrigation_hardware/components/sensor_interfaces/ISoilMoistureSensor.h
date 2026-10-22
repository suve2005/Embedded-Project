#pragma once

class ISoilMoistureSensor
{   public:
     ISoilMoistureSensor(){}
     virtual ~ISoilMoistureSensor(){} 
     virtual float getVolumetricWaterContent() = 0;
     virtual int getRawADCValue() = 0;

};
