/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>

#include <net/net_context.h>
#include <net/net_mgmt.h>

#include <misc/printk.h>
#include <string.h>
#include <stdio.h>

#include "tb_pubsub.h"
#include "sensors.h"
#include "lights.h"
#include "config.h"

#define ATTR_UPDATE_INTERVAL 15000

/* Callback to tell us when we have a net connection */
#if defined(CONFIG_NET_MGMT_EVENT)
static struct net_mgmt_event_callback cb;
#endif

static void update_attributes()
{
	static char payload[128];

	/* Formulate the JSON payload for the attribute update */
	snprintf(payload, sizeof(payload), "{\"firmware_version\":\"%s\", \"serial_number\":\"%s\", \"uptime\":\"%d\"}",
		"1.2.3.4",
		"jdukes-001-team-some",
		(uint32_t)k_uptime_get_32() / 1000);

	tb_publish_attributes(payload);
}

static void event_iface_up(struct net_mgmt_event_callback *cb,
			   u32_t mgmt_event, struct net_if *iface)
{
	/* Interface is ready - start main application activities */
	tb_pubsub_start();
	sensors_start();
	lights_init();
}

void main(void)
{
	struct net_if *iface = net_if_get_default();

	/* We want to delay starting main application activities until the
	 * network interface is ready
	 */

#if defined(CONFIG_NET_MGMT_EVENT)
	if (!atomic_test_bit(iface->flags, NET_IF_UP)) {
		/* Subscribe to the interface IP_UP event notification */
		net_mgmt_init_event_callback(&cb, event_iface_up, NET_EVENT_IF_UP);
		net_mgmt_add_event_callback(&cb);
	}
#else
	event_iface_up(NULL, NET_EVENT_IF_UP, iface);
#endif

	/* Loop forever, only waking periodically to trigger an update of our
	 * published attributes
	 */

	while (true) {
		k_sleep(ATTR_UPDATE_INTERVAL);
   		update_attributes();
	}

	return;
}
