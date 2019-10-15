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

#include "SARA4_PPP_CellularNetwork.h"

using namespace mbed;

SARA4_PPP_CellularNetwork::SARA4_PPP_CellularNetwork(ATHandler &atHandler) : AT_CellularNetwork(atHandler)
{
}

SARA4_PPP_CellularNetwork::~SARA4_PPP_CellularNetwork()
{
}

nsapi_error_t SARA4_PPP_CellularNetwork::set_access_technology_impl(RadioAccessTechnology opRat)
{
    _op_act = RAT_CATM1;
    return NSAPI_ERROR_OK;
}
