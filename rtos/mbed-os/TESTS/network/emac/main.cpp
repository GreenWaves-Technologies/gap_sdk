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

#if !defined(MBED_CONF_APP_ECHO_SERVER)       || \
    !defined(MBED_CONF_APP_ECHO_SERVER_TRACE) || \
    !defined(MBED_CONF_APP_WIFI_SCAN)
#error [NOT_SUPPORTED] Requires parameters from mbed_app.json
#endif

#define ETHERNET 1
#define WIFI 2

#if MBED_CONF_TARGET_NETWORK_DEFAULT_INTERFACE_TYPE != ETHERNET && \
    MBED_CONF_TARGET_NETWORK_DEFAULT_INTERFACE_TYPE != WIFI
#error [NOT_SUPPORTED] Either wifi or ethernet testing need to be enabled
#endif

#if MBED_CONF_TARGET_NETWORK_DEFAULT_INTERFACE_TYPE == WIFI
#if !defined(TARGET_UBLOX_EVK_ODIN_W2)      && \
    !defined(TARGET_REALTEK_RTL8195AM)      && \
    !defined(TARGET_MTB_ADV_WISE_1530)      && \
    !defined(TARGET_MTB_USI_WM_BN_BM_22)    && \
    !defined(TARGET_MTB_MXCHIP_EMW3166)     && \
    !defined(TARGET_MTB_UBLOX_ODIN_W2)      && \
    !defined(TARGET_UNO_91H)
#error [NOT_SUPPORTED] Wifi tests are not valid for the target
#endif
#endif

#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"

#include "emac_tests.h"
#include "emac_util.h"

using namespace utest::v1;

// Test setup
utest::v1::status_t test_setup(const size_t number_of_cases)
{
#if !MBED_CONF_APP_ECHO_SERVER
#ifdef MBED_GREENTEA_TEST_EMAC_TIMEOUT_S
    GREENTEA_SETUP(MBED_GREENTEA_TEST_EMAC_TIMEOUT_S, "default_auto");
#else
    GREENTEA_SETUP(1400, "default_auto");
#endif // #ifdef MBED_GREENTEA_TEST_EMAC_TIMEOUT_S
#endif // #if !MBED_CONF_APP_ECHO_SERVER

    return verbose_test_setup_handler(number_of_cases);
}

Case cases[] = {
    Case("EMAC initialize", test_emac_initialize),
    Case("EMAC broadcast", test_emac_broadcast),
    Case("EMAC unicast", test_emac_unicast),
    Case("EMAC unicast frame length", test_emac_unicast_frame_len),
    Case("EMAC unicast burst", test_emac_unicast_burst),
    Case("EMAC unicast long", test_emac_unicast_long),
    Case("EMAC multicast filter", test_emac_multicast_filter),
    Case("EMAC memory", test_emac_memory)
};

Specification specification(test_setup, cases);

int main()
{
    return !Harness::run(specification);
}

