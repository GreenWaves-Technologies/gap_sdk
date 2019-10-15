/*
 * Copyright (c) 2019, Arm Limited and affiliates.
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

#include "nsconfig.h"

#ifdef HAVE_WS
#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_SSL_TLS_C) && defined(MBEDTLS_X509_CRT_PARSE_C)
#define WS_MBEDTLS_SECURITY_ENABLED
#endif

#include <string.h>
#include <randLIB.h>
#include "ns_types.h"
#include "ns_list.h"
#include "ns_trace.h"
#include "nsdynmemLIB.h"
#include "common_functions.h"
#include "Security/protocols/sec_prot_certs.h"
#include "Security/protocols/tls_sec_prot/tls_sec_prot_lib.h"

#ifdef WS_MBEDTLS_SECURITY_ENABLED

#include "mbedtls/sha256.h"
#include "mbedtls/error.h"
#include "mbedtls/platform.h"
#include "mbedtls/ssl_cookie.h"
#include "mbedtls/entropy.h"
#include "mbedtls/entropy_poll.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/ssl_ciphersuites.h"
#include "mbedtls/debug.h"

#include "mbedtls/ssl_internal.h"

#define TRACE_GROUP "tlsl"

#define TLS_HANDSHAKE_TIMEOUT_MIN 25000
#define TLS_HANDSHAKE_TIMEOUT_MAX 201000

//#define TLS_SEC_PROT_LIB_TLS_DEBUG   // Enable mbed TLS debug traces

struct tls_security_s {
    mbedtls_ssl_config             conf;                 /**< mbed TLS SSL configuration */
    mbedtls_ssl_context            ssl;                  /**< mbed TLS SSL context */

    mbedtls_ctr_drbg_context       ctr_drbg;             /**< mbed TLS pseudo random number generator context */
    mbedtls_entropy_context        entropy;              /**< mbed TLS entropy context */

    mbedtls_x509_crt               cacert;               /**< CA certificate(s) */
    mbedtls_x509_crl               crl;                  /**< Certificate Revocation List */
    mbedtls_x509_crt               owncert;              /**< Own certificate(s) */
    mbedtls_pk_context             pkey;                 /**< Private key for own certificate */

    uint8_t                        client_random[32];    /**< Client random (from Client Hello) */
    uint8_t                        server_random[32];    /**< Server random (from Server Hello) */

    void                           *handle;              /**< Handle provided in callbacks (defined by library user) */
    tls_sec_prot_lib_send          *send;                /**< Send callback */
    tls_sec_prot_lib_receive       *receive;             /**< Receive callback */
    tls_sec_prot_lib_export_keys   *export_keys;         /**< Export keys callback */
    tls_sec_prot_lib_set_timer     *set_timer;           /**< Set timer callback */
    tls_sec_prot_lib_get_timer     *get_timer;           /**< Get timer callback */
};

static void tls_sec_prot_lib_ssl_set_timer(void *ctx, uint32_t int_ms, uint32_t fin_ms);
static int tls_sec_prot_lib_ssl_get_timer(void *ctx);
static int tls_sec_lib_entropy_poll(void *data, unsigned char *output, size_t len, size_t *olen);
static int tls_sec_prot_lib_ssl_send(void *ctx, const unsigned char *buf, size_t len);
static int tls_sec_prot_lib_ssl_recv(void *ctx, unsigned char *buf, size_t len);
static int tls_sec_prot_lib_ssl_export_keys(void *ctx, const unsigned char *ms,
                                            const unsigned char *kb, size_t maclen, size_t keylen, size_t ivlen);
static void tls_sec_prot_lib_random_extract(tls_security_t *sec, const uint8_t *buf, uint16_t len);
#ifdef TLS_SEC_PROT_LIB_TLS_DEBUG
static void tls_sec_prot_lib_debug(void *ctx, int level, const char *file, int line, const char *string);
#endif

