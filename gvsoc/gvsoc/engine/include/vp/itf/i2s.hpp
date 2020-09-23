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

#ifndef __VP_ITF_I2S_HPP__
#define __VP_ITF_I2S_HPP__

#include "vp/vp.hpp"

namespace vp {



  class i2s_slave;



  typedef void (i2s_sync_meth_t)(void *, int sck, int ws, int sd);
  typedef void (i2s_sync_meth_muxed_t)(void *, int sck, int ws, int sd, int id);



  class i2s_master : public vp::master_port
  {
    friend class i2s_slave;

  public:

    inline i2s_master();

    inline void sync(int sck, int ws, int sd);

    void bind_to(vp::port *port, vp::config *config);

    inline void set_sync_meth(i2s_sync_meth_t *meth);
    inline void set_sync_meth_muxed(i2s_sync_meth_muxed_t *meth, int id);

    bool is_bound() { return slave_port != NULL; }

  private:

    static inline void sync_muxed_stub(i2s_master *_this, int sck, int ws, int sd);

    void (*slave_sync)(void *comp, int sck, int ws, int sd);
    void (*slave_sync_mux)(void *comp, int sck, int ws, int sd, int mux);

    void (*sync_meth)(void *, int sck, int ws, int sd);
    void (*sync_meth_mux)(void *, int sck, int ws, int sd, int mux);

    static inline void sync_default(void *, int sck, int ws, int sd);

    vp::component *comp_mux;
    int sync_mux;
    i2s_slave *slave_port = NULL;
    int mux_id;

    int sd;

  };



  class i2s_slave : public vp::slave_port
  {

    friend class i2s_master;

  public:

    inline i2s_slave();

    inline void sync(int sck, int ws, int sd)
    {
      if (next) next->sync(sck, ws, sd);
      slave_sync_meth(this->get_remote_context(), sck, ws, sd);
    }

    inline void set_sync_meth(i2s_sync_meth_t *meth);
    inline void set_sync_meth_muxed(i2s_sync_meth_muxed_t *meth, int id);

    inline void bind_to(vp::port *_port, vp::config *config);

    bool is_bound() { return remote_port != NULL; }

    inline int get_sd();

  private:

    static inline void sync_muxed_stub(i2s_slave *_this, int sck, int ws, int sd);

    void (*slave_sync_meth)(void *, int sck, int ws, int sd);
    void (*slave_sync_meth_mux)(void *, int sck, int ws, int sd, int mux);

    void (*sync_meth)(void *comp, int sck, int ws, int sd);
    void (*sync_mux_meth)(void *comp, int sck, int ws, int sd, int mux);

    static inline void sync_default(i2s_slave *, int sck, int ws, int sd);

    vp::component *comp_mux;
    int sync_mux;
    int mux_id;
    i2s_master *master_port = NULL;
    i2s_slave *next = NULL;

  };


  inline i2s_master::i2s_master() {
    slave_sync = &i2s_master::sync_default;
    slave_sync_mux = NULL;
    this->sd = 2;
  }




  inline void i2s_master::sync_muxed_stub(i2s_master *_this, int sck, int ws, int sd)
  {
    return _this->sync_meth_mux(_this->comp_mux, sck, ws, sd, _this->sync_mux);
  }

  inline void i2s_master::bind_to(vp::port *_port, vp::config *config)
  {
    i2s_slave *port = (i2s_slave *)_port;
    if (port->sync_mux_meth == NULL)
    {
      sync_meth = port->sync_meth;
      set_remote_context(port->get_context());
    }
    else
    {
      sync_meth_mux = port->sync_mux_meth;
      sync_meth = (i2s_sync_meth_t *)&i2s_master::sync_muxed_stub;

      set_remote_context(this);
      comp_mux = (vp::component *)port->get_context();
      sync_mux = port->mux_id;
    }
  }

  inline void i2s_master::set_sync_meth(i2s_sync_meth_t *meth)
  {
    slave_sync = meth;
  }

  inline void i2s_master::set_sync_meth_muxed(i2s_sync_meth_muxed_t *meth, int id)
  {
    slave_sync_mux = meth;
    slave_sync = NULL;
    mux_id = id;
  }

  inline void i2s_master::sync_default(void *, int sck, int ws, int sd)
  {
  }

  inline void i2s_master::sync(int sck, int ws, int sd)
  {
    this->sd = sd;
    return sync_meth(this->get_remote_context(), sck, ws, this->slave_port->get_sd());
  }

  inline void i2s_slave::sync_muxed_stub(i2s_slave *_this, int sck, int ws, int sd)
  {
    return _this->slave_sync_meth_mux(_this->comp_mux, sck, ws, sd, _this->sync_mux);
  }

  inline int i2s_slave::get_sd()
  {
    int sd = this->master_port->sd;
    i2s_slave *current = this->next;

    while(current)
    {
      int next_sd = current->master_port->sd;
      if (next_sd != 2)
      {
        if (sd == 2 || next_sd == sd)
          sd = next_sd;
        else
          sd = 3;
      }
      current = current->next;
    }

    return sd;
  }

  inline void i2s_slave::bind_to(vp::port *_port, vp::config *config)
  {
    i2s_master *port = (i2s_master *)_port;

    if (this->master_port != NULL)
    {
      i2s_slave *slave = new i2s_slave;
      port->slave_port = slave;
      slave->bind_to(_port, config);
      slave->next = this->next;
      this->next = slave;
    }
    else
    {
      slave_port::bind_to(_port, config);
      this->master_port = port;
      port->slave_port = this;
      if (port->slave_sync_mux == NULL)
      {
        this->slave_sync_meth = port->slave_sync;
        this->set_remote_context(port->get_context());
      }
      else
      {
        this->slave_sync_meth_mux = port->slave_sync_mux;
        this->slave_sync_meth = (i2s_sync_meth_t *)&i2s_slave::sync_muxed_stub;

        set_remote_context(this);
        comp_mux = (vp::component *)port->get_context();
        sync_mux = port->mux_id;
      }
    }
  }

  inline i2s_slave::i2s_slave() : sync_meth(NULL), sync_mux_meth(NULL) {
    sync_meth = (i2s_sync_meth_t *)&i2s_slave::sync_default;
  }

  inline void i2s_slave::set_sync_meth(i2s_sync_meth_t *meth)
  {
    sync_meth = meth;
    sync_mux_meth = NULL;
  }

  inline void i2s_slave::set_sync_meth_muxed(i2s_sync_meth_muxed_t *meth, int id)
  {
    sync_mux_meth = meth;
    sync_meth = NULL;
    mux_id = id;
  }

  inline void i2s_slave::sync_default(i2s_slave *, int sck, int ws, int sd)
  {
  }



};

#endif
