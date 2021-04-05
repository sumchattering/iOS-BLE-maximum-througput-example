
#include <logging/log.h>
LOG_MODULE_REGISTER(test_service, LOG_LEVEL_DBG);

#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>
#include <bluetooth/gatt.h>
#include <logging/log.h>
#include <stdint.h>

static struct bt_conn *current_conn = NULL;

#define UUID_BASE_W32            0xc3dc0000
#define UUID_BASE_W1             0x78f8
#define UUID_BASE_W2             0x44de
#define UUID_BASE_W3             0xbc0f
#define UUID_BASE_W48            0x05291d85f5aa
#define UUID_128_ENCODE(uuid_16) BT_UUID_128_ENCODE(UUID_BASE_W32 | (uuid_16), UUID_BASE_W1, UUID_BASE_W2, UUID_BASE_W3, UUID_BASE_W48)

#define TEST_SERVICE_UUID BT_UUID_DECLARE_128(UUID_128_ENCODE(0x0001))
#define TEST_CHARACTERISTIC_UUID  BT_UUID_DECLARE_128(UUID_128_ENCODE(0x0002))
#define TEST_CHARACTERISTIC_PROPS (BT_GATT_CHRC_NOTIFY)
#define TEST_CHARACTERISTIC_PERM  (BT_GATT_PERM_NONE)

static void test__ccc_cb(const struct bt_gatt_attr *attr, uint16_t value);
#define CCC_PERM (BT_GATT_PERM_READ | BT_GATT_PERM_WRITE)

BT_GATT_SERVICE_DEFINE(
    test_service,
    BT_GATT_PRIMARY_SERVICE(TEST_SERVICE_UUID),
    BT_GATT_CHARACTERISTIC(TEST_CHARACTERISTIC_UUID, TEST_CHARACTERISTIC_PROPS, TEST_CHARACTERISTIC_PERM, NULL, NULL, NULL),
    BT_GATT_CCC(test__ccc_cb, CCC_PERM), );

static void test__ccc_cb(const struct bt_gatt_attr *attr, uint16_t value)
{
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