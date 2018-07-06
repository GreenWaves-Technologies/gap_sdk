/*
 * Copyright (C) 2018 ETH Zurich, University of Bologna and GreenWaves Technologies
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

#ifndef __RT_RT_FREQ_H__
#define __RT_RT_FREQ_H__

#include "rt/rt_data.h"

/// @cond IMPLEM

#if !defined(ARCHI_NB_FLL) || ARCHI_NB_FLL == 1

#define __RT_FREQ_DOMAIN_FC 0
#define __RT_FREQ_DOMAIN_CL 0
#define __RT_FREQ_DOMAIN_PERIPH 0
#define RT_FREQ_NB_DOMAIN  1

#else

#if PULP_CHIP == CHIP_QUENTIN || PULP_CHIP == CHIP_KERBIN || PULP_CHIP == CHIP_PULP

#define __RT_FREQ_DOMAIN_FC 0
#define __RT_FREQ_DOMAIN_CL 2
#define __RT_FREQ_DOMAIN_PERIPH 1
#define RT_FREQ_NB_DOMAIN 3

#else

#define __RT_FREQ_DOMAIN_FC 0
#define __RT_FREQ_DOMAIN_CL 1
#define __RT_FREQ_DOMAIN_PERIPH 0
#define RT_FREQ_NB_DOMAIN 2

#endif

#endif

/// @endcond


/**        
 * @ingroup groupKernel        
 */

/**        
 * @defgroup Freq Frequency management
 * 
 */



/**        
 * @addtogroup Freq
 * @{        
 */

/**@{*/



/** \enum rt_freq_domain_e
 * \brief Frequency domains.
 *
 * Describes a frequency domain.
 */
typedef enum {
  RT_FREQ_DOMAIN_FC     = __RT_FREQ_DOMAIN_FC,     /*!< Frequency domain for fabric controller. */
  RT_FREQ_DOMAIN_CL     = __RT_FREQ_DOMAIN_CL,     /*!< Frequency domain for cluster 0. Other cluster domains can be obtained by adding the cluster ID to this constant. */  
  RT_FREQ_DOMAIN_PERIPH = __RT_FREQ_DOMAIN_PERIPH,
} rt_freq_domain_e;



/** \brief Get current frequency of a domain.
 *
 * Gets the current frequency of a specific frequency domain in Hz. 
 * 
 * \param     domain The frequency domain.
 * \return           The frequency in Hz.
 */
static inline int rt_freq_get(rt_freq_domain_e domain);



/** \brief Set frequency of a domain.
 *
 * Set thefrequency of a specific frequency domain in Hz. 
 * This can return an error if the frequency is invalid.
 * 
 * \param     domain The frequency domain.
 * \param     freq   The desired frequency in Hz.
 * \return           0 if successfull, -1 otherwise.
 */
static inline int rt_freq_set(rt_freq_domain_e domain, unsigned int freq);


//!@}

/**        
 * @}
 */


/// @cond IMPLEM

#if PULP_CHIP == CHIP_QUENTIN || PULP_CHIP == CHIP_KERBIN || PULP_CHIP == CHIP_PULP
#define __RT_FLL_CL 2
#define __RT_FLL_PERIPH 1
#define __RT_FLL_FC 0
#else
#define __RT_FLL_FC     0
#define __RT_FLL_PERIPH 0
#define __RT_FLL_CL     1
#endif

int rt_freq_set_and_get(rt_freq_domain_e domain, unsigned int freq, unsigned int *out_freq);

extern int __rt_freq_domains[];
extern int __rt_freq_next_domains[];

static inline int rt_freq_set(rt_freq_domain_e domain, unsigned int freq)
{
  return rt_freq_set_and_get(domain, freq, NULL);
}

#if defined(FLL_VERSION)

static inline int rt_freq_get(rt_freq_domain_e domain)
{
  return __rt_freq_domains[domain];
}

static inline int rt_freq_get_next(rt_freq_domain_e domain)
{
  return __rt_freq_next_domains[domain];
}

#else

static inline int rt_freq_get(rt_freq_domain_e domain)
{
#if PULP_CHIP_FAMILY == CHIP_VIVOSOC2
  return 100000000;
#else
  return 50000000;
#endif
}

static inline int rt_freq_get_next(rt_freq_domain_e domain)
{
#if PULP_CHIP_FAMILY == CHIP_VIVOSOC2
  return 100000000;
#else
  return 50000000;
#endif
}

#endif

int __rt_freq_set_constraint_multiple(unsigned int freq);

void __rt_freq_remove_constraint_multiple(unsigned int freq);

unsigned int __rt_fll_set_freq(int fll, unsigned int frequency);

unsigned int __rt_fll_init(int fll);

void __rt_fll_deinit(int fll);

void __rt_freq_init();

void __rt_flls_constructor();

extern int __rt_fll_freq[];

static inline unsigned int __rt_fll_freq_get(int fll)
{
  return __rt_fll_freq[fll];
}

static inline unsigned int __rt_freq_periph_get()
{
#if PULP_CHIP == CHIP_VIVOSOC3
  return 10000000;
#else
  return rt_freq_get(RT_FREQ_DOMAIN_PERIPH);
#endif
}

static inline unsigned int __rt_freq_periph_get_next()
{
  return rt_freq_get_next(RT_FREQ_DOMAIN_PERIPH);
}

/// @endcond

#endif
