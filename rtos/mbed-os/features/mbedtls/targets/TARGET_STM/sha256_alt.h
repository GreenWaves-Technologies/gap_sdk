/**
 * \file sha256_alt.h
 *
 * \brief SHA256 hw acceleration (hash function)
 *
 * Copyright (c) 2017, STMicroelectronics
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */
#ifndef MBEDTLS_SHA256_ALT_H
#define MBEDTLS_SHA256_ALT_H

#if defined (MBEDTLS_SHA256_ALT)

#include "cmsis.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ST_SHA256_BLOCK_SIZE ((size_t)  64) // HW handles 512 bits, ie 64 bytes
#define ST_SHA256_TIMEOUT    ((uint32_t) 3)
/**
 * \brief          SHA-256 context structure
 * \note     HAL_HASH_SHA256_Accumulate will accumulate 512 bits packets, unless it is the last call to the  function
 *           A ST_SHA256_BLOCK_SIZE bytes buffer is used to save values and handle the processing
 *           ST_SHA256_BLOCK_SIZE bytes per ST_SHA256_BLOCK_SIZE bytes
 *           If sha256_finish is called and sbuf_len>0, the remaining bytes are accumulated prior to the call to HAL_HASH_SHA256_Finish
 */
typedef struct {
    int is224;                  /*!< 0 => SHA-256, else SHA-224 */
    HASH_HandleTypeDef hhash_sha256;
    unsigned char sbuf[ST_SHA256_BLOCK_SIZE]; /*!< ST_SHA256_BLOCK_SIZE buffer to store values so that algorithm is called once the buffer is filled */
    unsigned char sbuf_len; /*!< number of bytes to be processed in sbuf */
    uint32_t ctx_save_cr;
    uint32_t ctx_save_str;
    uint32_t ctx_save_csr[38];
}
mbedtls_sha256_context;

/**
 * \brief          Initialize SHA-256 context
 *
 * \param ctx      SHA-256 context to be initialized
 */
void mbedtls_sha256_init(mbedtls_sha256_context *ctx);

/**
 * \brief          Clear SHA-256 context
 *
 * \param ctx      SHA-256 context to be cleared
 */
void mbedtls_sha256_free(mbedtls_sha256_context *ctx);

/**
 * \brief          Clone (the state of) a SHA-256 context
 *
 * \param dst      The destination context
 * \param src      The context to be cloned
 */
void mbedtls_sha256_clone(mbedtls_sha256_context *dst,
                          const mbedtls_sha256_context *src);

/**
 * \brief          SHA-256 context setup
 *
 * \param ctx      context to be initialized
 * \param is224    0 = use SHA256, 1 = use SHA224
 *
 * \returns        error code
 */
int mbedtls_sha256_starts_ret(mbedtls_sha256_context *ctx, int is224);

/**
 * \brief          SHA-256 process buffer
 *
 * \param ctx      SHA-256 context
 * \param input    buffer holding the  data
 * \param ilen     length of the input data
 *
 * \returns        error code
 */
int mbedtls_sha256_update_ret(mbedtls_sha256_context *ctx,
                              const unsigned char *input,
                              size_t ilen);

/**
 * \brief          SHA-256 final digest
 *
 * \param ctx      SHA-256 context
 * \param output   SHA-224/256 checksum result
 *
 * \returns        error code
 */
int mbedtls_sha256_finish_ret(mbedtls_sha256_context *ctx, unsigned char output[32]);

/* Internal use */
int mbedtls_internal_sha256_process(mbedtls_sha256_context *ctx, const unsigned char data[ST_SHA256_BLOCK_SIZE]);

#if !defined(MBEDTLS_DEPRECATED_REMOVED)
#if defined(MBEDTLS_DEPRECATED_WARNING)
#define MBEDTLS_DEPRECATED      __attribute__((deprecated))
#else
#define MBEDTLS_DEPRECATED
#endif
/**
 * \brief          This function starts a SHA-256 checksum calculation.
 *
 * \deprecated     Superseded by mbedtls_sha256_starts_ret() in 2.7.0.
 *
 * \param ctx      The SHA-256 context to initialize.
 * \param is224    Determines which function to use.
 *                 <ul><li>0: Use SHA-256.</li>
 *                 <li>1: Use SHA-224.</li></ul>
 */
MBEDTLS_DEPRECATED void mbedtls_sha256_starts(mbedtls_sha256_context *ctx,
                                              int is224);

/**
 * \brief          This function feeds an input buffer into an ongoing
 *                 SHA-256 checksum calculation.
 *
 * \deprecated     Superseded by mbedtls_sha256_update_ret() in 2.7.0.
 *
 * \param ctx      The SHA-256 context to initialize.
 * \param input    The buffer holding the data.
 * \param ilen     The length of the input data.
 */
MBEDTLS_DEPRECATED void mbedtls_sha256_update(mbedtls_sha256_context *ctx,
                                              const unsigned char *input,
                                              size_t ilen);

/**
 * \brief          This function finishes the SHA-256 operation, and writes
 *                 the result to the output buffer.
 *
 * \deprecated     Superseded by mbedtls_sha256_finish_ret() in 2.7.0.
 *
 * \param ctx      The SHA-256 context.
 * \param output   The SHA-224or SHA-256 checksum result.
 */
MBEDTLS_DEPRECATED void mbedtls_sha256_finish(mbedtls_sha256_context *ctx,
                                              unsigned char output[32]);

/**
 * \brief          This function processes a single data block within
 *                 the ongoing SHA-256 computation. This function is for
 *                 internal use only.
 *
 * \deprecated     Superseded by mbedtls_internal_sha256_process() in 2.7.0.
 *
 * \param ctx      The SHA-256 context.
 * \param data     The buffer holding one block of data.
 */
MBEDTLS_DEPRECATED void mbedtls_sha256_process(mbedtls_sha256_context *ctx,
                                               const unsigned char data[64]);

#undef MBEDTLS_DEPRECATED
#endif /* !MBEDTLS_DEPRECATED_REMOVED */

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_SHA256_ALT */

#endif /* sha256_alt.h */
