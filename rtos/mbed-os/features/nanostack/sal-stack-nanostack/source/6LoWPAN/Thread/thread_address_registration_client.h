/*
 * Copyright (c) 2017-2018, Arm Limited and affiliates.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef THREAD_ADDRESS_REGISTRATION_CLIENT_H_
#define THREAD_ADDRESS_REGISTRATION_CLIENT_H_


#ifdef HAVE_THREAD_V2

void thread_address_registration_init(void);
void thread_address_registration_deinit(void);

void thread_address_registration_timer_set(protocol_interface_info_entry_t *interface, uint16_t dua_delay_seconds, uint16_t mlr_refresh_seconds);
void thread_address_registration_timer(protocol_interface_info_entry_t *interface, uint16_t seconds);
#else

#define thread_address_registration_init(void)
#define thread_address_registration_deinit(void)

#define thread_address_registration_timer_set(interface, dua_delay_seconds, mlr_refresh_seconds);
#define thread_address_registration_timer(interface, seconds);

#endif


#endif /* THREAD_ADDRESS_REGISTRATION_CLIENT_H_ */
