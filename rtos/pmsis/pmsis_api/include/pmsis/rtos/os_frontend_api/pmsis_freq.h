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

#ifndef __PMSIS_FREQ_H__
#define __PMSIS_FREQ_H__

typedef enum {
  PI_FREQ_DOMAIN_FC     = 0,
  PI_FREQ_DOMAIN_CL     = 1,
  PI_FREQ_DOMAIN_PERIPH = 0
} pi_freq_domain_e;

/** \brief Get current frequency of a domain.
 *
 * Gets the current frequency of a specific frequency domain in Hz. 
 * 
 * \param     domain The frequency domain.
 * \return           The frequency in Hz.
 */
static inline uint32_t pi_freq_get(pi_freq_domain_e domain);



/** \brief Set frequency of a domain.
 *
 * Set thefrequency of a specific frequency domain in Hz. 
 * This can return an error if the frequency is invalid.
 * 
 * \param     domain The frequency domain.
 * \param     freq   The desired frequency in Hz.
 * \return           0 if successfull, -1 otherwise.
 */
static inline int32_t pi_freq_set(pi_freq_domain_e domain, uint32_t freq);

#endif