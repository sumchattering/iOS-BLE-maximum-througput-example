#ifndef TEST_SERVICE_H_
#define TEST_SERVICE_H_

#include <bluetooth/conn.h>

void test_service__notify_connect(struct bt_conn *conn);
void test_service__notify_disconnect(struct bt_conn *conn);

#endif  // BLE_DOWNLOAD_SERVICE_H_
