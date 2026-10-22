#include "BleServer.h"
#include "host/ble_hs.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include <stdio.h>
#include <string.h>

static char sensor_data_str[64] = "Waiting for sensor data...";

static int gatt_svr_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg) 
{
    int rc = os_mbuf_append(ctxt->om, sensor_data_str, strlen(sensor_data_str));
    return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
}

static ble_uuid16_t env_sensing_uuid;
static ble_uuid16_t analog_output_uuid;
static struct ble_gatt_chr_def chr_defs[2];
static struct ble_gatt_svc_def gatt_svr_svcs[2];
static uint8_t own_addr_type;

static void ble_app_advertise(void) {
    struct ble_gap_adv_params adv_params;
    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

    struct ble_hs_adv_fields fields;
    memset(&fields, 0, sizeof(fields));
    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
    fields.name = (uint8_t *)"SmartAgri";
    fields.name_len = 9;
    fields.name_is_complete = 1;
    ble_gap_adv_set_fields(&fields);

    ble_gap_adv_start(own_addr_type, NULL, BLE_HS_FOREVER, &adv_params, NULL, NULL);
}

static void ble_app_on_sync(void) {
    ble_hs_id_infer_auto(0, &own_addr_type);
    ble_app_advertise();
}

static void host_task(void *param) {
    nimble_port_run();
    nimble_port_freertos_deinit();
}

void BleServer::init() {
    env_sensing_uuid.u.type = BLE_UUID_TYPE_16;
    env_sensing_uuid.value = 0x181A; 

    analog_output_uuid.u.type = BLE_UUID_TYPE_16;
    analog_output_uuid.value = 0x2A59; 

    memset(chr_defs, 0, sizeof(chr_defs));
    chr_defs[0].uuid = &analog_output_uuid.u;
    chr_defs[0].access_cb = gatt_svr_chr_access;
    chr_defs[0].flags = BLE_GATT_CHR_F_READ;

    memset(gatt_svr_svcs, 0, sizeof(gatt_svr_svcs));
    gatt_svr_svcs[0].type = BLE_GATT_SVC_TYPE_PRIMARY;
    gatt_svr_svcs[0].uuid = &env_sensing_uuid.u;
    gatt_svr_svcs[0].characteristics = chr_defs;

    nimble_port_init();
    ble_svc_gap_device_name_set("SmartAgri");
    ble_svc_gap_init();
    ble_svc_gatt_init();
    ble_gatts_count_cfg(gatt_svr_svcs);
    ble_gatts_add_svcs(gatt_svr_svcs);
    ble_hs_cfg.sync_cb = ble_app_on_sync;
    nimble_port_freertos_init(host_task); 
}

// 6-Argument Function Signature exactly matches your BleServer.h
void BleServer::updateData(int node_id, float soil_moisture, float soil_temp, float canopy_temp, float canopy_rh, float battery_volts) {
    snprintf(sensor_data_str, sizeof(sensor_data_str), "%d,%.1f,%.1f,%.1f,%.1f,%.1f", 
             node_id, soil_moisture, soil_temp, canopy_temp, canopy_rh, battery_volts);
}