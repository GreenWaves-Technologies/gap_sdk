/*
 * Copyright (C) 2020 GreenWaves Technologies
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

#ifndef __PMSIS_CHIPS_VEGA_PMU_H__
#define __PMSIS_CHIPS_VEGA_PMU_H__

/**
 * \ingroup groupChips
 */

/**
 * \addtogroup VEGA
 * @{
 */

/**
 * \defgroup VEGA_PMU VEGA PMU
 *
 * This part enumerates VEGA power domains.
 *
 * \addtogroup VEGA_PMU
 * \{
 */

/**
 * \enum pi_pmu_domain_e
 *
 * \brief PMU domains.
 *
 * This is used to identify power domains on VEGA.
 */
typedef enum
{
    PI_PMU_DOMAIN_FC     = 0,   /*!< FC power domain. */
    PI_PMU_DOMAIN_CL     = 1,   /*!< Cluster power domain. */
} pi_pmu_domain_e;

/**
 * \cond IMPLEM
 */

/**
 * \endcond IMPLEM
 */

/**
 * \}
 */

/**
 * \}
 */

#endif  /* __PMSIS_CHIPS_VEGA_PAD_H__ */
