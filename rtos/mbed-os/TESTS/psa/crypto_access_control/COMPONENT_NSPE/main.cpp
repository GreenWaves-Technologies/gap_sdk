/*
 * Copyright (c) 2019, Arm Limited and affiliates
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#if ((!defined(TARGET_PSA)) || (!defined(MBEDTLS_PSA_CRYPTO_C)) || (!defined(COMPONENT_PSA_SRV_IPC)))
#error [NOT_SUPPORTED] These tests can run only on SPM-enabled targets and where Mbed Crypto is ON - skipping.
#endif

#include <stdio.h>
#include "mbed.h"
#include "greentea-client/test_env.h"
#include "unity.h"
#include "utest.h"
#include "psa/crypto.h"
#include "entropy.h"
#include "entropy_poll.h"
#include "test_partition_proxy.h"
#include "psa/lifecycle.h"

using namespace utest::v1;

#if defined(MBEDTLS_ENTROPY_NV_SEED) || defined(COMPONENT_PSA_SRV_IPC)

#if !defined(MAX)
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#define MBEDTLS_PSA_INJECT_ENTROPY_MIN_SIZE \
            MAX(MBEDTLS_ENTROPY_MIN_PLATFORM, MBEDTLS_ENTROPY_BLOCK_SIZE)

void inject_entropy()
{
    uint8_t seed[MBEDTLS_PSA_INJECT_ENTROPY_MIN_SIZE] = { 0 };
    for (int i = 0; i < MBEDTLS_PSA_INJECT_ENTROPY_MIN_SIZE; ++i) {
        seed[i] = i;
    }
    mbedtls_psa_inject_entropy(seed, MBEDTLS_PSA_INJECT_ENTROPY_MIN_SIZE);
}
#endif // defined(MBEDTLS_ENTROPY_NV_SEED) || defined(COMPONENT_PSA_SRV_IPC)

static psa_status_t create_and_generate_key_via_test_partition(psa_key_id_t key_id, psa_key_type_t key_type,
                                                               psa_algorithm_t key_alg, psa_key_usage_t key_usage,
                                                               size_t key_bits, psa_key_handle_t *key_handle,
                                                               unsigned char close_key)
{
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_create_persistent_key(key_id, key_handle));
    TEST_ASSERT_NOT_EQUAL(0, *key_handle);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_set_key_policy(*key_handle, key_usage, key_alg));
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_generate_key(*key_handle, key_type, key_bits));
    if (close_key) {
        TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_close_key(*key_handle));
    }
    return (PSA_SUCCESS);
}

void test_open_other_partition_key(void)
{
    static const psa_key_id_t key_id = 999;
    static const psa_key_type_t key_type = PSA_KEY_TYPE_AES;
    static const psa_key_usage_t key_usage = PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT;
    static const psa_algorithm_t key_alg = PSA_ALG_CBC_NO_PADDING;
    static const size_t key_bits = 128;
    psa_key_handle_t key_handle = 0;

    /* via test partition - create a key, set key policy, generate key material and close */
    TEST_ASSERT_EQUAL(PSA_SUCCESS, create_and_generate_key_via_test_partition(key_id, key_type, key_alg, key_usage,
                                                                              key_bits, &key_handle, 1));

    /* via test partition - reopen the key created by the test partition */
    key_handle = 0;
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_open_persistent_key(key_id, &key_handle));
    TEST_ASSERT_NOT_EQUAL(0, key_handle);

    /* via test partition - close the key created by the test partition */
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_close_key(key_handle));

    /* try to open the key created by the test partition */
    TEST_ASSERT_EQUAL(PSA_ERROR_DOES_NOT_EXIST, psa_open_key(PSA_KEY_LIFETIME_PERSISTENT, key_id, &key_handle));

    /* via test partition - reopen the key created by the test partition and keep it open */
    key_handle = 0;
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_open_persistent_key(key_id, &key_handle));
    TEST_ASSERT_NOT_EQUAL(0, key_handle);

    /* via test partition - destroy the key created by the test partition */
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_destroy_key(key_handle));
}

