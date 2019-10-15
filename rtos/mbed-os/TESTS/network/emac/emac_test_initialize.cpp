/*
 * Copyright (c) 2017, ARM Limited, All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <inttypes.h>
#include "mbed.h"
#include "mbed_stats.h"
#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"

#include "NetworkInterface.h"
#include "EMAC.h"
#include "EMACMemoryManager.h"
#include "emac_TestMemoryManager.h"
#include "emac_TestNetworkStack.h"
#include "emac_initialize.h"
#include "emac_tests.h"
#include "emac_util.h"

using namespace utest::v1;

static unsigned char eth_mac_addr[ETH_MAC_ADDR_LEN];
EMAC *emac_handle = NULL;

void test_emac_initialize()
{
    worker_loop_init();

    static NetworkInterface *network_interface = NetworkInterface::get_default_instance();

#define WIFI 2
#if MBED_CONF_TARGET_NETWORK_DEFAULT_INTERFACE_TYPE == WIFI
#if MBED_CONF_APP_WIFI_SCAN
    WiFiAccessPoint ap[30];

    int size = network_interface->wifiInterface()->scan(ap, 30);

    for (int i = 0; i < size; i++) {
        const char *ssid = ap[i].get_ssid();
        nsapi_security_t security = ap[i].get_security();
        int8_t rssi = ap[i].get_rssi();
        char ch = ap[i].get_channel();

        printf("BS %i\r\n", i);
        printf("ssid %s\r\n", ssid);
        printf("security %i\r\n", security);
        printf("rssi %i\r\n", rssi);
        printf("ch %i\r\n\r\n", ch);
    }
#endif
#endif

    // Power up the interface and emac driver
    TEST_ASSERT_EQUAL_INT(NSAPI_ERROR_OK, network_interface->connect());

    worker_loop_link_up_wait();
}

unsigned char *emac_if_get_hw_addr(void)
{
    return &eth_mac_addr[0];
}

EMAC *emac_if_get(void)
{
    return emac_handle;
}

EmacTestMemoryManager *emac_m_mngr_get(void)
{
    return &EmacTestMemoryManager::get_instance();
}

bool emac_if_init(EMAC *emac)
{
    emac_handle = emac;

    emac->set_link_input_cb(emac_if_link_input_cb);
    emac->set_link_state_cb(emac_if_link_state_change_cb);

    if (!emac->power_up()) {
        TEST_ASSERT_MESSAGE(0, "emac power up failed!");
    }

    int hwaddr_len = emac->get_hwaddr_size();
    printf("emac hwaddr length %i\r\n\r\n", hwaddr_len);

    TEST_ASSERT_MESSAGE(hwaddr_len == 6, "invalid emac hwaddr length!");

    // If driver updates this, write it back, otherwise write default from mbed_mac_address
    mbed_mac_address(reinterpret_cast<char *>(&eth_mac_addr[0]));
    emac->get_hwaddr(eth_mac_addr);
    emac->set_hwaddr(eth_mac_addr);
    printf("emac hwaddr %x:%x:%x:%x:%x:%x\r\n\r\n", eth_mac_addr[0], eth_mac_addr[1], eth_mac_addr[2], eth_mac_addr[3], eth_mac_addr[4], eth_mac_addr[5]);

    int mtu_size = emac->get_mtu_size();
    printf("emac mtu %i\r\n\r\n", mtu_size);
    emac_if_set_mtu_size(mtu_size);

    char hw_name[11];
    emac->get_ifname(hw_name, 10);
    printf("emac if name %s\r\n\r\n", hw_name);

    return true;
}
