/*
 * Copyright (c) 2018, GreenWaves Technologies, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of GreenWaves Technologies, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "gap_spis.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Array of GPIO peripheral base address. */
static PORT_Type *const port_addrs[] = PORT_BASE_PTRS;

typedef struct {
    PinName pin;
    int peripheral;
    int function;
} PinMap;

extern const PinMap PinMap_SPIS_SDIO0[];
extern const PinMap PinMap_SPIS_SDIO1[];
extern const PinMap PinMap_SPIS_SDIO2[];
extern const PinMap PinMap_SPIS_SDIO3[];
extern const PinMap PinMap_SPIS_CSN[];
extern const PinMap PinMap_SPIS_CLK[];

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
static void pin_function(PinName pin, int function)
{
    int pin_num = (pin & 0xFF) - GAP_PIN_OFFSET;

    if (0<= pin_num && pin_num < GAP_PORT_PIN_NUM )
        PORT_SetPinMux(port_addrs[GET_GPIO_PORT(pin)], pin_num, (port_mux_t)function);
}

static void pinmap_pinout(PinName pin, const PinMap *map) {
    if (pin == NC)
        return;

    while (map->pin != NC) {
        if (map->pin == pin) {
            pin_function(pin, map->function);

            return;
        }
        map++;
    }
}

void SPIS_Init(PinName mosi, PinName miso, PinName sclk, PinName ssel, PinName sdio2, PinName sdio3) {
    pinmap_pinout(mosi, PinMap_SPIS_SDIO1);
    pinmap_pinout(miso, PinMap_SPIS_SDIO0);
    pinmap_pinout(sclk, PinMap_SPIS_CLK);
    pinmap_pinout(ssel, PinMap_SPIS_CSN);
    pinmap_pinout(sdio2, PinMap_SPIS_SDIO2);
    pinmap_pinout(sdio3, PinMap_SPIS_SDIO3);
}