void test_create_key_same_id_different_partitions(void)
{
    static const psa_key_id_t key_id = 999;
    static const psa_key_type_t key_type = PSA_KEY_TYPE_AES;
    static const psa_key_usage_t key_usage_remote = PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT,
                                 key_usage_local = PSA_KEY_USAGE_SIGN | PSA_KEY_USAGE_VERIFY;
    static const psa_algorithm_t key_alg = PSA_ALG_CBC_NO_PADDING;
    static const size_t key_bits_remote = 128,
                        key_bits_local = 256;
    psa_key_handle_t key_handle_remote = 0,
                     key_handle_local = 0;
    psa_key_type_t got_key_type_remote = 0,
                   got_key_type_local = 0;
    size_t got_key_bits_remote = 0,
           got_key_bits_local = 0;
    psa_key_usage_t got_key_usage_remote = 0;
    psa_algorithm_t got_key_alg_remote = 0;
    psa_key_policy_t policy = PSA_KEY_POLICY_INIT;

    /* via test partition - create a key, set key policy, generate key material and close */
    TEST_ASSERT_EQUAL(PSA_SUCCESS, create_and_generate_key_via_test_partition(key_id, key_type, key_alg,
                                                                              key_usage_remote, key_bits_remote,
                                                                              &key_handle_remote, 1));

    /* create a key, set key policy, generate key material and close from current partition (i.e. NSPE) */
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_create_key(PSA_KEY_LIFETIME_PERSISTENT, key_id, &key_handle_local));
    TEST_ASSERT_NOT_EQUAL(0, key_handle_local);
    psa_key_policy_set_usage(&policy, key_usage_local, key_alg);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_set_key_policy(key_handle_local, &policy));
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_generate_key(key_handle_local, key_type, key_bits_local, NULL, 0));
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_close_key(key_handle_local));

    /* via test partition - reopen the key created by the test partition */
    key_handle_remote = 0;
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_open_persistent_key(key_id, &key_handle_remote));
    TEST_ASSERT_NOT_EQUAL(0, key_handle_remote);

    /* reopen the key created from the current partition (NSPE) */
    key_handle_local = 0;
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_open_key(PSA_KEY_LIFETIME_PERSISTENT, key_id, &key_handle_local));
    TEST_ASSERT_NOT_EQUAL(0, key_handle_local);

    /* via test partition - get key info for the key created by the test partition */
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_get_key_information(key_handle_remote,
                                                                             &got_key_type_remote,
                                                                             &got_key_bits_remote));
    TEST_ASSERT_EQUAL(key_type, got_key_type_remote);
    TEST_ASSERT_EQUAL(key_bits_remote, got_key_bits_remote);

    /* via test partition - get key policy for key created by the test partition */
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_get_key_policy(key_handle_remote,
                                                                        &got_key_usage_remote,
                                                                        &got_key_alg_remote));
    TEST_ASSERT_EQUAL(key_usage_remote, got_key_usage_remote);
    TEST_ASSERT_EQUAL(key_alg, got_key_alg_remote);

    /* get key info for key created by the current partition (NSPE) */
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_get_key_information(key_handle_local, &got_key_type_local, &got_key_bits_local));
    TEST_ASSERT_EQUAL(key_type, got_key_type_local);
    TEST_ASSERT_EQUAL(key_bits_local, got_key_bits_local);

    /* get key policy for key created by the current partition (NSPE) */
    policy = psa_key_policy_init();
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_get_key_policy(key_handle_local, &policy));
    TEST_ASSERT_EQUAL(key_usage_local, policy.usage);
    TEST_ASSERT_EQUAL(key_alg, policy.alg);

    /* via test partition - destroy the key created by the test partition */
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_destroy_key(key_handle_remote));

    /* destroy the key created by the current partition (NSPE) */
    TEST_ASSERT_EQUAL(PSA_SUCCESS, psa_destroy_key(key_handle_local));
}

