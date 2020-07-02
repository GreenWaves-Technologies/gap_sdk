/*
 * Copyright (C) 2020  GreenWaves Technologies, SAS
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/* 
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#ifndef __PULP_UDMA_UDMA_IMPL_HPP__
#define __PULP_UDMA_UDMA_IMPL_HPP__

#ifdef UDMA_VERSION

#if UDMA_VERSION == 2
#include "udma_v2_impl.hpp"
#elif UDMA_VERSION == 3
#include "udma_v3_impl.hpp"
#elif UDMA_VERSION == 4
#include "udma_v4_impl.hpp"
#else
#error Unsupported UDMA version
#endif

#endif

#endif