/*
 * Copyright (C) 2020 GreenWaves Technologies
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

#ifndef __BSP_DEBUG_H__
#define __BSP_DEBUG_H__

#if defined(TRACE_PARTITION)
#define PARTITION_TRACE_TRC(...) PI_LOG_TRC(__func__, __VA_ARGS__)
#define PARTITION_TRACE_ERR(...) PI_LOG_ERR(__func__, __VA_ARGS__)
#else
#define PARTITION_TRACE_TRC(...) ((void) 0)
#define PARTITION_TRACE_ERR(...) ((void) 0)
#endif  /* TRACE_PARTITION */

#endif  /* __BSP_DEBUG_H__ */