void test_use_other_partition_key_manage_key(void)
{
    static const psa_key_id_t key_id = 999;
    static const psa_key_type_t key_type = PSA_KEY_TYPE_AES;
    static const psa_algorithm_t key_alg = PSA_ALG_CBC_NO_PADDING;
    static const psa_key_usage_t key_usage = PSA_KEY_USAGE_EXPORT;
    static const size_t key_bits = 128;
    static const unsigned char key_data[] = {
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
    };
    psa_key_handle_t key_handle = 0;
    psa_key_policy_t policy = PSA_KEY_POLICY_INIT;
    unsigned char output[sizeof(key_data)] = { 0 };
    size_t len, got_key_bits;
    psa_key_type_t got_key_type;
    psa_key_lifetime_t got_lifetime;

    /* via test partition - create a key without generating any key material */
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_create_persistent_key(key_id, &key_handle));
    TEST_ASSERT_NOT_EQUAL(0, key_handle);

    /* try to set the key policy for the key that was created by the test partition */
    psa_key_policy_set_usage(&policy, key_usage, key_alg);
    TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_HANDLE, psa_set_key_policy(key_handle, &policy));

    /* via test partition - set key policy */
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_set_key_policy(key_handle, key_usage, key_alg));

    /* try to generate key data for the key that was created by the test partition */
    TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_HANDLE, psa_generate_key(key_handle, key_type, key_bits, NULL, 0));

    /* via test partition - generate key material and close the key */
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_generate_key(key_handle, key_type, key_bits));
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_close_key(key_handle));

    /* via test partition - reopen the key created by the test partition and keep it open */
    key_handle = 0;
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_open_persistent_key(key_id, &key_handle));
    TEST_ASSERT_NOT_EQUAL(0, key_handle);

    /* try to work with the handle created for a key created by the test partition */
    got_key_type = 0;
    got_key_bits = 0;
    got_lifetime = 0;
    policy = psa_key_policy_init();
    TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_HANDLE, psa_get_key_policy(key_handle, &policy));
    TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_HANDLE, psa_get_key_lifetime(key_handle, &got_lifetime));
    TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_HANDLE, psa_close_key(key_handle));
    TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_HANDLE, psa_destroy_key(key_handle));
    TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_HANDLE, psa_get_key_information(key_handle, &got_key_type, &got_key_bits));
    TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_HANDLE, psa_export_key(key_handle, output, sizeof(output), &len));
    TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_HANDLE, psa_export_public_key(key_handle, output, sizeof(output), &len));

    /* via test partition - destroy the key created by the test partition */
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_destroy_key(key_handle));

    /* via test partition - create a key, set key policy but no key material */
    key_handle = 0;
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_create_persistent_key(key_id, &key_handle));
    TEST_ASSERT_NOT_EQUAL(0, key_handle);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_set_key_policy(key_handle, key_usage, key_alg));

    /* try to import key data into the key that was created by the test partition */
    TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_HANDLE, psa_import_key(key_handle, key_type,
                                                               key_data, sizeof(key_data)));

    /* via test partition - import key data for the key created by the test partition */
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_import_key(key_handle, key_type, key_data, sizeof(key_data)));

    /* via test partition - destroy the key created by the test partition */
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_destroy_key(key_handle));
}

void test_use_other_partition_key_mac(void)
{
    static const psa_key_id_t key_id = 999;
    static const psa_key_type_t key_type = PSA_KEY_TYPE_AES;
    static const psa_algorithm_t key_alg = PSA_ALG_CBC_NO_PADDING;
    static const psa_key_usage_t key_usage = PSA_KEY_USAGE_SIGN | PSA_KEY_USAGE_VERIFY;
    static const size_t key_bits = 128;
    psa_key_handle_t key_handle = 0;
    psa_mac_operation_t operation;

    /* via test partition - create a key, set key policy, generate key material and close */
    TEST_ASSERT_EQUAL(PSA_SUCCESS, create_and_generate_key_via_test_partition(key_id, key_type, key_alg, key_usage,
                                                                              key_bits, &key_handle, 1));

    /* via test partition - reopen the key created by the test partition */
    key_handle = 0;
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_open_persistent_key(key_id, &key_handle));
    TEST_ASSERT_NOT_EQUAL(0, key_handle);

    /* try to setup mac sign operation using the key that was created by the test partition */
    operation = psa_mac_operation_init();
    TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_HANDLE, psa_mac_sign_setup(&operation, key_handle, key_alg));

    /* try to setup mac verify operation using the key that was created by the test partition */
    operation = psa_mac_operation_init();
    TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_HANDLE, psa_mac_verify_setup(&operation, key_handle, key_alg));

    /* via test partition - destroy the key created by the test partition */
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_destroy_key(key_handle));
}