int8_t tls_sec_prot_lib_init(tls_security_t *sec)
{
    const char *pers = "ws_tls";

    mbedtls_ssl_init(&sec->ssl);
    mbedtls_ssl_config_init(&sec->conf);
    mbedtls_ctr_drbg_init(&sec->ctr_drbg);
    mbedtls_entropy_init(&sec->entropy);

    mbedtls_x509_crt_init(&sec->cacert);
    mbedtls_x509_crl_init(&sec->crl);
    mbedtls_x509_crt_init(&sec->owncert);
    mbedtls_pk_init(&sec->pkey);

    if (mbedtls_entropy_add_source(&sec->entropy, tls_sec_lib_entropy_poll, NULL,
                                   128, MBEDTLS_ENTROPY_SOURCE_WEAK) < 0) {
        return -1;
    }

    if ((mbedtls_ctr_drbg_seed(&sec->ctr_drbg, mbedtls_entropy_func, &sec->entropy,
                               (const unsigned char *) pers, strlen(pers))) != 0) {
        return -1;
    }

    return 0;
}

uint16_t tls_sec_prot_lib_size(void)
{
    return sizeof(tls_security_t);
}

void tls_sec_prot_lib_set_cb_register(tls_security_t *sec, void *handle,
                                      tls_sec_prot_lib_send *send, tls_sec_prot_lib_receive *receive,
                                      tls_sec_prot_lib_export_keys *export_keys, tls_sec_prot_lib_set_timer *set_timer,
                                      tls_sec_prot_lib_get_timer *get_timer)
{
    if (!sec) {
        return;
    }

    sec->handle = handle;
    sec->send = send;
    sec->receive = receive;
    sec->export_keys = export_keys;
    sec->set_timer = set_timer;
    sec->get_timer = get_timer;
}

void tls_sec_prot_lib_free(tls_security_t *sec)
{
    mbedtls_x509_crt_free(&sec->cacert);
    mbedtls_x509_crl_free(&sec->crl);
    mbedtls_x509_crt_free(&sec->owncert);
    mbedtls_pk_free(&sec->pkey);
    mbedtls_entropy_free(&sec->entropy);
    mbedtls_ctr_drbg_free(&sec->ctr_drbg);
    mbedtls_ssl_config_free(&sec->conf);
    mbedtls_ssl_free(&sec->ssl);
}

static int tls_sec_prot_lib_configure_certificates(tls_security_t *sec, const sec_prot_certs_t *certs)
{
    if (!certs->own_cert_chain.cert[0]) {
        return -1;
    }

    // Parse own certificate chain
    uint8_t index = 0;
    while (true) {
        uint16_t cert_len;
        uint8_t *cert = sec_prot_certs_cert_get(&certs->own_cert_chain, index, &cert_len);
        if (!cert) {
            if (index == 0) {
                return -1;
            }
            break;
        }
        if (mbedtls_x509_crt_parse(&sec->owncert, cert, cert_len) < 0) {
            return -1;
        }
        index++;
    }

    // Parse private key
    uint8_t key_len;
    uint8_t *key = sec_prot_certs_priv_key_get(&certs->own_cert_chain, &key_len);
    if (!key) {
        return -1;
    }

    if (mbedtls_pk_parse_key(&sec->pkey, key, key_len, NULL, 0) < 0) {
        return -1;
    }

    // Configure own certificate chain and private key
    if (mbedtls_ssl_conf_own_cert(&sec->conf, &sec->owncert, &sec->pkey) != 0) {
        return -1;
    }

    // Parse trusted certificate chains
    ns_list_foreach(cert_chain_entry_t, entry, &certs->trusted_cert_chain_list) {
        index = 0;
        while (true) {
            uint16_t cert_len;
            uint8_t *cert = sec_prot_certs_cert_get(entry, index, &cert_len);
            if (!cert) {
                if (index == 0) {
                    return -1;
                }
                break;
            }
            if (mbedtls_x509_crt_parse(&sec->cacert, cert, cert_len) < 0) {
                return -1;
            }
            index++;
        }
    }

    // Parse certificate revocation lists
    ns_list_foreach(cert_revocat_list_entry_t, entry, &certs->cert_revocat_lists) {
        uint16_t crl_len;
        uint8_t *crl = sec_prot_certs_revocat_list_get(entry, &crl_len);
        if (!crl) {
            break;
        }
        if (mbedtls_x509_crl_parse(&sec->crl, crl, crl_len) < 0) {
            return -1;
        }
    }

    // Configure trusted certificates and certificate revocation lists
    mbedtls_ssl_conf_ca_chain(&sec->conf, &sec->cacert, &sec->crl);

    // Certificate verify required on both client and server
    mbedtls_ssl_conf_authmode(&sec->conf, MBEDTLS_SSL_VERIFY_REQUIRED);

    return 0;
}

