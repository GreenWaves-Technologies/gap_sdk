/*
 * Copyright 2019 GreenWaves Technologies, SAS
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

#ifndef __BRIDGE_STUBS_H__
#define __BRIDGE_STUBS_H__

#if defined(__PULP_OS__)

#define BRIDGE_Init() {}
#define BRIDGE_Connect(__wait_bridge, __event)          rt_bridge_connect(__wait_bridge, __event)
#define BRIDGE_Open(__name, __flags, __mode, __event)   rt_bridge_open(__name, __flags, __mode, __event)
#define BRIDGE_Read(__file, __buffer, __size, __event)  rt_bridge_read(__file, __buffer, __size, __event)
#define BRIDGE_Write(__file, __buffer, __size, __event) rt_bridge_write(__file, __buffer, __size, __event)
#define BRIDGE_Close(__file, __event)                   rt_bridge_close(__file, __event)
#define BRIDGE_Disconnect(__event)                      rt_bridge_disconnect(__event)

#endif  /* __PULP_OS__ */

#endif  /* __BRIDGE_STUBS_H__ */