void test_use_other_partition_key_cipher(void)
{
    static const psa_key_id_t key_id = 999;
    static const psa_key_type_t key_type = PSA_KEY_TYPE_AES;
    static const psa_algorithm_t key_alg = PSA_ALG_CBC_NO_PADDING;
    static const psa_key_usage_t key_usage = PSA_KEY_USAGE_SIGN | PSA_KEY_USAGE_VERIFY;
    static const size_t key_bits = 128;
    psa_key_handle_t key_handle = 0;
    psa_cipher_operation_t operation;

    /* via test partition - create a key, set key policy, generate key material and close */
    TEST_ASSERT_EQUAL(PSA_SUCCESS, create_and_generate_key_via_test_partition(key_id, key_type, key_alg, key_usage,
                                                                              key_bits, &key_handle, 1));

    /* via test partition - reopen the key created by the test partition */
    key_handle = 0;
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_open_persistent_key(key_id, &key_handle));
    TEST_ASSERT_NOT_EQUAL(0, key_handle);

    /* try to setup cipher encrypt sign operation using the key that was created by the test partition */
    operation = psa_cipher_operation_init();
    TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_HANDLE, psa_cipher_encrypt_setup(&operation, key_handle, key_alg));

    /* try to setup cipher decrypt operation using the key that was created by the test partition */
    operation = psa_cipher_operation_init();
    TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_HANDLE, psa_cipher_decrypt_setup(&operation, key_handle, key_alg));

    /* via test partition - destroy the key created by the test partition */
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_destroy_key(key_handle));
}

void test_use_other_partition_key_aead(void)
{
    static const psa_key_id_t key_id = 999;
    static const psa_key_type_t key_type = PSA_KEY_TYPE_AES;
    static const psa_algorithm_t key_alg = PSA_ALG_GCM;
    static const psa_key_usage_t key_usage = PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT;
    static const size_t key_bits = 128;
    static const unsigned char nonce[16] = { 0 };
    unsigned char plain_text[] = "encrypt me!";
    unsigned char cipher_text[PSA_AEAD_ENCRYPT_OUTPUT_SIZE(key_alg, sizeof(plain_text))] = { 0 };
    psa_key_handle_t key_handle = 0;
    size_t len;

    /* via test partition - create a key, set key policy, generate key material and close */
    TEST_ASSERT_EQUAL(PSA_SUCCESS, create_and_generate_key_via_test_partition(key_id, key_type, key_alg, key_usage,
                                                                              key_bits, &key_handle, 1));

    /* via test partition - reopen the key created by the test partition */
    key_handle = 0;
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_open_persistent_key(key_id, &key_handle));
    TEST_ASSERT_NOT_EQUAL(0, key_handle);

    /* try to aead encrypt using the key that was created by the test partition */
    TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_HANDLE, psa_aead_encrypt(key_handle, key_alg, nonce, sizeof(nonce), NULL, 0,
                                                                 plain_text, sizeof(plain_text),
                                                                 cipher_text, sizeof(cipher_text), &len));

    /* try to aead decrypt using the key that was created by the test partition */
    TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_HANDLE, psa_aead_decrypt(key_handle, key_alg, nonce, sizeof(nonce), NULL, 0,
                                                                 cipher_text, sizeof(cipher_text),
                                                                 plain_text, sizeof(plain_text), &len));

    /* via test partition - destroy the key created by the test partition */
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_destroy_key(key_handle));
}

