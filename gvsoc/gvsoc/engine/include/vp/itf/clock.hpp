/*
 * Copyright (C) 2020  GreenWaves Technologies, SAS
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/* 
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
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
