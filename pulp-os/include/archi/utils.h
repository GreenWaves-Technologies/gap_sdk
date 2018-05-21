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


#ifndef __ARCHI_UTILS_H__
#define __ARCHI_UTILS_H__


// Generate register mask given offset and size of bitfield
#define ARCHI_REG_MASK(offset,size) (((1<<(size))-1) << (offset))

// Generate register value given current (full) register value, register field value, offset and size of bitfield
#define ARCHI_REG_FIELD_SET(fullValue,value,offset,size) (((fullValue) & ~ARCHI_REG_MASK(offset, size)) | ((value) << (offset)))

// Get register field value given current (full) register value, offset and size of bitfield
#define ARCHI_REG_FIELD_GET(fullValue,offset,size) (((fullValue) & ARCHI_REG_MASK(offset, size)) >> (offset))


#endif