void test_use_other_partition_key_asymmetric_sign_verify(void)
{
    static const psa_key_id_t key_id = 999;
    static const psa_key_type_t key_type = PSA_KEY_TYPE_ECC_KEYPAIR(PSA_ECC_CURVE_SECP256R1);
    static const psa_algorithm_t key_alg = PSA_ALG_ECDSA(PSA_ALG_SHA_256);
    static const psa_key_usage_t key_usage = PSA_KEY_USAGE_SIGN | PSA_KEY_USAGE_VERIFY;
    static const size_t key_bits = 256;
    static const unsigned char input[] = "hello world!";
    unsigned char signature[PSA_ECDSA_SIGNATURE_SIZE(key_bits)] = { 0 };
    psa_key_handle_t key_handle = 0;
    size_t len;

    /* via test partition - create a key, set key policy, generate key material and close */
    TEST_ASSERT_EQUAL(PSA_SUCCESS, create_and_generate_key_via_test_partition(key_id, key_type, key_alg, key_usage,
                                                                              key_bits, &key_handle, 1));

    /* via test partition - reopen the key created by the test partition */
    key_handle = 0;
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_open_persistent_key(key_id, &key_handle));
    TEST_ASSERT_NOT_EQUAL(0, key_handle);

    /* try to asymmetric sign using the key that was created by the test partition */
    TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_HANDLE, psa_asymmetric_sign(key_handle, key_alg, input, sizeof(input),
                                                                    signature, sizeof(signature), &len));

    /* try to asymmetric verify using the key that was created by the test partition */
    TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_HANDLE, psa_asymmetric_verify(key_handle, key_alg, input, sizeof(input),
                                                                      signature, sizeof(signature)));

    /* via test partition - destroy the key created by the test partition */
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_destroy_key(key_handle));
}

