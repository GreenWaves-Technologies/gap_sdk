/*
 * Copyright (C) 2020  GreenWaves Technologies, SAS
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

#include <stdint.h>
#include <vp/vp.hpp>

namespace vp
{
    class regfield
    {
        public:
        regfield(std::string name, int bit, int width) : name(name), bit(bit), width(width) {}
        std::string name;
        int bit;
        int width;
    };

    class reg
    {

    public:
        std::string get_hw_name() { return this->hw_name; }
        std::string get_name() { return this->name != "" ? this->name : this->hw_name; }
        void init(vp::component *top, std::string name, int bits, uint8_t *value, uint8_t *reset_val);
        void reset(bool active);
        virtual void access(uint64_t reg_offset, int size, uint8_t *value, bool is_write) {}
        virtual void update(uint64_t reg_offset, int size, uint8_t *value, bool is_write) {}

        virtual void build(vp::component *comp, std::string name) {}

        inline uint8_t *get_bytes() { return this->value_bytes; }
        inline void set_1(uint8_t value) { *(uint8_t *)this->value_bytes = value; }
        inline void set_8(uint8_t value) { *(uint8_t *)this->value_bytes = value; }
        inline void set_16(uint16_t value) { *(uint16_t *)this->value_bytes = value; }
        inline void set_32(uint32_t value) { *(uint32_t *)this->value_bytes = value; }
        inline void set_64(uint64_t value) { *(uint64_t *)this->value_bytes = value; }

        inline void release()
        {
            this->trace.msg("Release register\n");
            if (this->reg_event.get_event_active())
                this->reg_event.event(NULL);
        }

        inline void read(int reg_offset, int size, uint8_t *value) { memcpy((void *)value, (void *)(this->value_bytes + reg_offset), size); }
        inline void read(uint8_t *value) { memcpy((void *)value, (void *)this->value_bytes, this->nb_bytes); }
        inline uint8_t get_1() { return *(uint8_t *)this->value_bytes; }
        inline uint8_t get_8() { return *(uint8_t *)this->value_bytes; }
        inline uint16_t get_16() { return *(uint16_t *)this->value_bytes; }
        inline uint32_t get_32() { return *(uint32_t *)this->value_bytes; }
        inline uint64_t get_64() { return *(uint64_t *)this->value_bytes; }
        uint64_t get_field(int offset, int width);
        void register_callback(std::function<void(uint64_t, int, uint8_t *, bool)> callback) { this->callback = callback; }
        bool access_callback(uint64_t reg_offset, int size, uint8_t *value, bool is_write);
        void register_alias(std::function<reg *()> alias) { this->alias = alias; }

        int nb_bytes;
        int bits;
        uint8_t *reset_value_bytes;
        uint8_t *value_bytes;
        std::string name = "";
        std::string hw_name;
        component *top;
        vp::trace trace;
        vp::trace reg_event;
        std::vector<regfield *> regfields;
        bool do_reset;
        uint64_t offset;
        int width;
        std::function<void(uint64_t, int, uint8_t *, bool)> callback = NULL;
        std::function<reg *()> alias = NULL;
    };

    class reg_1 : public reg
    {
    public:
        void init(vp::component *top, std::string name, uint8_t *reset_val);
        void build(vp::component *comp, std::string name);

        inline uint8_t get() { return this->value; }
        inline void set(uint8_t value)
        {
            this->trace.msg("Setting register (value: 0x%x)\n", value);
            this->value = value;
            if (this->reg_event.get_event_active())
                this->reg_event.event((uint8_t *)&this->value);
        }
        inline void inc(uint8_t value)
        {
            this->set(this->get() + value);
        }
        inline void dec(uint8_t value)
        {
            this->set(this->get() - value);
        }
        inline void write(uint8_t *value)
        {
            memcpy((void *)this->value_bytes, (void *)value, this->nb_bytes);
            if (this->reg_event.get_event_active())
                this->reg_event.event((uint8_t *)this->value_bytes);
        }
        inline void write(int reg_offset, int size, uint8_t *value)
        {
            memcpy((void *)(this->value_bytes + reg_offset), (void *)value, size);
            this->dump_after_write();
            if (this->reg_event.get_event_active())
                this->reg_event.event((uint8_t *)this->value_bytes);
        }
        inline void dump_after_write() { this->trace.msg("Modified register (value: 0x%x)\n", this->value); }
        inline void update(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
        {
            if (is_write)
                this->write(reg_offset, size, value);
            else
                this->read(reg_offset, size, value);
        }

        inline void access(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
        {
            if (!this->access_callback(reg_offset, size, value, is_write))
            {
                this->update(reg_offset, size, value, is_write);
            }
        }

    protected:
        uint8_t reset_val;
        uint8_t write_mask = 0xFF;

    private:
        uint8_t value;
    };

    class reg_8 : public reg
    {
    public:
        void init(vp::component *top, std::string name, uint8_t *reset_val);
        void build(vp::component *comp, std::string name);

        inline uint8_t get() { return this->value; }
        inline void set(uint8_t value)
        {
            this->trace.msg("Setting register (value: 0x%x)\n", value);
            this->value = value;
            if (this->reg_event.get_event_active())
                this->reg_event.event((uint8_t *)&this->value);
        }
        inline void inc(uint8_t value)
        {
            this->set(this->get() + value);
        }
        inline void dec(uint8_t value)
        {
            this->set(this->get() - value);
        }
        inline void set_field(uint8_t value, int offset, int width)
        {
            this->value = (this->value & ~(((1UL << width) - 1) << offset)) | (value << offset);
            if (this->reg_event.get_event_active())
                this->reg_event.event((uint8_t *)&this->value);
        }
        inline uint8_t get_field(int offset, int width) { return (this->value >> offset) & ((1 << width) - 1); }
        inline void write(uint8_t *value)
        {
            memcpy((void *)this->value_bytes, (void *)value, this->nb_bytes);
            if (this->reg_event.get_event_active())
                this->reg_event.event((uint8_t *)this->value_bytes);
        }
        inline void write(int reg_offset, int size, uint8_t *value)
        {
            memcpy((void *)(this->value_bytes + reg_offset), (void *)value, size);
            this->dump_after_write();
            if (this->reg_event.get_event_active())
                this->reg_event.event((uint8_t *)this->value_bytes);
        }
        inline void dump_after_write() { this->trace.msg("Modified register (value: 0x%x)\n", this->value); }
        inline void update(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
        {
            if (is_write)
                this->write(reg_offset, size, value);
            else
                this->read(reg_offset, size, value);
        }

        inline void access(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
        {
            if (!this->access_callback(reg_offset, size, value, is_write))
            {
                this->update(reg_offset, size, value, is_write);
            }
        }

    protected:
        uint8_t reset_val;
        uint8_t write_mask = 0xFF;

    private:
        uint8_t value;
    };

    class reg_16 : public reg
    {
    public:
        void init(vp::component *top, std::string name, uint8_t *reset_val);
        void build(vp::component *comp, std::string name);

        inline uint16_t get() { return this->value; }
        inline void set(uint16_t value)
        {
            this->trace.msg("Setting register (value: 0x%x)\n", value);
            this->value = value;
            if (this->reg_event.get_event_active())
                this->reg_event.event((uint8_t *)&this->value);
        }
        inline void inc(uint16_t value)
        {
            this->set(this->get() + value);
        }
        inline void dec(uint16_t value)
        {
            this->set(this->get() - value);
        }
        inline void set_field(uint16_t value, int offset, int width)
        {
            this->value = (this->value & ~(((1UL << width) - 1) << offset)) | (value << offset);
            if (this->reg_event.get_event_active())
                this->reg_event.event((uint8_t *)&this->value);
        }
        inline uint16_t get_field(int offset, int width) { return (this->value >> offset) & ((1 << width) - 1); }
        inline void write(uint8_t *value)
        {
            memcpy((void *)this->value_bytes, (void *)value, this->nb_bytes);
            if (this->reg_event.get_event_active())
                this->reg_event.event((uint8_t *)this->value_bytes);
        }
        inline void write(int reg_offset, int size, uint8_t *value)
        {
            memcpy((void *)(this->value_bytes + reg_offset), (void *)value, size);
            this->dump_after_write();
            if (this->reg_event.get_event_active())
                this->reg_event.event((uint8_t *)this->value_bytes);
        }
        inline void dump_after_write() { this->trace.msg("Modified register (value: 0x%x)\n", this->value); }
        inline void update(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
        {
            if (is_write)
                this->write(reg_offset, size, value);
            else
                this->read(reg_offset, size, value);
        }

        inline void access(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
        {
            if (!this->access_callback(reg_offset, size, value, is_write))
            {
                this->update(reg_offset, size, value, is_write);
            }
        }

    protected:
        uint16_t reset_val;
        uint16_t write_mask = 0xFFFF;

    private:
        uint16_t value;
    };

    class reg_32 : public reg
    {
    public:
        void init(vp::component *top, std::string name, uint8_t *reset_val);
        void build(vp::component *comp, std::string name);

        inline uint32_t get() { return this->value; }
        inline void set(uint32_t value)
        {
            this->trace.msg("Setting register (value: 0x%x)\n", value);
            this->value = value;
            if (this->reg_event.get_event_active())
                this->reg_event.event((uint8_t *)&this->value);
        }
        inline void inc(uint32_t value)
        {
            this->set(this->get() + value);
        }
        inline void dec(uint32_t value)
        {
            this->set(this->get() - value);
        }
        inline void set_field(uint32_t value, int offset, int width)
        {
            this->value = (this->value & ~(((1UL << width) - 1) << offset)) | (value << offset);
            if (this->reg_event.get_event_active())
                this->reg_event.event((uint8_t *)&this->value);
        }
        inline uint32_t get_field(int offset, int width) { return (this->value >> offset) & ((1 << width) - 1); }
        inline void write(uint8_t *value) { this->write(0, 4, value); }
        void write(int reg_offset, int size, uint8_t *value);
        inline void dump_after_write() { this->trace.msg("Modified register (value: 0x%x)\n", this->value); }
        inline void update(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
        {
            if (is_write)
                this->write(reg_offset, size, value);
            else
                this->read(reg_offset, size, value);
        }

        inline void access(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
        {
            if (!this->access_callback(reg_offset, size, value, is_write))
            {
                this->update(reg_offset, size, value, is_write);
            }
        }

    protected:
        uint32_t reset_val;
        uint32_t write_mask = 0xFFFFFFFF;

    private:
        uint32_t value;
    };

    class reg_64 : public reg
    {
    public:
        void init(vp::component *top, std::string name, uint8_t *reset_val);
        void build(vp::component *comp, std::string name);

        inline uint64_t get() { return this->value; }
        inline void set(uint64_t value)
        {
            this->trace.msg("Setting register (value: 0x%x)\n", value);
            this->value = value;
            if (this->reg_event.get_event_active())
                this->reg_event.event((uint8_t *)&this->value);
        }
        inline void inc(uint64_t value)
        {
            this->set(this->get() + value);
        }
        inline void dec(uint64_t value)
        {
            this->set(this->get() - value);
        }
        inline void write(uint8_t *value)
        {
            memcpy((void *)this->value_bytes, (void *)value, this->nb_bytes);
            if (this->reg_event.get_event_active())
                this->reg_event.event((uint8_t *)this->value_bytes);
        }
        inline void write(int reg_offset, int size, uint8_t *value)
        {
            memcpy((void *)(this->value_bytes + reg_offset), (void *)value, size);
            this->dump_after_write();
            if (this->reg_event.get_event_active())
                this->reg_event.event((uint8_t *)this->value_bytes);
        }
        inline void dump_after_write() { this->trace.msg("Modified register (value: 0x%x)\n", this->value); }
        inline void update(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
        {
            if (is_write)
                this->write(reg_offset, size, value);
            else
                this->read(reg_offset, size, value);
        }

        inline void access(uint64_t reg_offset, int size, uint8_t *value, bool is_write)
        {
            if (!this->access_callback(reg_offset, size, value, is_write))
            {
                this->update(reg_offset, size, value, is_write);
            }
        }

    protected:
        uint64_t reset_val;

    private:
        uint64_t value;
    };


    class regmap {
    public:
        std::vector<reg *> get_registers() { return this->registers; }
        void build(vp::component *comp, vp::trace *trace, std::string name="");
        bool access(uint64_t offset, int size, uint8_t *value, bool is_write);
        void reset(bool active);

        vp::trace *trace;

    protected:
        std::vector<reg *> registers;
        vp::component *comp;
    };
};