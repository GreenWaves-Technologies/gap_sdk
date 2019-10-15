/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#ifndef __VP_COMPONENT_HPP__
#define __VP_COMPONENT_HPP__

#include "vp/vp_data.hpp"
#include <map>
#include <list>
#include <string>
#include <vector>
#include <functional>
#include "vp/ports.hpp"
#include "vp/config.hpp"
#include "vp/clock/clock_event.hpp"
#include "vp/itf/clk.hpp"
#include "vp/clock/component_clock.hpp"
#include "vp/trace/component_trace.hpp"
#include "vp/power/component_power.hpp"
#include "json.hpp"
#include <functional>


#define   likely(x) __builtin_expect(x, 1)
#define unlikely(x) __builtin_expect(x, 0)

using namespace std;

#define VP_ERROR_SIZE (1<<16)
extern char vp_error[];

namespace vp {

  class config;
  class clock_engine;
  class component;

  class reg
  {

    public:
      void init(vp::component *top, std::string name, int bits, uint8_t *value, uint8_t *reset_val);
      void reset(bool active);

      inline uint8_t *get_bytes() { return this->value_bytes; }
      inline void set_1(uint8_t value) { *(uint8_t *)this->value_bytes = value; }
      inline void set_8(uint8_t value) { *(uint8_t *)this->value_bytes = value; }
      inline void set_16(uint16_t value) { *(uint16_t *)this->value_bytes = value; }
      inline void set_32(uint32_t value) { *(uint32_t *)this->value_bytes = value; }
      inline void set_64(uint64_t value) { *(uint64_t *)this->value_bytes = value; }

      inline void read(int reg_offset, int size, uint8_t *value) { memcpy((void *)value, (void *)(this->value_bytes+reg_offset), size); }
      inline void read(uint8_t *value) { memcpy((void *)value, (void *)this->value_bytes, this->nb_bytes); }
      inline uint8_t  get_1 () { return *(uint8_t *)this->value_bytes; }
      inline uint8_t  get_8 () { return *(uint8_t *)this->value_bytes; }
      inline uint16_t get_16() { return *(uint16_t *)this->value_bytes; }
      inline uint32_t get_32() { return *(uint32_t *)this->value_bytes; }
      inline uint64_t get_64() { return *(uint64_t *)this->value_bytes; }

      int nb_bytes;
      int bits;
      uint8_t *reset_value_bytes;
      uint8_t *value_bytes;
      std::string name;
      component *top;
      vp::trace trace;
      vp::trace reg_event;
  };

  class reg_1: public reg
  {
  public:
    void init(vp::component *top, std::string name, uint8_t *reset_val);

    inline uint8_t get() { return this->value; }
    inline void set(uint8_t value) {
      this->trace.msg("Setting register (value: 0x%x)\n", value); this->value = value;
      if (this->reg_event.get_event_active())
        this->reg_event.event((uint8_t *)&this->value);
    }
    inline void write(uint8_t *value) {
      memcpy((void *)this->value_bytes, (void *)value, this->nb_bytes);
      if (this->reg_event.get_event_active())
        this->reg_event.event((uint8_t *)this->value_bytes);
    }
    inline void write(int reg_offset, int size, uint8_t *value) {
      memcpy((void *)(this->value_bytes+reg_offset), (void *)value, size); this->dump_after_write();
      if (this->reg_event.get_event_active())
        this->reg_event.event((uint8_t *)this->value_bytes);
    }
    inline void dump_after_write() { this->trace.msg("Modified register (value: 0x%x)\n", this->value); }
    inline void access(int reg_offset, int size, uint8_t *value, bool is_write)
    {
      if (is_write)
        this->write(reg_offset, size, value);
      else
        this->read(reg_offset, size, value);
    }

  private:
    uint8_t value;

  };

  class reg_8: public reg
  {
  public:
    void init(vp::component *top, std::string name, uint8_t *reset_val);

