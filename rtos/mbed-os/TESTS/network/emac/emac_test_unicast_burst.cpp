/*
 * Copyright (c) 2018, ARM Limited, All Rights Reserved
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

#include "mbed.h"
#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"

#include "emac_tests.h"
#include "emac_util.h"
#include "emac_ctp.h"

using namespace utest::v1;

void test_emac_unicast_burst_cb(int opt)
{
    static bool send_request = true;
    static int no_response_cnt = 0;
    static int retries = 0;
    static int msg_len = 0;
    static int test_step = 0;

    // Timeout
    if (opt == TIMEOUT && send_request) {
        CTP_MSG_SEND(msg_len, emac_if_get_echo_server_addr(0), emac_if_get_own_addr(), emac_if_get_own_addr(), 0);
        send_request = false;
        no_response_cnt = 0;
    } else if (opt == TIMEOUT) {
        if (++no_response_cnt > 50) {
            if (++retries > 5) {
                printf("too many retries\r\n\r\n");
                SET_ERROR_FLAGS(TEST_FAILED);
                END_TEST_LOOP;
            } else {
                printf("retry count %i\r\n\r\n", retries);
                send_request = true;
            }
        }
    }

    // Echo response received
    if (opt == INPUT) {
        if (msg_len == ETH_MAX_LEN) {
            if (++test_step > 10) {
                END_TEST_LOOP;
            } else {
                msg_len = 0;
            }
        } else if (msg_len + 50 >= ETH_MAX_LEN) {
            msg_len = ETH_MAX_LEN;
        } else {
            msg_len += 50;
        }
        printf("message length %i\r\n\r\n", msg_len);
        retries = 0;
        send_request = true;
    }
}

void test_emac_unicast_burst()
{
    RESET_ALL_ERROR_FLAGS;
    SET_TRACE_LEVEL(TRACE_SEND | TRACE_SUCCESS | TRACE_FAILURE);

    if (ECHO_SERVER_ADDRESS_KNOWN) {
        START_TEST_LOOP(test_emac_unicast_burst_cb, 100);
    }

    PRINT_ERROR_FLAGS;
    TEST_ASSERT_FALSE(ERROR_FLAGS);
    RESET_OUTGOING_MSG_DATA;
}
