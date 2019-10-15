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

#if PULP_CHIP == CHIP_QUENTIN || PULP_CHIP == CHIP_KERBIN || PULP_CHIP == CHIP_PULP || PULP_CHIP == CHIP_PULP_V1 || PULP_CHIP == CHIP_ARNOLD || PULP_CHIP == CHIP_VEGA || PULP_CHIP == CHIP_GAP9

#define __RT_FREQ_DOMAIN_FC 1
#define __RT_FREQ_DOMAIN_CL 2
#define __RT_FREQ_DOMAIN_PERIPH 0
#define RT_FREQ_NB_DOMAIN 3

#elif PULP_CHIP == CHIP_PULPISSIMO || PULP_CHIP == CHIP_PULPISSIMO_V1

#define __RT_FREQ_DOMAIN_FC 0
// TODO it is just defined here to avoid compilation issue
#define __RT_FREQ_DOMAIN_CL 2
#define __RT_FREQ_DOMAIN_PERIPH 1
#define RT_FREQ_NB_DOMAIN 2

#elif PULP_CHIP_FAMILY == CHIP_VIVOSOC3 || PULP_CHIP_FAMILY == CHIP_VIVOSOC3_1

#define __RT_FREQ_DOMAIN_FC       1   // soc domain, soc fll
#define __RT_FREQ_DOMAIN_CL       0   // cluster domain, soc fll or cl fll
#define __RT_FREQ_DOMAIN_PERIPH   2   // periph domain, soc fll or per fll
#define __RT_FREQ_DOMAIN_PM       3   // pm domain, soc fll
#define RT_FREQ_NB_DOMAIN         4 

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


#if PULP_CHIP_FAMILY == CHIP_VIVOSOC3 || PULP_CHIP_FAMILY == CHIP_VIVOSOC3_1

/** \enum rt_freq_domain_e
 * \brief Frequency domains.
 *
 * Describes a frequency domain.
 */
typedef enum {
  RT_FREQ_DOMAIN_FC     = __RT_FREQ_DOMAIN_FC,     /*!< Frequency domain for fabric controller. */
  RT_FREQ_DOMAIN_CL     = __RT_FREQ_DOMAIN_CL,     /*!< Frequency domain for cluster 0. Other cluster domains can be obtained by adding the cluster ID to this constant. */  
  RT_FREQ_DOMAIN_PERIPH = __RT_FREQ_DOMAIN_PERIPH, /*!< Frequency domain for peripherals with secondary udma clk */ 
  RT_FREQ_DOMAIN_PM     = __RT_FREQ_DOMAIN_PM,     /*!< Frequency domain for power manager. */
} rt_freq_domain_e;

/** \brief Get current frequency of a domain.
 *
 * Gets the current frequency of a specific frequency domain in Hz. 
 * 
 * \param     domain The frequency domain.
 * \return           The frequency in Hz.
 */
static inline unsigned int rt_freq_get(rt_freq_domain_e domain);

/** \brief Get current config of a domain.
 *
 * Gets the current clock tree configuration of a specific frequency domain. 
 * 
 * \param     domain The frequency domain.
 * \return           The clock tree configuration.
 */
static inline hal_freq_domain_clk_tree_config_e rt_freq_config_get(rt_freq_domain_e domain);

/** \brief Get current config of a domain.
 *
 * Gets the current clock tree configuration of a specific frequency domain. 
 * 
 * \param     domain The frequency domain.
 * \param     config The clock tree configuration.
 * \return           0 if success, -1 otherwise.
 */
extern int rt_freq_config_set(rt_freq_domain_e domain, hal_freq_domain_clk_tree_config_e config);

/** \brief Get frequency of a set of a given FLL. 
 *
 * Gets the frequency of the given set of a specific FLL in Hz. 
 * 
 * \param     fll    The FLL.
 * \param     set    The set of the FLL.
 * \return           The frequency in Hz.
 */
extern unsigned int rt_freq_fll_freq_get(hal_fll_e fll, hal_fll_reg_set_e set);

/** \brief Set frequency of a set of a given FLL. 
 *
 * Gets the frequency of the given set of a specific FLL in Hz. 
 * 
 * \param     fll    The FLL.
 * \param     set    The set of the FLL.
 * \param     freq   The frequency in Hz.
 * \return           The frequency that was set in Hz.
 */
extern unsigned int rt_freq_fll_freq_set(hal_fll_e fll, hal_fll_reg_set_e set, unsigned int freq);

