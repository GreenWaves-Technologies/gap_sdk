/*
 * Copyright (C) 2019 GreenWaves Technologies
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

/*
 * Created by Mathieu Barbe <mathieu.barbe@greenwaves-technologies.com>.
 * on 12/21/2019.
 */

#ifndef API_ERRNO_H
#define API_ERRNO_H

typedef enum {
    PI_OK = 0x00, /*!< indicating success (no error) */
    PI_FAIL = 0x01, /*!< Generic code indicating failure */

    PI_ERR_INVALID_ARG = 0x02, /*!< Invalid argument */
    PI_ERR_INVALID_STATE = 0x03, /*!< Invalid state */
    PI_ERR_INVALID_SIZE = 0x04, /*!< Invalid size */
    PI_ERR_NOT_FOUND = 0x05, /*!< Requested resource not found */
    PI_ERR_NOT_SUPPORTED = 0x06, /*!< Operation or feature not supported */
    PI_ERR_TIMEOUT = 0x07, /*!< Operation timed out */
    PI_ERR_INVALID_CRC = 0x08, /*!< CRC or checksum was invalid */
    PI_ERR_INVALID_VERSION = 0x09, /*!< Version was invalid */
    PI_ERR_INVALID_APP = 0x0A, /*!< App binary is not compliant with GAP. */
    PI_ERR_INVALID_MAGIC_CODE = 0x0B, /*!< Magic code does not match. */

    PI_ERR_I2C_NACK = 0x100, /*! I2C request ended with a NACK */

    PI_ERR_NO_MEM = 0x200, /*!< Generic out of memory */
    PI_ERR_L2_NO_MEM = 0x201, /*!< L2 out of memory */
} pi_err_t;

#endif //API_ERRNO_H
