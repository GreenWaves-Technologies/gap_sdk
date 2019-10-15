
/** \addtogroup frameworks */
/** @{*/
/*
 * Copyright (c) 2013-2016, ARM Limited, All Rights Reserved
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

#ifndef GREENTEA_CLIENT_TEST_ENV_H_
#define GREENTEA_CLIENT_TEST_ENV_H_

#ifdef __cplusplus
#define MBED_GREENTEA_CLIENT_VERSION_STRING "1.3.0"

#include <stdio.h>

/**
 *  Auxilary macros
 */
#ifndef NL
#define NL "\n"
#endif
#ifndef RCNL
#define RCNL "\r\n"
#endif

/**
 * Ensure compatibility with utest
 */
#define TEST_ENV_TESTCASE_COUNT     GREENTEA_TEST_ENV_TESTCASE_COUNT
#define TEST_ENV_TESTCASE_START     GREENTEA_TEST_ENV_TESTCASE_START
#define TEST_ENV_TESTCASE_FINISH    GREENTEA_TEST_ENV_TESTCASE_FINISH
#define TEST_ENV_TESTCASE_SUMMARY   GREENTEA_TEST_ENV_TESTCASE_SUMMARY

/**
 *  Default length for UUID buffers (used during the sync process)
 */
#define GREENTEA_UUID_LENGTH        48

/**
 *  Generic test suite transport protocol keys
 */
extern const char* GREENTEA_TEST_ENV_END;
extern const char* GREENTEA_TEST_ENV_EXIT;
extern const char* GREENTEA_TEST_ENV_SYNC;
extern const char* GREENTEA_TEST_ENV_TIMEOUT;
extern const char* GREENTEA_TEST_ENV_HOST_TEST_NAME;
extern const char* GREENTEA_TEST_ENV_HOST_TEST_VERSION;

/**
 *  Test suite success code strings
 */
extern const char* GREENTEA_TEST_ENV_SUCCESS;
extern const char* GREENTEA_TEST_ENV_FAILURE;

/**
 *  Test case transport protocol start/finish keys
 */
extern const char* GREENTEA_TEST_ENV_TESTCASE_NAME;
extern const char* GREENTEA_TEST_ENV_TESTCASE_COUNT;
extern const char* GREENTEA_TEST_ENV_TESTCASE_START;
extern const char* GREENTEA_TEST_ENV_TESTCASE_FINISH;
extern const char* GREENTEA_TEST_ENV_TESTCASE_SUMMARY;

/**
 *  Code Coverage (LCOV)  transport protocol keys
 */
extern const char* GREENTEA_TEST_ENV_LCOV_START;

/**
 *  Greentea-client related API for communication with host side
 */
void GREENTEA_SETUP_UUID(const int timeout, const char *host_test_name, char *buffer, size_t size);
void GREENTEA_TESTSUITE_RESULT(const int);
void GREENTEA_TESTCASE_START(const char *test_case_name);
void GREENTEA_TESTCASE_FINISH(const char *test_case_name, const size_t passes, const size_t failed);

/**
 *  Test suite result related notification API
 */
void greentea_send_kv(const char *, const int);
void greentea_send_kv(const char *, const int, const int);
void greentea_send_kv(const char *, const char *, const int);
void greentea_send_kv(const char *, const char *, const int, const int);

#ifdef MBED_CFG_DEBUG_OPTIONS_COVERAGE
/**
 *  Code Coverage API
 */
void greentea_notify_coverage_start(const char *path);
void greentea_notify_coverage_end();
#endif  // MBED_CFG_DEBUG_OPTIONS_COVERAGE

#endif  // __cplusplus

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  Greentea-client C API
 */
void GREENTEA_SETUP(const int timeout, const char * host_test);
void greentea_send_kv(const char * key, const char * val);
int greentea_parse_kv(char * key, char * val,
                        const int key_len, const int val_len);
int greentea_getc();

#ifdef __cplusplus
}
#endif

#endif  // GREENTEA_CLIENT_TEST_ENV_H_

/** @}*/
