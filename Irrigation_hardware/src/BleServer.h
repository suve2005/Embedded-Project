#pragma once

class BleServer {
public:
    static void init();
    
    // Updated to accept all required FastAPI fields
    static void updateData(int node_id, float soil_moisture, float soil_temp, float canopy_temp, float canopy_rh, float battery_volts);
};