    inline uint8_t get() { return this->value; }
    inline void set(uint8_t value) {
      this->value = value;
      if (this->reg_event.get_event_active())
        this->reg_event.event((uint8_t *)&this->value);
    }
    inline void set_field(uint8_t value, int offset, int width) {
      this->value = (this->value & (((1<<width)-1)<<offset)) | (value << offset);
      if (this->reg_event.get_event_active())
        this->reg_event.event((uint8_t *)&this->value);
    }
    inline uint8_t get_field(int offset, int width) { return (this->value >> offset) & ((1<<width)-1); }
    inline void write(uint8_t *value) {
      memcpy((void *)this->value_bytes, (void *)value, this->nb_bytes);
      if (this->reg_event.get_event_active())
        this->reg_event.event((uint8_t *)this->value_bytes);
    }
    inline void write(int reg_offset, int size, uint8_t *value) {
      memcpy((void *)(this->value_bytes+reg_offset), (void *)value, size); this->dump_after_write();
      if (this->reg_event.get_event_active())
        this->reg_event.event((uint8_t *)this->value_bytes);
    }
    inline void dump_after_write() { this->trace.msg("Modified register (value: 0x%x)\n", this->value); }
    inline void access(int reg_offset, int size, uint8_t *value, bool is_write)
    {
      if (is_write)
        this->write(reg_offset, size, value);
      else
        this->read(reg_offset, size, value);
    }

  private:
    uint8_t value;

  };

  class reg_16: public reg
  {
  public:
    void init(vp::component *top, std::string name, uint8_t *reset_val);

    inline uint16_t get() { return this->value; }
    inline void set(uint16_t value) {
      this->value = value;
      if (this->reg_event.get_event_active())
        this->reg_event.event((uint8_t *)&this->value);
    }
    inline void set_field(uint16_t value, int offset, int width) {
      this->value = (this->value & (((1<<width)-1)<<offset)) | (value << offset);
      if (this->reg_event.get_event_active())
        this->reg_event.event((uint8_t *)&this->value);
    }
    inline uint16_t get_field(int offset, int width) { return (this->value >> offset) & ((1<<width)-1); }
    inline void write(uint8_t *value) {
      memcpy((void *)this->value_bytes, (void *)value, this->nb_bytes);
      if (this->reg_event.get_event_active())
        this->reg_event.event((uint8_t *)this->value_bytes);
    }
    inline void write(int reg_offset, int size, uint8_t *value) {
      memcpy((void *)(this->value_bytes+reg_offset), (void *)value, size); this->dump_after_write();
      if (this->reg_event.get_event_active())
        this->reg_event.event((uint8_t *)this->value_bytes);
    }
    inline void dump_after_write() { this->trace.msg("Modified register (value: 0x%x)\n", this->value); }
    inline void access(int reg_offset, int size, uint8_t *value, bool is_write)
    {
      if (is_write)
        this->write(reg_offset, size, value);
      else
        this->read(reg_offset, size, value);
    }

  private:
    uint16_t value;

  };

  class reg_32: public reg
  {
  public:
    void init(vp::component *top, std::string name, uint8_t *reset_val);

    inline uint32_t get() { return this->value; }
    inline void set(uint32_t value) {
      this->value = value;
      if (this->reg_event.get_event_active())
        this->reg_event.event((uint8_t *)&this->value);
    }
    inline void set_field(uint32_t value, int offset, int width) {
      this->value = (this->value & (((1<<width)-1)<<offset)) | (value << offset);
      if (this->reg_event.get_event_active())
        this->reg_event.event((uint8_t *)&this->value);
    }
    inline uint32_t get_field(int offset, int width) { return (this->value >> offset) & ((1<<width)-1); }
    inline void write(uint8_t *value) {
      memcpy((void *)this->value_bytes, (void *)value, this->nb_bytes);
      if (this->reg_event.get_event_active())
        this->reg_event.event((uint8_t *)this->value_bytes);
    }
    inline void write(int reg_offset, int size, uint8_t *value) {
      memcpy((void *)(this->value_bytes+reg_offset), (void *)value, size); this->dump_after_write();
      if (this->reg_event.get_event_active())
        this->reg_event.event((uint8_t *)this->value_bytes);
    }
    inline void dump_after_write() { this->trace.msg("Modified register (value: 0x%x)\n", this->value); }
    inline void access(int reg_offset, int size, uint8_t *value, bool is_write)
    {
      if (is_write)
        this->write(reg_offset, size, value);
      else
        this->read(reg_offset, size, value);
    }

  private:
    uint32_t value;

  };

  class reg_64: public reg
  {
  public:
    void init(vp::component *top, std::string name, uint8_t *reset_val);

