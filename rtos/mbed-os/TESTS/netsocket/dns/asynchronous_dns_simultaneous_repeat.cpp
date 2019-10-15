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
#include "dns_tests.h"

using namespace utest::v1;

namespace {
int result_ok;
int result_no_mem;
int result_dns_failure;
int result_exp_timeout;
}

void ASYNCHRONOUS_DNS_SIMULTANEOUS_REPEAT()
{

    for (int i = 0; i < 10; i++) {
        do_asynchronous_gethostbyname(dns_test_hosts, MBED_CONF_APP_DNS_SIMULT_QUERIES + 1, &result_ok, &result_no_mem,
                                      &result_dns_failure, &result_exp_timeout);

        // For 1st round can fail to no memory, on other rounds some of the addresses are found from cache
        TEST_ASSERT(result_ok == MBED_CONF_APP_DNS_SIMULT_QUERIES || result_ok == MBED_CONF_APP_DNS_SIMULT_QUERIES + 1);
        TEST_ASSERT(result_no_mem == 1 || result_no_mem == 0); // last query fails for 1st round to no memory as expected
        TEST_ASSERT_EQUAL(0, result_dns_failure);
        TEST_ASSERT_EQUAL(0, result_exp_timeout);
    }
}
