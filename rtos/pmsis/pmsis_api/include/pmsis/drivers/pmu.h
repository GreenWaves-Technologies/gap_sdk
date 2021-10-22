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

#ifndef __PMSIS_DRIVERS_PMU_H__
#define __PMSIS_DRIVERS_PMU_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup groupDrivers
 *
 * \defgroup PMU PMU
 *
 * \brief Power Management Unit.
 *
 * The PMU API provides support for power management of each domains,
 * sleep and wake up handling.
 *
 * \addtogroup PMU
 * \{
 */


/**
 * \brief Set voltage.
 *
 * This function allows to set voltage of a given power domain.
 *
 * \param domain         Power domain to configure.
 * \param voltage        Voltage to set, in mV.
 *
 * \retval 0             If operation is successful.
 * \retval ERRNO         An error code otherwise.
 *
 * \note When changing frequency, voltage may need to be changed first.
 */
int pi_pmu_voltage_set(pi_pmu_domain_e domain, uint32_t voltage);

/**
 * \brief Get current PMU state.
 *
 * This function is used to get current state of PMU.
 *
 * \param domain         Power domain to get current state.
 *
 * \return \ref pi_pmu_domain_state_e Power domain state.
 */
int pi_pmu_state_get(pi_pmu_domain_e domain);

/**
 * \brief Get current boot state.
 *
 * This function is used to get current boot state of PMU.
 *
 * \param domain         Power domain to get boot state.
 *
 * \return \ref pi_pmu_boot_state_e Wakeup boot state.
 */
int pi_pmu_boot_state_get(pi_pmu_domain_e domain);

/**
 * \brief Sleep mode setting.
 *
 * This function is to be called prior to enable sleep mode.
 *
 * \param domain         Power domain to configure.
 * \param conf           Pointer to struct pi_pmu_sleep_conf_s.
 *
 * \retval 0             If operation is successful.
 * \retval ERRNO         An error code otherwise.
 *
 * \note Waking device should be set up before calling this function(RTC or GPIO).
 */
int pi_pmu_sleep_mode_set(pi_pmu_domain_e domain, struct pi_pmu_sleep_conf_s *conf);

/**
 * \brief Activate sleep mode.
 *
 * \param domain         Power domain to configure.
 *
 * \retval 0             If operation is successful.
 * \retval ERRNO         An error code otherwise.
 */
int pi_pmu_sleep_mode_enable(pi_pmu_domain_e domain);


/**
 * \} addtogroup PMU
 */

#ifdef __cplusplus
}
#endif
#endif  /* __PMSIS_DRIVERS_PMU_H__ */
