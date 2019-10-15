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

#ifndef __VP_ITF_CLOCK_HPP__
#define __VP_ITF_CLOCK_HPP__

#include "vp/vp.hpp"

namespace vp {

  class component;

  class clock_slave;

  class clock_master : public master_port
  {
    friend class clock_slave;
  public:

    clock_master();

    inline void sync(bool value)
    {
      if (next) next->sync(value);
      sync_meth(this->get_remote_context(), value);
    }

    inline void set_frequency(int64_t frequency)
    {
      if (next) next->set_frequency(frequency);
      set_frequency_meth(this->get_remote_context(), frequency);
    }

    void bind_to(vp::port *port, vp::config *config);

    bool is_bound() { return slave_port != NULL; }

    void finalize();

  private:
    static inline void sync_muxed(clock_master *_this, bool value);
    static inline void set_frequency_muxed(clock_master *_this, int64_t frequency);
    static inline void sync_freq_cross_stub(clock_master *_this, bool value);

    static inline void sync_default(void *, bool value);
    static inline void set_frequency_default(void *, int64_t value);
    static inline void set_frequency_freq_cross_stub(clock_master *_this, int64_t value);

    void (*sync_meth)(void *, bool value);
    void (*sync_meth_mux)(void *, bool value, int id);
    void (*sync_meth_freq_cross)(void *, bool value);

    void (*set_frequency_meth)(void *, int64_t frequency);
    void (*set_frequency_meth_mux)(void *, int64_t frequency, int id);
    void (*set_frequency_meth_freq_cross)(void *, int64_t value);

    vp::component *comp_mux;
    int sync_mux;
    clock_slave *slave_port = NULL;
    clock_master *next = NULL;

    void *slave_context_for_freq_cross;
  };


  class clock_slave : public slave_port
  {

    friend class clock_master;

  public:

    inline clock_slave();

    void set_sync_meth(void (*)(void *_this, bool value));
    void set_sync_meth_muxed(void (*)(void *_this, bool value, int), int id);

    void set_set_frequency_meth(void (*)(void *_this, int64_t frequency));
    void set_set_frequency_meth_muxed(void (*)(void *_this, int64_t, int), int id);

    inline void bind_to(vp::port *_port, vp::config *config);


  private:

    void (*sync)(void *comp, bool value);
    void (*sync_mux)(void *comp, bool value, int id);

    void (*set_frequency)(void *comp, int64_t frequency);
    void (*set_frequency_mux)(void *comp, int64_t frequency, int id);

    int sync_mux_id;
  };

};

#include "vp/itf/implem/clock.hpp"

#endif