void test_use_other_partition_key_asymmetric_encrypt_decrypt(void)
{
    static const psa_key_id_t key_id = 999;
    static const psa_key_type_t key_type = PSA_KEY_TYPE_RSA_KEYPAIR;
    static const psa_algorithm_t key_alg = PSA_ALG_RSA_PKCS1V15_CRYPT;
    static const psa_key_usage_t key_usage = PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT;
    static const unsigned char input[] = "encrypt me!";
    static const unsigned char key_data[] = {
        0x30, 0x82, 0x01, 0x3b, 0x02, 0x01, 0x00, 0x02, 0x41, 0x00, 0xee, 0x2b,
        0x13, 0x1d, 0x6b, 0x18, 0x18, 0xa9, 0x4c, 0xa8, 0xe9, 0x1c, 0x42, 0x38,
        0x7e, 0xb1, 0x5a, 0x7c, 0x27, 0x1f, 0x57, 0xb8, 0x9e, 0x73, 0x36, 0xb1,
        0x44, 0xd4, 0x53, 0x5b, 0x16, 0xc8, 0x30, 0x97, 0xec, 0xde, 0xfb, 0xbb,
        0x92, 0xd1, 0xb5, 0x31, 0x3b, 0x5a, 0x37, 0x21, 0x4d, 0x0e, 0x8f, 0x25,
        0x92, 0x2d, 0xca, 0x77, 0x8b, 0x42, 0x4b, 0x25, 0x29, 0x5f, 0xc8, 0xa1,
        0xa7, 0x07, 0x02, 0x03, 0x01, 0x00, 0x01, 0x02, 0x41, 0x00, 0x97, 0x8a,
        0xc8, 0xea, 0xdb, 0x0d, 0xc6, 0x03, 0x53, 0x47, 0xd6, 0xab, 0xa8, 0x67,
        0x12, 0x15, 0xff, 0x21, 0x28, 0x33, 0x85, 0x39, 0x6f, 0x78, 0x97, 0xc0,
        0x4b, 0xaf, 0x5e, 0x2a, 0x83, 0x5f, 0x3b, 0x53, 0xef, 0x80, 0xa8, 0x2e,
        0xd3, 0x6a, 0xe6, 0x87, 0xa9, 0x25, 0x38, 0x0b, 0x55, 0xa0, 0xc7, 0x3e,
        0xb8, 0x56, 0x56, 0xe9, 0x89, 0xdc, 0xf0, 0xed, 0x7f, 0xb4, 0x88, 0x70,
        0x24, 0xe1, 0x02, 0x21, 0x00, 0xfd, 0xad, 0x8e, 0x1c, 0x68, 0x53, 0x56,
        0x3f, 0x8b, 0x92, 0x1d, 0x2d, 0x11, 0x24, 0x62, 0xae, 0x7d, 0x6b, 0x17,
        0x60, 0x82, 0xd2, 0xba, 0x43, 0xe8, 0x7e, 0x1a, 0x37, 0xfc, 0x1a, 0x8b,
        0x33, 0x02, 0x21, 0x00, 0xf0, 0x59, 0x2c, 0xf4, 0xc5, 0x5b, 0xa4, 0x43,
        0x07, 0xb1, 0x89, 0x81, 0xbc, 0xdb, 0xda, 0x37, 0x6c, 0x51, 0xe5, 0x90,
        0xff, 0xa5, 0x34, 0x5b, 0xa8, 0x66, 0xf6, 0x96, 0x2d, 0xca, 0x94, 0xdd,
        0x02, 0x20, 0x19, 0x95, 0xf1, 0xa9, 0x67, 0xd4, 0x4f, 0xf4, 0xa4, 0xcd,
        0x1d, 0xe8, 0x37, 0xbc, 0x65, 0xbf, 0x97, 0xa2, 0xbf, 0x7e, 0xda, 0x73,
        0x0a, 0x9a, 0x62, 0xce, 0xa5, 0x32, 0x54, 0x59, 0x11, 0x05, 0x02, 0x20,
        0x27, 0xf9, 0x6c, 0xf4, 0xb8, 0xee, 0x68, 0xff, 0x8d, 0x04, 0x06, 0x2e,
        0xc1, 0xce, 0x7f, 0x18, 0xc0, 0xb7, 0x4e, 0x4b, 0x33, 0x79, 0xb2, 0x9f,
        0x9b, 0xfe, 0xa3, 0xfc, 0x8e, 0x59, 0x27, 0x31, 0x02, 0x21, 0x00, 0xce,
        0xfa, 0x6d, 0x22, 0x04, 0x96, 0xb4, 0x3f, 0xeb, 0x83, 0x19, 0x42, 0x55,
        0xd8, 0xfb, 0x93, 0x0a, 0xfc, 0xf4, 0x6f, 0x36, 0x60, 0x6e, 0x3a, 0xa0,
        0xeb, 0x7a, 0x93, 0xad, 0x88, 0xc1, 0x0c
    };
    unsigned char encrypted[64] = { 0 };
    unsigned char decrypted[sizeof(input)] = { 0 };
    psa_key_handle_t key_handle = 0;
    size_t len;

    /* via test partition - create a key without generating any key material */
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_create_persistent_key(key_id, &key_handle));
    TEST_ASSERT_NOT_EQUAL(0, key_handle);

    /* via test partition - set key policy */
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_set_key_policy(key_handle, key_usage, key_alg));

    /* via test partition - import key data for the key created by the test partition */
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_import_key(key_handle, key_type, key_data, sizeof(key_data)));

    /* via test partition - close the key created by the test partition */
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_close_key(key_handle));

    /* via test partition - reopen the key created by the test partition and keep it open */
    key_handle = 0;
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_open_persistent_key(key_id, &key_handle));
    TEST_ASSERT_NOT_EQUAL(0, key_handle);

    /* try to asymmetric encrypt using the key that was created by the test partition */
    TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_HANDLE, psa_asymmetric_encrypt(key_handle, key_alg, input, sizeof(input),
                                                                       NULL, 0, encrypted, sizeof(encrypted), &len));

    /* try to asymmetric decrypt using the key that was created by the test partition */
    TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_HANDLE, psa_asymmetric_decrypt(key_handle, key_alg,
                                                                       encrypted, sizeof(encrypted), NULL, 0,
                                                                       decrypted, sizeof(decrypted), &len));

    /* via test partition - destroy the key created by the test partition */
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_destroy_key(key_handle));
}

