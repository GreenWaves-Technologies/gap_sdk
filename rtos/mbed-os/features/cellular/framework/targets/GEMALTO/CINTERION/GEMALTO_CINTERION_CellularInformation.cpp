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

#include "GEMALTO_CINTERION_CellularInformation.h"

namespace mbed {

GEMALTO_CINTERION_CellularInformation::GEMALTO_CINTERION_CellularInformation(ATHandler &at) : AT_CellularInformation(at)
{
}

nsapi_error_t GEMALTO_CINTERION_CellularInformation::get_iccid(char *buf, size_t buf_size)
{
    _at.lock();
    _at.cmd_start("AT^SCID");
    _at.cmd_stop();
    _at.resp_start("^SCID:");
    _at.read_string(buf, buf_size);
    _at.resp_stop();
    return _at.unlock_return_error();
}

} /* namespace mbed */