/** \brief Get status of a set of a given FLL. 
 *
 * Gets the status of the given set of a specific FLL in Hz. 
 * 
 * \param     fll    The FLL.
 * \return           0 if set 0 active, 1 if set 1 active, -1 if FLL disabled.
 */
extern int rt_freq_fll_status_get(hal_fll_e fll);

/** \brief Set frequency of a set of a given FLL. 
 *
 * Gets the frequency of the given set of a specific FLL in Hz. 
 * 
 * \param     fll    The FLL.
 * \param     set    The set of the FLL.
 * \param     enable 0: disable the FLL, 1: enable the FLL.
 * \return           0 if success, -1 otherwise.
 */
extern int rt_freq_fll_status_set(hal_fll_e fll, hal_fll_reg_set_e set, unsigned char enable);

#else
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

#endif

//!@}

/**        
 * @}
 */

/// @cond IMPLEM

#if PULP_CHIP_FAMILY == CHIP_VIVOSOC3 || PULP_CHIP_FAMILY == CHIP_VIVOSOC3_1

  typedef struct __rt_freq_domain_settings_s {   
    unsigned int __rt_freq_domain_freq;
    hal_freq_domain_clk_tree_config_e __rt_freq_domain_config;
    char __rt_freq_domain_fll_on;
    char __rt_freq_domain_alt_fll_on;
  } __rt_freq_domain_settings_t;

  extern __rt_freq_domain_settings_t __rt_freq_domain_settings[];  

  // fll helper functions
  void __rt_freq_init(void);  
  void __rt_flls_constructor(void);
  void __rt_fll_set_init(hal_fll_e fll);
  void __rt_fll_disable(hal_fll_e fll);
  unsigned int __rt_fll_active_freq_get(hal_fll_e fll);
  unsigned int __rt_fll_set_freq_get(hal_fll_e fll, hal_fll_reg_set_e set);
  unsigned int __rt_fll_set_freq_set(hal_fll_e fll, hal_fll_reg_set_e set, unsigned int frequency);
  hal_fll_reg_set_e __rt_fll_set_active_get(hal_fll_e fll);
  unsigned int __rt_fll_set_active_set(hal_fll_e fll, hal_fll_reg_set_e set);
  unsigned int __rt_fll_enable(hal_fll_e fll);
  void __rt_fll_disable(hal_fll_e fll);

  static inline unsigned int rt_freq_get(rt_freq_domain_e domain) 
  {
    return __rt_freq_domain_settings[domain].__rt_freq_domain_freq; // freq in Hz
  }

  static inline hal_freq_domain_clk_tree_config_e rt_freq_config_get(rt_freq_domain_e domain)
  {
    return __rt_freq_domain_settings[domain].__rt_freq_domain_config;
  }  

  static inline unsigned int __rt_freq_periph_get()
  {
    return rt_freq_get(RT_FREQ_DOMAIN_PERIPH);
  }  
   
#else

  #if PULP_CHIP == CHIP_QUENTIN || PULP_CHIP == CHIP_KERBIN || PULP_CHIP == CHIP_PULP || PULP_CHIP == CHIP_PULP_V1 || PULP_CHIP == CHIP_ARNOLD
  #define __RT_FLL_CL 2
  #define __RT_FLL_PERIPH 1
  #define __RT_FLL_FC 0
  #elif PULP_CHIP == CHIP_PULPISSIMO || PULP_CHIP == CHIP_PULPISSIMO_V1
  #define __RT_FLL_CL 2
  #define __RT_FLL_PERIPH 1
  #define __RT_FLL_FC 0
  #elif PULP_CHIP == CHIP_VEGA || PULP_CHIP == CHIP_GAP9
  #define __RT_FLL_PERIPH 0
  #define __RT_FLL_FC     1
  #define __RT_FLL_CL     2
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

  static inline void __rt_freq_set_value(rt_freq_domain_e domain, unsigned int freq)
  {
    __rt_freq_domains[domain] = freq;
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
  #if PULP_CHIP_FAMILY == CHIP_USOC_V1
    return 60000000;
  #else
  #if PULP_CHIP_FAMILY == CHIP_VIVOSOC2
    return 100000000;
  #else
    return 50000000;
  #endif
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

  void rt_freq_wait_convergence(int fll);

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
  #if PULP_CHIP_FAMILY == CHIP_USOC_V1
    return 50000000;
  #else
    return rt_freq_get(RT_FREQ_DOMAIN_PERIPH);
  #endif
  }

  static inline unsigned int __rt_freq_periph_get_next()
  {
    return rt_freq_get_next(RT_FREQ_DOMAIN_PERIPH);
  }
#endif  

/// @endcond

#endif