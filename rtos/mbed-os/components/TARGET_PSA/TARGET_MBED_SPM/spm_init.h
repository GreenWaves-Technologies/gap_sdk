/* Copyright (c) 2017-2018 ARM Limited
 *
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

#ifndef __MBED_SPM_INIT_H__
#define __MBED_SPM_INIT_H__

#if defined(COMPONENT_SPE)

/*
 * Initialize SPM
 */
void psa_spm_init(void);

#endif // defined(COMPONENT_SPE)

#if defined(COMPONENT_SPM_MAILBOX)

/*
 * Initialize SPM mailbox driver
 */
void spm_ipc_mailbox_init(void);

#if defined(COMPONENT_NSPE)

/*
 * PSA mailbox dispatcher
 *
 * Dispatcher thread never returns and expected to be called
 * from startup code as a detached thread entry point
 */
void psa_spm_mailbox_dispatcher(void);

#endif // defined(COMPONENT_NSPE)

#endif // defined(COMPONENT_SPM_MAILBOX)

#endif // __MBED_SPM_INIT_H__