    inline uint64_t get() { return this->value; }
    inline void set(uint64_t value) {
      this->value = value;
      if (this->reg_event.get_event_active())
        this->reg_event.event((uint8_t *)&this->value);
    }
    inline void write(uint8_t *value) {
      memcpy((void *)this->value_bytes, (void *)value, this->nb_bytes);
      if (this->reg_event.get_event_active())
        this->reg_event.event((uint8_t *)this->value_bytes);
    }
    inline void write(int reg_offset, int size, uint8_t *value) {
      memcpy((void *)(this->value_bytes+reg_offset), (void *)value, size); this->dump_after_write();
      if (this->reg_event.get_event_active())
        this->reg_event.event((uint8_t *)this->value_bytes);
    }
    inline void dump_after_write() { this->trace.msg("Modified register (value: 0x%x)\n", this->value); }
    inline void access(int reg_offset, int size, uint8_t *value, bool is_write)
    {
      if (is_write)
        this->write(reg_offset, size, value);
      else
        this->read(reg_offset, size, value);
    }

  private:
    uint64_t value;

  };

  class component : public component_clock
  {

    friend class component_clock;

  public:
    component(const char *config);

    virtual int build() { return 0; }
    virtual void pre_start() {}
    virtual void start() {}
    virtual void stop() {}
    virtual void pre_reset() {}
    virtual void reset(bool active) {}
    virtual void load() {}
    virtual void elab();
    virtual string run() { return "error"; }
    virtual int run_status() { return 0; }


    void set_config(const char *config);

    inline js::config *get_js_config() { return comp_js_config; }

    inline config *get_config(std::string name);


    inline int get_config_int(std::string name, int index);

    inline int get_config_int(std::string name);

    inline bool get_config_bool(std::string name);

    inline std::string get_config_str(std::string name);

    inline int64_t get_time();

    virtual vp::time_engine *get_time_engine() ;

    string get_path() { return path; }


    void conf(string path, vp::component *parent);

    void add_child(vp::component *child);

    config *import_config(const char *config_string);

    void reg_step_pre_start(std::function<void()> callback);

    void post_post_build();

    void pre_build() {
      component_clock::pre_build(this);
    }

    void pre_start_all();

    void reset_all(bool active, bool from_itf=false);

    void new_master_port(std::string name, master_port *port);

    void new_master_port(void *comp, std::string name, master_port *port);

    void new_slave_port(std::string name, slave_port *port);

    void new_slave_port(void *comp, std::string name, slave_port *port);

    void new_service(std::string name, void *service);


    int get_ports(bool master, int size, const char *names[], void *ports[]);

    int get_services(int size, const char *names[], void *services[]);

    void *get_service(string name);

    void set_services(int nb_services, const char *name[], void *services[]);

    void new_reg_any(std::string name, vp::reg *reg, int bits, uint8_t *reset_val);
    void new_reg(std::string name, vp::reg_1 *reg, uint8_t reset_val, bool reset=true);
    void new_reg(std::string name, vp::reg_8 *reg, uint8_t reset_val, bool reset=true);
    void new_reg(std::string name, vp::reg_16 *reg, uint16_t reset_val, bool reset=true);
    void new_reg(std::string name, vp::reg_32 *reg, uint32_t reset_val, bool reset=true);
    void new_reg(std::string name, vp::reg_64 *reg, uint64_t reset_val, bool reset=true);

    inline trace *get_trace() { return &this->root_trace; }

    std::vector<vp::component *> get_childs() { return childs; }

    component_trace traces;
    component_power power;

    trace warning;

  protected:
    template<typename P> int get_ports(std::map<std::string, P *> ports_map,
      int size, const char *names[], void *ports[]);

    std::map<std::string, master_port *> master_ports;
    std::map<std::string, slave_port *> slave_ports;
    std::map<std::string, void *> services;
    std::map<std::string, void *> all_services;
 
    std::vector<component *> childs;

  private:

    js::config *comp_js_config;
    trace root_trace;


    string path;

    component *parent = NULL;

    vector<std::function<void()>> pre_start_callbacks;
    vector<vp::reg *> regs;

    bool reset_done_from_itf;

    time_engine *time_engine_ptr = NULL;
  };

};  

#endif
