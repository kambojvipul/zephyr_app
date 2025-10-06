
#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/services/bas.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/logging/log.h>

#include <zephyr/arch/cpu.h>
#include <zephyr/sys/reboot.h>

#include "app_config.h"
#include "app_events.h"
#include "app_uuids.h"
LOG_MODULE_REGISTER(ble, LOG_LEVEL_INF);


extern uint16_t val_mv;
static uint16_t voltage_mv = 0;
uint8_t ble_connection = 0;


static const struct bt_uuid_128 voltage_svc_uuid = BT_UUID_INIT_128(
	BT_UUID_CUSTOM_SERVICE_VAL);

static const struct bt_uuid_128 voltage_chrc_uuid = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef1));

static const struct bt_uuid_128 sample_interval_uuid = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef2));


static ssize_t read_u16(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			void *buf, uint16_t len, uint16_t offset)
{
	const uint16_t *u16 = attr->user_data;
	uint16_t value = sys_cpu_to_le16(*u16);

	return bt_gatt_attr_read(conn, attr, buf, len, offset, &value,
				 sizeof(value));
}

// CCC (Client Characteristic Configuration) callback for notifications
static void voltage_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    bool notif_enabled = (value == BT_GATT_CCC_NOTIFY);
    LOG_INF("Voltage Notifications %s\n", notif_enabled ? "enabled" : "disabled");
}


// GATT Service Declaration
BT_GATT_SERVICE_DEFINE(voltage_svc,
    BT_GATT_PRIMARY_SERVICE(&voltage_svc_uuid),

    // Characteristic: Voltage Measurement (Read and Notify)
    BT_GATT_CHARACTERISTIC(&voltage_chrc_uuid.uuid,
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_READ,
                           read_u16, NULL, &voltage_mv),


    // CCC Descriptor: Allows mobile client to enable notifications
    BT_GATT_CCC(voltage_ccc_cfg_changed,
                BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
    
    
       BT_GATT_CHARACTERISTIC(&sample_interval_uuid.uuid,
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_READ,
                           read_u16, NULL, &sample_interval_ms),

);

// Advertising Data structure
static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA_BYTES(BT_DATA_GAP_APPEARANCE, 0x00, 0x03),
	BT_DATA_BYTES(BT_DATA_UUID16_ALL,
		      BT_UUID_16_ENCODE(BT_UUID_ESS_VAL),
		      BT_UUID_16_ENCODE(BT_UUID_BAS_VAL)),
};

static const struct bt_data sd[] = {
	BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, sizeof(CONFIG_BT_DEVICE_NAME) - 1),
};


// Advertise BLE
void bt_ready(void)
{
	int err;

	LOG_INF("Bluetooth initialized\n");

     // First stop any ongoing advertising
     k_msleep(300);
    err = bt_le_adv_stop();
    if (err) {
        LOG_WRN("Failed to stop advertising (err %d), continuing anyway\n", err);
    }

    k_msleep(300);
	err = bt_le_adv_start(BT_LE_ADV_CONN_FAST_1, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
	if (err) {
		LOG_ERR("BLE Advertising failed to start (err %d)\n", err);
		sys_reboot(SYS_REBOOT_COLD);

	}


	LOG_INF("BLE Advertising successfully started\n");
}

static void connected(struct bt_conn *conn, uint8_t err)
{
	if (err) {
		LOG_INF("Connection failed, err 0x%02x %s\n", err, bt_hci_err_to_str(err));
	} else {
		LOG_INF("BLE Connected\n");
        ble_connection = 1;
	}
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	LOG_INF("BLE Disconnected, reason 0x%02x %s\n", reason, bt_hci_err_to_str(reason));
    ble_connection = 0;
        bt_enable(NULL);
        bt_ready(); //Advertise Again

}

BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected = connected,
	.disconnected = disconnected,
};




//ble notify handler

void ble_notify_handler(struct k_work *work)
{
    //int ret = 0;
            struct k_work_delayable *dwork = k_work_delayable_from_work(work);
            voltage_mv = val_mv;    
        //update gatt characteristic value
    if(enable_notification && ble_connection)
    {
          bt_gatt_notify(NULL, &voltage_svc.attrs[1], &voltage_mv, sizeof(voltage_mv));
            LOG_INF("BLE Notified. \n");
        }
        k_work_schedule(dwork, K_MSEC(sample_interval_ms));
}


 int ble_init()
{
      // 2. BLE Initialization
    int ret = 0;
    ret = bt_enable(NULL);
    if (ret) {
        LOG_ERR("Bluetooth initialization failed (ret %d)\n", ret);
        return ret;
    }
    bt_ready(); //Advertise BLE
    return 0;
}

