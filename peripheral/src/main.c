/* main.c - Application main entry point */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <sys/printk.h>
#include <sys/byteorder.h>
#include <zephyr.h>

#include <logging/log.h>
LOG_MODULE_REGISTER(testservice, LOG_LEVEL_DBG);

void main(void)
{
	LOG_INF("boot: Maximum throughput example");

	ble__enable();

	while (1) {
		k_sleep(K_SECONDS(1));
	}
}
