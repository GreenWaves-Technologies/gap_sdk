/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#ifndef __PULP_UDMA_UDMA_IMPL_HPP__
#define __PULP_UDMA_UDMA_IMPL_HPP__

#ifdef UDMA_VERSION

#if UDMA_VERSION == 2
#include "udma_v2_impl.hpp"
#elif UDMA_VERSION == 3
#include "udma_v3_impl.hpp"
#else
#error Unsupported UDMA version
#endif

#endif

#endif