#include "ble.h"

#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>
#include <logging/log.h>

#include "test_service.h"

LOG_MODULE_REGISTER(ble, LOG_LEVEL_DBG);

// Advertising Data

#define ADV_PARAMS BT_LE_ADV_PARAM(BT_LE_ADV_OPT_USE_NAME | BT_LE_ADV_OPT_CONNECTABLE, BT_GAP_ADV_FAST_INT_MIN_1, BT_GAP_ADV_FAST_INT_MAX_1, NULL)

static const struct bt_data advertising_data[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID128_ALL,
		      0xf0, 0xde, 0xbc, 0x9a, 0x78, 0x56, 0x34, 0x12,
		      0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12),
};

// Connection Callbacks

static void connected_cb(struct bt_conn *conn, uint8_t conn_err)
{
    if (conn_err) {
        LOG_ERR("connection failed: 0x%02x", conn_err);
        return;
    }

    LOG_INF("connected");
    test_service__notify_connect(conn);
}

static void disconnected_cb(struct bt_conn *conn, uint8_t reason)
{
    LOG_INF("disconnected: 0x%02x", reason);
    test_service__notify_disconnect(conn);
}

static struct bt_conn_cb conn_callbacks = {
    .connected    = connected_cb,
    .disconnected = disconnected_cb,
};

// Ready Callback

static void ready_cb(int err)
{
    if (err) {
		LOG_ERR("Bluetooth init failed (err %d)\n", err);
        return;
    }

    bt_conn_cb_register(&conn_callbacks);

    int rc = bt_le_adv_start(ADV_PARAMS, advertising_data, ARRAY_SIZE(advertising_data), NULL, 0);
    if (rc) {
    	LOG_ERR("Bluetooth advertising failed [%d]", rc);
    } else {
    	LOG_INF("Bluetooth is ready and started Advertising");
    }
}

void ble__enable(void)
{
    int err = bt_enable(ready_cb);
	if (err) {
		LOG_ERR("Bluetooth enable failed (err %d)\n", err);
		return;
	}
}