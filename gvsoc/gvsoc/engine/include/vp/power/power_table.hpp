/*
 * Copyright (C) 2020 GreenWaves Technologies, SAS, ETH Zurich and
 *                    University of Bologna
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

/*
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#pragma once

#include "vp/vp_data.hpp"



namespace vp
{
    namespace power
    {
        class Linear_temp_table;
        class Linear_volt_table;

        /**
         * @brief Power values of a power characteristic
         * 
         * This manages the actual power value of a power characteristic, depending on current
         * temperature, voltage and frequency.
         * The value is interpolated, based on power numbers at various voltages, temperatures and frequencies.
         */
        class Linear_table
        {
        public:
            /**
             * @brief Construct a new Linear_table object
             * 
             * @param config JSON config giving all the power numbers at various frequencies, temperatures an voltages
             */
            Linear_table(js::config *config);

            /**
             * @brief Get the power value at specified temperature, voltage and frequency
             * 
             * The value is interpolated from the tables extracted from JSON tables.
             * 
             * @param temp         Temperature
             * @param volt         Voltage
             * @param frequency    Frequency
             * @return double      The estimated power value
             */
            double get(double temp, double volt, double frequency);

        private:
            // Vector of power tables at supported temperatures
            std::vector<Linear_temp_table *> temp_tables;
        };

        /**
         * @brief Power values of a power characteristic at a given temperature
         */
        class Linear_temp_table
        {
        public:
            /**
             * @brief Construct a new Linear_temp_table object
             * 
             * @param temp   Temperature for which this table is valid
             * @param config JSON config containing the power numbers
             */
            Linear_temp_table(double temp, js::config *config);

            /**
             * @brief Get the power value at specified voltage and frequency
             * 
             * The value is interpolated from the tables extracted from JSON tables.
             * These tables contains the power values at different voltages and frequencies
             * at the temperature for which this class has been created.
             * 
             * @param volt         Voltage
             * @param frequency    Frequency
             * @return double      The estimated power value
             */
            double get(double volt, double frequency);

            /**
             * @brief Get temperature for which this class has been created
             * 
             * @return double Temperature
             */
            inline double get_temp() { return this->temp; }

        private:
            std::vector<Linear_volt_table *> volt_tables;   // Power tables at various voltages
            double temp;           // Temperature for which this table has been created
        };

        /**
         * @brief Power values of a power characteristic at a given temperature and voltage
         */
        class Linear_volt_table
        {
        public:
            /**
             * @brief Construct a new Linear_volt_table object
             * 
             * @param volt      Voltage for which the power numbers are defined.
             * @param config    JSON config giving the power numbers for various frequencies at the specified voltage
             */
            Linear_volt_table(double volt, js::config *config);

            /**
             * @brief Get the power number at the specified frequency
             * 
             * The value is interpolated from the tables extracted from JSON tables.
             * These tables contains the power values at different frequencies
             * at the voltage and temperature for which this class has been created.
             * 
             * @param frequency Frequency at which the power number should be given
             * @return double 
             */
            inline double get(double frequency) { return any; }

            /**
             * @brief Get voltage for which this class has been created
             * 
             * @return double Voltage
             */
            inline double get_volt() { return this->volt; }

        private:
            double any;      // Power value for any frequency
            double volt;     // Voltage for which this class has been instantiated
        };
    };
};
