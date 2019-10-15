/*
 * Copyright (c) 2019, ARM Limited, All Rights Reserved
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
#include "TLSSocket.h"
#include "greentea-client/test_env.h"
#include "unity/unity.h"
#include "utest.h"
#include "tls_tests.h"

using namespace utest::v1;

#if defined(MBEDTLS_SSL_CLI_C)

void TLSSOCKET_SEND_REPEAT()
{
    SKIP_IF_TCP_UNSUPPORTED();
    TLSSocket sock;
    tlssocket_connect_to_discard_srv(sock);

    int snd;
    Timer timer;
    static const char tx_buffer[] = {'h', 'e', 'l', 'l', 'o'};
    for (int i = 0; i < 100; i++) {
        snd = sock.send(tx_buffer, sizeof(tx_buffer));
        if (snd != sizeof(tx_buffer)) {
            TEST_FAIL();
            break;
        }
    }

    TEST_ASSERT_EQUAL(NSAPI_ERROR_OK, sock.close());
}

#endif // defined(MBEDTLS_SSL_CLI_C)
