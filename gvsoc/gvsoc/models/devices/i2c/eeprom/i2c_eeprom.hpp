/*
 * Copyright (C) 2021 GreenWaves Technologies, SAS, ETH Zurich and
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

#pragma once

#include <vp/vp.hpp>
#include <vp/itf/clock.hpp>
#include <vp/itf/i2c.hpp>

#include <vector>

#include "i2c_helper.hpp"

class I2c_eeprom_memory
{
    public:
        I2c_eeprom_memory(void);

        // set the current reading address
        void set_address(int address);

        // return byte at the current address, increments address by 1 (wrap at the end of a page)
        uint8_t read(void);
        // write byte at the current address, increments address by 1 (wrap at the end of a page)
        void write(uint8_t byte);

        void initialize_memory(int number_of_pages, int page_size, uint8_t default_value);
        void erase_memory(void);

    private:
        void increment_address(void);

        int current_address;

        int number_of_pages;
        int page_size;
        uint8_t default_value;

        std::vector<std::vector<uint8_t>> memory;
};

class I2c_eeprom : public vp::component
{
    public:
        I2c_eeprom(js::config* config);

        int build(void);
        void start(void);
        void reset(bool active);

    private:
        static void event_handler(void *__this, vp::clock_event *event);

        void i2c_enqueue_event(vp::clock_event* event, uint64_t time_ps);
        void i2c_cancel_event(vp::clock_event* event);
        static void i2c_sync(void *__this, int scl, int sda);
        void i2c_helper_callback(i2c_operation_e id, i2c_status_e status, int value);

        /**********/
        /* FIELDS */
        /**********/

        /** trace utility */
        vp::trace trace;

        /** I2C interface */
        vp::i2c_master i2c_itf;

        /* clock related */
        vp::clock_master clock_cfg;

        /** I2C slave helper */
        I2C_helper i2c_helper;

        /** I2C address of the eeprom */
        int i2c_address;

        /** size of the page in byte */
        int64_t page_size;
        /** number of pages */
        int64_t number_of_pages;

        /** memory */
        I2c_eeprom_memory memory;

        /* state data */
        bool starting = false;
        bool is_addressed = false;
        bool is_read = false;

        uint32_t byte_counter = 0;
        uint32_t current_address = 0x0;
};
