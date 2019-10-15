/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
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
#include "sleep_api.h"
#include "cmsis.h"
#include "mbed_interface.h"
#include "softdevice_handler.h"
#include "nrf_soc.h"

// Mask of reserved bits of the register ICSR in the System Control Block peripheral
// In this case, bits which are equal to 0 are the bits reserved in this register
#define SCB_ICSR_RESERVED_BITS_MASK     0x9E43F03F

void hal_sleep(void)
{
    // ensure debug is disconnected if semihost is enabled....

    // Trigger an event when an interrupt is pending. This allows to wake up
    // the processor from disabled interrupts.
    SCB->SCR |= SCB_SCR_SEVONPEND_Msk;

    // If the SoftDevice is enabled, its API must be used to go to sleep.
    if (softdevice_handler_isEnabled()) {
        sd_power_mode_set(NRF_POWER_MODE_LOWPWR);
        sd_app_evt_wait();
    } else {
        NRF_POWER->TASKS_LOWPWR = 1;

        // Note: it is not sufficient to just use WFE here, since the internal
        // event register may be already set from an event that occurred in the
        // past (like an SVC call to the SoftDevice) and in such case WFE will
        // just clear the register and continue execution.
        // Therefore, the strategy here is to first clear the event register
        // by using SEV/WFE pair, and then execute WFE again, unless there is
        // a pending interrupt.

        // Set an event and wake up whatsoever, this will clear the event
        // register from all previous events set (SVC call included)
        __SEV();
        __WFE();

        // Test if there is an interrupt pending (mask reserved regions)
        if (SCB->ICSR & (SCB_ICSR_RESERVED_BITS_MASK)) {
            // Ok, there is an interrut pending, no need to go to sleep
            return;
        } else {
            // next event will wakeup the CPU
            // If an interrupt occured between the test of SCB->ICSR and this
            // instruction, WFE will just not put the CPU to sleep
            __WFE();
        }
    }
}

void hal_deepsleep(void)
{
    hal_sleep();
    //   NRF_POWER->SYSTEMOFF=1;
}
