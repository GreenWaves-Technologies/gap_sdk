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

#ifndef __VP_ITF_IMPLEM_WIRE_CLASS_HPP__
#define __VP_ITF_IMPLEM_WIRE_CLASS_HPP__

#include "vp/vp.hpp"

namespace vp {

  class component;

  template<class T>
  class wire_slave;

  template<class T>
  class wire_master : public master_port
  {
    friend class wire_slave<T>;
  public:

    wire_master();

    void set_sync_meth(void (*)(void *_this, T value));
    void set_sync_meth_muxed(void (*)(void *_this, T value, int), int id);

    inline void sync(T value)
    {
      if (next) next->sync(value);
      sync_meth(this->get_remote_context(), value);
    }

    inline void sync_back(T *value)
    {
      if (next) next->sync_back(value);
      sync_back_meth(this->get_remote_context(), value);
    }

    void bind_to(vp::port *port, vp::config *config);

    bool is_bound() { return slave_port != NULL; }

    void finalize();

  private:
    static inline void sync_muxed(wire_master *_this, T value);
    static inline void sync_freq_cross_stub(wire_master *_this, T value);
    static inline void sync_back_freq_cross_stub(wire_master *_this, T *value);
    static inline void sync_back_muxed(wire_master *_this, T *value);
    void (*sync_meth)(void *, T value);
    void (*sync_meth_mux)(void *, T value, int id);
    void (*sync_back_meth)(void *, T *value);
    void (*sync_back_meth_mux)(void *, T *value, int id);

    void (*sync_meth_freq_cross)(void *, T value);
    void (*sync_back_meth_freq_cross)(void *, T *value);

    void (*master_sync_meth)(void *comp, T value);
    void (*master_sync_meth_mux)(void *comp, T value, int id);

    // Default sync callback, just do nothing.
    static inline void sync_default(void *, T value) {}

    vp::component *comp_mux;
    int sync_mux;
    int sync_back_mux;
    wire_slave<T> *slave_port = NULL;
    wire_master<T> *next = NULL;

    void *slave_context_for_freq_cross;

    int master_sync_mux_id;
  };



  template<class T>
  class wire_slave : public slave_port
  {

    friend class wire_master<T>;

  public:

    inline wire_slave();

    inline void sync(T value)
    {
      this->master_sync_meth(this->get_remote_context(), value);
    }

    void set_sync_meth(void (*)(void *_this, T value));
    void set_sync_meth_muxed(void (*)(void *_this, T value, int), int id);

    void set_sync_back_meth(void (*)(void *_this, T *value));
    void set_sync_back_meth_muxed(void (*)(void *_this, T *value, int), int id);

    inline void bind_to(vp::port *_port, vp::config *config);

    bool is_bound() { return master_port != NULL; }


  private:

    static inline void sync_muxed(wire_slave *_this, T value);
    
    static inline void sync_muxed_stub(wire_slave *_this, T value);

    void (*sync_meth)(void *comp, T value);
    void (*sync_meth_mux)(void *comp, T value, int id);

    void (*sync_back)(void *comp, T *value);
    void (*sync_back_mux)(void *comp, T *value, int id);

    void (*master_sync_meth)(void *comp, T value);
    void (*master_sync_meth_mux)(void *comp, T value, int id);

    int sync_mux_id;

    int sync_back_mux_id;

    vp::component *master_comp_mux;
    int master_sync_mux;

    wire_master<T> *master_port = NULL;
  };

};

#endif
