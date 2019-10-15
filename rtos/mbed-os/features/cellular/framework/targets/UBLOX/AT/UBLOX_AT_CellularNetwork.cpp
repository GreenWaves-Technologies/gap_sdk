/*
 * Copyright (c) 2018, Arm Limited and affiliates.
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

#include "UBLOX_AT_CellularNetwork.h"

using namespace mbed;

UBLOX_AT_CellularNetwork::UBLOX_AT_CellularNetwork(ATHandler &atHandler) : AT_CellularNetwork(atHandler)
{
    _op_act = RAT_UNKNOWN;
}

UBLOX_AT_CellularNetwork::~UBLOX_AT_CellularNetwork()
{
    if (_connection_status_cb) {
        _connection_status_cb(NSAPI_EVENT_CONNECTION_STATUS_CHANGE, NSAPI_ERROR_CONNECTION_LOST);
    }
}

nsapi_error_t UBLOX_AT_CellularNetwork::set_access_technology_impl(RadioAccessTechnology opRat)
{
    nsapi_error_t ret = NSAPI_ERROR_OK;

    switch (opRat) {
#if defined(TARGET_UBLOX_C030_U201) || defined(TARGET_UBLOX_C027)
        case RAT_GSM:
        case RAT_GSM_COMPACT:
            break;
        case RAT_EGPRS:
            break;
#elif defined(TARGET_UBLOX_C030_U201)
        case RAT_UTRAN:
            break;
        case RAT_HSDPA:
            break;
        case RAT_HSUPA:
            break;
        case RAT_HSDPA_HSUPA:
            break;
#elif defined(TARGET_UBLOX_C030_R41XM)
        case RAT_CATM1:
            break;
#elif defined(TARGET_UBLOX_C030_R41XM) || defined(TARGET_UBLOX_C030_N211)
        case RAT_NB1:
            break;
#endif
        default: {
            _op_act = RAT_UNKNOWN;
            ret = NSAPI_ERROR_UNSUPPORTED;
        }
    }

    return (ret);
}