void test_use_other_partition_key_derivation_setup(void)
{
    static const psa_key_id_t key_id = 999;
    static const psa_algorithm_t key_alg = PSA_ALG_HKDF(PSA_ALG_SHA_256);
    static const psa_key_usage_t key_usage = PSA_KEY_USAGE_DERIVE;
    static const psa_key_type_t key_type = PSA_KEY_TYPE_DERIVE;
    static const unsigned char key_data[] = {
        0x30, 0x82, 0x01, 0x3b, 0x02, 0x01, 0x00, 0x02, 0x41, 0x00, 0xee, 0x2b,
        0x13, 0x1d, 0x6b, 0x18, 0x18, 0xa9, 0x4c, 0xa8, 0xe9, 0x1c, 0x42, 0x38
    };
    static const unsigned char salt[] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
        0x0c
    };
    static const unsigned char label[] = {
        0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9
    };

    psa_key_handle_t key_handle = 0;
    psa_crypto_generator_t generator = psa_crypto_generator_init();
    size_t bits = 128;

    /* via test partition - create a key without generating any key material */
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_create_persistent_key(key_id, &key_handle));
    TEST_ASSERT_NOT_EQUAL(0, key_handle);

    /* via test partition - set key policy */
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_set_key_policy(key_handle, key_usage, key_alg));

    /* via test partition - import key data for the key created by the test partition */
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_import_key(key_handle, key_type, key_data, sizeof(key_data)));

    /* try to setup key derivation using the key that was created by the test partition */
    TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_HANDLE, psa_key_derivation(&generator, key_handle, key_alg,
                                                                   (unsigned char *)salt, sizeof(salt),
                                                                   (unsigned char *)label, sizeof(label),
                                                                   PSA_BITS_TO_BYTES(bits)));

    /* via test partition - destroy the key created by the test partition */
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_destroy_key(key_handle));
}

void test_use_other_partition_key_agreement_setup(void)
{
    static const psa_key_id_t key_id = 999;
    static const psa_algorithm_t key_alg = PSA_ALG_ECDH(PSA_ALG_SELECT_RAW);
    static const psa_key_usage_t key_usage = PSA_KEY_USAGE_DERIVE;
    static const psa_key_type_t key_type = PSA_KEY_TYPE_ECC_KEYPAIR(PSA_ECC_CURVE_SECP256R1);
    static const unsigned char key_data[] = {
        0xc8, 0x8f, 0x01, 0xf5, 0x10, 0xd9, 0xac, 0x3f, 0x70, 0xa2, 0x92, 0xda,
        0xa2, 0x31, 0x6d, 0xe5, 0x44, 0xe9, 0xaa, 0xb8, 0xaf, 0xe8, 0x40, 0x49,
        0xc6, 0x2a, 0x9c, 0x57, 0x86, 0x2d, 0x14, 0x33
    };
    static const unsigned char peer_key_data[] = {
        0x04, 0xd1, 0x2d, 0xfb, 0x52, 0x89, 0xc8, 0xd4, 0xf8, 0x12, 0x08, 0xb7,
        0x02, 0x70, 0x39, 0x8c, 0x34, 0x22, 0x96, 0x97, 0x0a, 0x0b, 0xcc, 0xb7,
        0x4c, 0x73, 0x6f, 0xc7, 0x55, 0x44, 0x94, 0xbf, 0x63, 0x56, 0xfb, 0xf3,
        0xca, 0x36, 0x6c, 0xc2, 0x3e, 0x81, 0x57, 0x85, 0x4c, 0x13, 0xc5, 0x8d,
        0x6a, 0xac, 0x23, 0xf0, 0x46, 0xad, 0xa3, 0x0f, 0x83, 0x53, 0xe7, 0x4f,
        0x33, 0x03, 0x98, 0x72, 0xab
    };

    psa_key_handle_t key_handle = 0;
    psa_crypto_generator_t generator = psa_crypto_generator_init();

    /* via test partition - create a key without generating any key material */
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_create_persistent_key(key_id, &key_handle));
    TEST_ASSERT_NOT_EQUAL(0, key_handle);

    /* via test partition - set key policy */
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_set_key_policy(key_handle, key_usage, key_alg));

    /* via test partition - import key data for the key created by the test partition */
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_import_key(key_handle, key_type, key_data, sizeof(key_data)));

    /* try to setup key agreement using the key that was created by the test partition */
    TEST_ASSERT_EQUAL(PSA_ERROR_INVALID_HANDLE, psa_key_agreement(&generator, key_handle,
                                                                  (unsigned char *)peer_key_data, sizeof(peer_key_data),
                                                                  key_alg));

    /* via test partition - destroy the key created by the test partition */
    TEST_ASSERT_EQUAL(PSA_SUCCESS, test_partition_crypto_destroy_key(key_handle));
}