int8_t tls_sec_prot_lib_connect(tls_security_t *sec, bool is_server, const sec_prot_certs_t *certs)
{
    if (!sec) {
        return -1;
    }

    int endpoint = MBEDTLS_SSL_IS_CLIENT;
    if (is_server) {
        endpoint = MBEDTLS_SSL_IS_SERVER;
    }

    if ((mbedtls_ssl_config_defaults(&sec->conf, endpoint, MBEDTLS_SSL_TRANSPORT_STREAM, 0)) != 0) {
        return -1;
    }

    // Configure random number generator
    mbedtls_ssl_conf_rng(&sec->conf, mbedtls_ctr_drbg_random, &sec->ctr_drbg);

#ifdef MBEDTLS_ECP_RESTARTABLE
    // Set ECC calculation maximum operations (affects only client)
    mbedtls_ecp_set_max_ops(ECC_CALCULATION_MAX_OPS);
#endif

    if ((mbedtls_ssl_setup(&sec->ssl, &sec->conf)) != 0) {
        return -1;
    }

    // Set calbacks
    mbedtls_ssl_set_bio(&sec->ssl, sec, tls_sec_prot_lib_ssl_send, tls_sec_prot_lib_ssl_recv, NULL);
    mbedtls_ssl_set_timer_cb(&sec->ssl, sec, tls_sec_prot_lib_ssl_set_timer, tls_sec_prot_lib_ssl_get_timer);

    // Configure certificates, keys and certificate revocation list
    if (tls_sec_prot_lib_configure_certificates(sec, certs) != 0) {
        tr_debug("security credential configure failed");
        return -1;
    }


    // Configure ciphersuites
    static const int sec_suites[] = {
        MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8,
        0,
        0,
        0
    };
    mbedtls_ssl_conf_ciphersuites(&sec->conf, sec_suites);

#ifdef TLS_SEC_PROT_LIB_TLS_DEBUG
    mbedtls_ssl_conf_dbg(&sec->conf, tls_sec_prot_lib_debug, sec);
    mbedtls_debug_set_threshold(5);
#endif

    // Export keys callback
    mbedtls_ssl_conf_export_keys_cb(&sec->conf, tls_sec_prot_lib_ssl_export_keys, sec);

    mbedtls_ssl_conf_min_version(&sec->conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MAJOR_VERSION_3);
    mbedtls_ssl_conf_max_version(&sec->conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MAJOR_VERSION_3);

    return 0;
}

#ifdef TLS_SEC_PROT_LIB_TLS_DEBUG
static void tls_sec_prot_lib_debug(void *ctx, int level, const char *file, int line, const char *string)
{
    (void) ctx;
    tr_debug("%i %s %i %s", level, file, line, string);
}
#endif

int8_t tls_sec_prot_lib_process(tls_security_t *sec)
{
    int ret = -1;

    while (ret != MBEDTLS_ERR_SSL_WANT_READ) {
        ret = mbedtls_ssl_handshake_step(&sec->ssl);

#ifdef MBEDTLS_ECP_RESTARTABLE
        if (ret == MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS /* || ret == MBEDTLS_ERR_SSL_ASYNC_IN_PROGRESS */) {
            return TLS_SEC_PROT_LIB_CALCULATING;
        }
#endif

        if (ret && (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)) {
            return TLS_SEC_PROT_LIB_ERROR;
        }

        if (sec->ssl.state == MBEDTLS_SSL_HANDSHAKE_OVER) {
            return TLS_SEC_PROT_LIB_HANDSHAKE_OVER;
        }
    }

    return TLS_SEC_PROT_LIB_CONTINUE;
}

static void tls_sec_prot_lib_ssl_set_timer(void *ctx, uint32_t int_ms, uint32_t fin_ms)
{
    tls_security_t *sec = (tls_security_t *)ctx;
    sec->set_timer(sec->handle, int_ms, fin_ms);
}

static int tls_sec_prot_lib_ssl_get_timer(void *ctx)
{
    tls_security_t *sec = (tls_security_t *)ctx;
    return sec->get_timer(sec->handle);
}

