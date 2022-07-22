/*
 * Copyright (C) 2019 GreenWaves Technologies
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

#ifndef __ARCHI_UDMA_UDMA_V4_H__
#define __ARCHI_UDMA_UDMA_V4_H__

#define UDMA_CTRL_NO_CHANNEL 0xFF

#define UDMA_CTRL_OFFSET        0x0
#define UDMA_PERIPH_AREA_OFFSET 0x80
#define UDMA_LIN_ADDRGEN_OFFSET 0x1000
#define UDMA_LIN_ADDRGEN_SIZE   0x20
#define UDMA_2D_ADDRGEN_OFFSET 0x1800
#define UDMA_2D_ADDRGEN_SIZE   0x20

#define UDMA_LIN_ADDRGEN_ADDR(id)   (UDMA_ADDR + UDMA_LIN_ADDRGEN_OFFSET + id*UDMA_LIN_ADDRGEN_SIZE)
#define UDMA_2D_ADDRGEN_ADDR(id)   (UDMA_ADDR + UDMA_2D_ADDRGEN_OFFSET + id*UDMA_2D_ADDRGEN_SIZE)

/*
 * Global register map
 */

// Periph size in log2
#define UDMA_PERIPH_SIZE_LOG2  7

// Periph size
#define UDMA_PERIPH_SIZE       (1<<UDMA_PERIPH_SIZE_LOG2)

// Channel area size in log2
#define UDMA_CHANNEL_SIZE_LOG2      5

// Channel area size
#define UDMA_CHANNEL_SIZE           (1<<UDMA_CHANNEL_SIZE_LOG2)

/*
 * Macros
 */

// Return the offset of a peripheral from its identifier
#define UDMA_PERIPH_OFFSET(id)              (((id)<<UDMA_PERIPH_SIZE_LOG2)+UDMA_PERIPH_AREA_OFFSET)

// Returns the identifier of a peripheral from its offset relative to the beginning of the peripheral area
#define UDMA_PERIPH_GET(offset)             ((offset)>>UDMA_PERIPH_SIZE_LOG2)

// Return the offset of a channel from its identifier
#define UDMA_CHANNEL_OFFSET(id)              ((id)<<UDMA_CHANNEL_SIZE_LOG2)

// Returns the identifier of a channel from its offset relative to the beginning of the channel area
#define UDMA_CHANNEL_GET(offset)             ((offset)>>UDMA_CHANNEL_SIZE_LOG2)

#endif