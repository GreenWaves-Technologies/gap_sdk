/*
 * Copyright (c) 2017, Arm Limited and affiliates.
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

#include "QUECTEL_BC95_CellularNetwork.h"

using namespace mbed;

QUECTEL_BC95_CellularNetwork::QUECTEL_BC95_CellularNetwork(ATHandler &atHandler) : AT_CellularNetwork(atHandler)
{
    _op_act = RAT_NB1;
}

QUECTEL_BC95_CellularNetwork::~QUECTEL_BC95_CellularNetwork()
{
}

nsapi_error_t QUECTEL_BC95_CellularNetwork::set_access_technology_impl(RadioAccessTechnology opRat)
{
    if (opRat != RAT_NB1) {
        // only rat that is supported by this modem
        _op_act = RAT_NB1;
        return NSAPI_ERROR_UNSUPPORTED;
    }

    return NSAPI_ERROR_OK;
}
