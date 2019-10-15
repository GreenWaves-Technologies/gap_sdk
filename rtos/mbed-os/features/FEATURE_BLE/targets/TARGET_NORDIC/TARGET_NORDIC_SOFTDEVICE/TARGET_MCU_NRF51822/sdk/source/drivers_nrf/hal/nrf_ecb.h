/*
 * Copyright (c) Nordic Semiconductor ASA
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 *   3. Neither the name of Nordic Semiconductor ASA nor the names of other
 *   contributors to this software may be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/**
 * @file
 * @brief ECB driver API.
 */

#ifndef NRF_ECB_H__
#define NRF_ECB_H__

/**
 * @defgroup nrf_ecb AES ECB encryption
 * @{
 * @ingroup nrf_drivers
 * @brief Driver for the AES Electronic Code Book (ECB) peripheral.
 *
 * To encrypt and decrypt data, the peripheral must first be powered on
 * using @ref nrf_ecb_init. Next, the key must be set using @ref nrf_ecb_set_key.
 */

#include <stdint.h>

/**
 * @brief Function for initializing and powering on the ECB peripheral.
 *
 * This function allocates memory for the ECBDATAPTR.
 * @retval true If initialization was successful.
 * @retval false If powering on failed.
 */
bool nrf_ecb_init(void);

/**
 * @brief Function for encrypting and decrypting 16-byte data using current key.
 *
 * This function avoids unnecessary copying of data if the parameters point to the 
 * correct locations in the ECB data structure.
 *
 * @param dst Result of encryption/decryption. 16 bytes will be written. 
 * @param src Source with 16-byte data to be encrypted/decrypted.
 *
 * @retval true  If the encryption operation completed.
 * @retval false If the encryption operation did not complete.
 */
bool nrf_ecb_crypt(uint8_t * dst, const uint8_t * src);

/**
 * @brief Function for setting the key to be used for encryption and decryption.
 *
 * @param key Pointer to the key. 16 bytes will be read.
 */
void nrf_ecb_set_key(const uint8_t * key);

#endif  // NRF_ECB_H__

/** @} */
