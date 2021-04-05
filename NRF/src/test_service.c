
#include <logging/log.h>
LOG_MODULE_REGISTER(test_service, LOG_LEVEL_DBG);

#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>
#include <bluetooth/gatt.h>
#include <logging/log.h>
#include <stdint.h>

static struct bt_conn *current_conn = NULL;

static struct bt_uuid_128 test_service_uuid = BT_UUID_INIT_128(
	0xf0, 0xde, 0xbc, 0x9a, 0x78, 0x56, 0x34, 0x12,
	0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12);

static struct bt_uuid_128 test_charactersitic_uuid = BT_UUID_INIT_128(
	0xf1, 0xde, 0xbc, 0x9a, 0x78, 0x56, 0x34, 0x12,
	0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12);

#define TEST_SERVICE_UUID BT_UUID_DECLARE_128(test_service_uuid)

#define TEST_CHARACTERISTIC_UUID  BT_UUID_DECLARE_128(test_charactersitic_uuid)
#define TEST_CHARACTERISTIC_PROPS (BT_GATT_CHRC_NOTIFY)
#define TEST_CHARACTERISTIC_PERM  (BT_GATT_PERM_NONE)

BT_GATT_SERVICE_DEFINE(
    test_service,
    BT_GATT_PRIMARY_SERVICE(TEST_SERVICE_UUID),
    BT_GATT_CHARACTERISTIC(TEST_CHARACTERISTIC_UUID, TEST_CHARACTERISTIC_PROPS, TEST_CHARACTERISTIC_PERM, NULL, NULL, NULL),
    BT_GATT_CCC(test__ccc_cb, CCC_PERM), );

static void test__ccc_cb(const struct bt_gatt_attr *attr, uint16_t value)
{
    int rc;

    if (value & BT_GATT_CCC_NOTIFY) {
        LOG_INF("Subscribed to sending data");
    } else {
        LOG_INF("unsubscribed to sending data");
    }
}


// App API calls ///////////////////////////////////////////////////////////////
void test_service__notify_connect(struct bt_conn *conn)
{
    if (current_conn != NULL) {
        LOG_ERR("unexpected connection: %p, %p", conn, current_conn);
        return;
    }

    current_conn = bt_conn_ref(conn);
}

void test_service__notify_disconnect(struct bt_conn *conn)
{
    if (conn != current_conn) {
        LOG_ERR("incorrect connection: %p, %p", conn, current_conn);
        return;
    }

    bt_conn_unref(conn);
    current_conn = NULL;
}