static int tls_sec_prot_lib_ssl_send(void *ctx, const unsigned char *buf, size_t len)
{
    tls_security_t *sec = (tls_security_t *)ctx;

    tls_sec_prot_lib_random_extract(sec, buf, len);

    return sec->send(sec->handle, buf, len);
}

static int tls_sec_prot_lib_ssl_recv(void *ctx, unsigned char *buf, size_t len)
{
    tls_security_t *sec = (tls_security_t *)ctx;
    int16_t ret = sec->receive(sec->handle, buf, len);

    if (ret == TLS_SEC_PROT_LIB_NO_DATA) {
        return MBEDTLS_ERR_SSL_WANT_READ;
    }

    tls_sec_prot_lib_random_extract(sec, buf, len);

    return ret;
}

static void tls_sec_prot_lib_random_extract(tls_security_t *sec, const uint8_t *buf, uint16_t len)
{
    static uint8_t *random_ptr = NULL;
    static uint8_t step = 0;

    if (step == 0) {
        if (*buf++ != 22 && len < 5) {
            return;
        }

        buf++; // version
        buf++;

        buf++; // length
        buf++;

        step++;

        if (len < 6) {
            return;
        }
    }

    if (step == 1) {
        if (*buf == 0x01) { // Client hello
            random_ptr = sec->client_random;
        } else if (*buf == 0x02) { // Server hello
            random_ptr = sec->server_random;
        } else {
            return;
        }
        buf++;

        buf++; // length
        buf++;
        buf++;

        buf++; // version
        buf++;

        memcpy(random_ptr, buf, 32);

        step = 0;
    }
}

static int tls_sec_prot_lib_ssl_export_keys(void *ctx, const unsigned char *ms,
                                            const unsigned char *kb, size_t maclen,
                                            size_t keylen, size_t ivlen)
{
    (void) kb;
    (void) maclen;
    (void) keylen;
    (void) ivlen;

    tls_security_t *sec = (tls_security_t *)ctx;

    uint8_t eap_tls_key_material[128];
    uint8_t random[64];
    memcpy(random, sec->client_random, 32);
    memcpy(&random[32], sec->server_random, 32);

    sec->ssl.handshake->tls_prf(ms, 48, "client EAP encryption",
                                random, 64, eap_tls_key_material, 128);

    sec->export_keys(sec->handle, ms, eap_tls_key_material);
    return 0;
}

static int tls_sec_lib_entropy_poll(void *ctx, unsigned char *output, size_t len, size_t *olen)
{
    (void)ctx;

    char *c = (char *)ns_dyn_mem_temporary_alloc(len);
    if (!c) {
        return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
    }
    memset(c, 0, len);
    for (uint16_t i = 0; i < len; i++) {
        *(c + i) = (char)randLIB_get_8bit();
    }
    memmove(output, c, len);
    *olen = len;

    ns_dyn_mem_free(c);
    return (0);
}

#else /* WS_MBEDTLS_SECURITY_ENABLED */

int8_t tls_sec_prot_lib_connect(tls_security_t *sec, bool is_server, const sec_prot_certs_t *certs)
{
    (void)sec;
    (void)is_server;
    (void)certs;
    return 0;
}

void tls_sec_prot_lib_free(tls_security_t *sec)
{
    (void)sec;
}

int8_t tls_sec_prot_lib_init(tls_security_t *sec)
{
    (void)sec;
    return 0;
}

int8_t tls_sec_prot_lib_process(tls_security_t *sec)
{
    (void)sec;
    return 0;
}

void tls_sec_prot_lib_set_cb_register(tls_security_t *sec, void *handle,
                                      tls_sec_prot_lib_send *send, tls_sec_prot_lib_receive *receive,
                                      tls_sec_prot_lib_export_keys *export_keys, tls_sec_prot_lib_set_timer *set_timer,
                                      tls_sec_prot_lib_get_timer *get_timer)
{
    (void)sec;
    (void)handle;
    (void)send;
    (void)receive;
    (void)export_keys;
    (void)set_timer;
    (void)get_timer;
}

uint16_t tls_sec_prot_lib_size(void)
{
    return 0;
}
#endif /* WS_MBEDTLS_SECURITY_ENABLED */
#endif /* HAVE_WS */