utest::v1::status_t case_setup_handler(const Case *const source, const size_t index_of_case)
{
    psa_status_t status = mbed_psa_reboot_and_request_new_security_state(PSA_LIFECYCLE_ASSEMBLY_AND_TEST);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    status = psa_crypto_init();
#if defined(MBEDTLS_ENTROPY_NV_SEED) || defined(COMPONENT_PSA_SRV_IPC)
    if (status == PSA_ERROR_INSUFFICIENT_ENTROPY) {
        inject_entropy();
        status = psa_crypto_init();
    }
#endif
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    return greentea_case_setup_handler(source, index_of_case);
}

utest::v1::status_t case_teardown_handler(const Case *const source, const size_t passed,
                                          const size_t failed, const failure_t failure)
{
    psa_status_t status = mbed_psa_reboot_and_request_new_security_state(PSA_LIFECYCLE_ASSEMBLY_AND_TEST);
    TEST_ASSERT_EQUAL(PSA_SUCCESS, status);
    mbedtls_psa_crypto_free();
    return greentea_case_teardown_handler(source, passed, failed, failure);
}

utest::v1::status_t test_setup(const size_t number_of_cases)
{
    GREENTEA_SETUP(120, "default_auto");
    return verbose_test_setup_handler(number_of_cases);
}

Case cases[] = {
    Case("open other partition's key",
         case_setup_handler, test_open_other_partition_key, case_teardown_handler),
    Case("create key with same id different partitions",
         case_setup_handler, test_create_key_same_id_different_partitions, case_teardown_handler),
    Case("use other partition's key - key manage",
         case_setup_handler, test_use_other_partition_key_manage_key, case_teardown_handler),
    Case("use other partition's key - mac",
         case_setup_handler, test_use_other_partition_key_mac, case_teardown_handler),
    Case("use other partition's key - cipher",
         case_setup_handler, test_use_other_partition_key_cipher, case_teardown_handler),
    Case("use other partition's key - aead",
         case_setup_handler, test_use_other_partition_key_aead, case_teardown_handler),
    Case("use other partition's key - asymmetric sign verify",
         case_setup_handler, test_use_other_partition_key_asymmetric_sign_verify, case_teardown_handler),
    Case("use other partition's key - asymmetric encrypt decrypt",
         case_setup_handler, test_use_other_partition_key_asymmetric_encrypt_decrypt, case_teardown_handler),
    Case("use other partition's key - key derivation setup",
         case_setup_handler, test_use_other_partition_key_derivation_setup, case_teardown_handler),
    Case("use other partition's key - key agreement setup",
         case_setup_handler, test_use_other_partition_key_agreement_setup, case_teardown_handler),
};

Specification specification(test_setup, cases);

int main(void)
{
    return !Harness::run(specification);
}
