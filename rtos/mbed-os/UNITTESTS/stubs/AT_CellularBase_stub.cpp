/*
 * Copyright (c) , Arm Limited and affiliates.
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

#include "nsapi_types.h"
#include "AT_CellularBase_stub.h"
#include "AT_CellularNetwork.h"
using namespace mbed;

ATHandler *AT_CellularBase_stub::handler_value = NULL;
ATHandler *AT_CellularBase_stub::handler_at_constructor_value = NULL;
device_err_t AT_CellularBase_stub::device_err_value;
bool AT_CellularBase_stub::supported_bool = true;

AT_CellularBase::AT_CellularBase(ATHandler &at) : _at(at)
{
    AT_CellularBase_stub::handler_at_constructor_value = &_at;
}

ATHandler &AT_CellularBase::get_at_handler()
{
    AT_CellularBase_stub::handler_value = &_at;
    return _at;
}

device_err_t AT_CellularBase::get_device_error() const
{
    return AT_CellularBase_stub::device_err_value;
}

intptr_t AT_CellularBase::get_property(CellularProperty key)
{
    if (key == PROPERTY_C_GREG) {
        return AT_CellularNetwork::RegistrationModeDisable;
    } else if (key == PROPERTY_C_REG || key == PROPERTY_C_EREG) {
        return AT_CellularNetwork::RegistrationModeEnable;
    } else if (key == PROPERTY_AT_CGAUTH) {
        return true;
    } else if (key == PROPERTY_IPV4_PDP_TYPE) {
        return true;
    }

    return AT_CellularBase_stub::supported_bool;
}
