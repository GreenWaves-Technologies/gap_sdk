/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
 *
 * Licensed under the Apache License, Versi2cn 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITi2cNS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissi2cns and
 * limitati2cns under the License.
 */

/* 
 * Authors: Germain Haugou, ETH (germain.haugou@iis.ee.ethz.ch)
 */

#ifndef __VP_ITF_I2C_HPP__
#define __VP_ITF_I2C_HPP__

#include "vp/vp.hpp"

namespace vp {



  class i2c_slave;



  typedef void (i2c_sync_meth_t)(void *, int scl, int sda);
  typedef void (i2c_sync_cycle_meth_t)(void *, int sda);

  typedef void (i2c_sync_meth_muxed_t)(void *, int scl, int sda, int id);
  typedef void (i2c_sync_cycle_meth_muxed_t)(void *, int sda, int id);

  typedef void (i2c_slave_sync_meth_t)(void *, int sda);
  typedef void (i2c_slave_sync_meth_muxed_t)(void *, int sda, int id);



  class i2c_master : public vp::master_port
  {
    friend class i2c_slave;

  public:

    inline i2c_master();

    inline void sync(int scl, int sda)
    {
      return sync_meth(this->get_remote_context(), scl, sda);
    }

    inline void sync_cycle(int sda)
    {
      return sync_cycle_meth(this->get_remote_context(), sda);
    }

    void bind_to(vp::port *port, vp::config *config);

    inline void set_sync_meth(i2c_slave_sync_meth_t *meth);

    inline void set_sync_meth_muxed(i2c_slave_sync_meth_muxed_t *meth, int id);

    bool is_bound() { return slave_port != NULL; }

  private:

    static inline void sync_muxed_stub(i2c_master *_this, int scl, int sda);
    static inline void sync_cycle_muxed_stub(i2c_master *_this, int sda);

    void (*slave_sync)(void *comp, int sda);
    void (*slave_sync_mux)(void *comp, int sda, int id);

    void (*sync_meth)(void *, int scl, int sda);
    void (*sync_meth_mux)(void *, int scl, int sda, int mux);
    void (*sync_cycle_meth)(void *, int sda);
    void (*sync_cycle_meth_mux)(void *, int sda, int mux);

    static inline void sync_default(void *, int sda);


    vp::component *comp_mux;
    int sync_mux;
    i2c_slave *slave_port = NULL;

    int mux_id;
  };



  class i2c_slave : public vp::slave_port
  {

    friend class i2c_master;

  public:

    inline i2c_slave();

    inline void sync(int sda)
    {
      slave_sync_meth(this->get_remote_context(), sda);
    }

    inline void set_sync_meth(i2c_sync_meth_t *meth);
    inline void set_sync_meth_muxed(i2c_sync_meth_muxed_t *meth, int id);

    inline void set_sync_cycle_meth(i2c_sync_cycle_meth_t *meth);
    inline void set_sync_cycle_meth_muxed(i2c_sync_cycle_meth_muxed_t *meth, int id);

    inline void bind_to(vp::port *_port, vp::config *config);

  private:

    static inline void sync_muxed_stub(i2c_slave *_this, int sda);


    void (*slave_sync_meth)(void *, int sda);
    void (*slave_sync_meth_mux)(void *, int sda, int mux);

    void (*sync_meth)(void *comp, int scl, int sda);
    void (*sync_mux_meth)(void *comp, int scl, int sda, int mux);
    void (*sync_cycle_meth)(void *comp, int sda);
    void (*sync_cycle_mux_meth)(void *comp, int sda, int mux);

    static inline void sync_default(i2c_slave *, int scl, int sda);
    static inline void sync_cycle_default(i2c_slave *, int sda);

    vp::component *comp_mux;
    int sync_mux;
    int mux_id;


  };


  inline i2c_master::i2c_master() {
    slave_sync = &i2c_master::sync_default;
    slave_sync_mux = NULL;
  }



  inline void i2c_master::sync_muxed_stub(i2c_master *_this, int scl, int sda)
  {
    return _this->sync_meth_mux(_this->comp_mux, scl, sda, _this->sync_mux);
  }



  inline void i2c_master::sync_cycle_muxed_stub(i2c_master *_this, int sda)
  {
    return _this->sync_cycle_meth_mux(_this->comp_mux, sda, _this->sync_mux);
  }



  inline void i2c_master::bind_to(vp::port *_port, vp::config *config)
  {
    i2c_slave *port = (i2c_slave *)_port;
    if (port->sync_mux_meth == NULL)
    {
      sync_meth = port->sync_meth;
      sync_cycle_meth = port->sync_cycle_meth;
      this->set_remote_context(port->get_context());
    }
    else
    {
      sync_meth_mux = port->sync_mux_meth;
      sync_meth = (i2c_sync_meth_t *)&i2c_master::sync_muxed_stub;

      sync_cycle_meth_mux = port->sync_cycle_mux_meth;
      sync_cycle_meth = (i2c_sync_cycle_meth_t *)&i2c_master::sync_cycle_muxed_stub;

      this->set_remote_context(this);
      comp_mux = (vp::component *)port->get_context();
      sync_mux = port->mux_id;
    }
  }

  inline void i2c_master::set_sync_meth(i2c_slave_sync_meth_t *meth)
  {
    slave_sync = meth;
  }

  inline void i2c_master::set_sync_meth_muxed(i2c_slave_sync_meth_muxed_t *meth, int id)
  {
    slave_sync_mux = meth;
    slave_sync = NULL;
    mux_id = id;
  }

  inline void i2c_master::sync_default(void *, int sda)
  {
  }

  inline void i2c_slave::sync_muxed_stub(i2c_slave *_this, int sda)
  {
    return _this->slave_sync_meth_mux(_this->comp_mux, sda, _this->sync_mux);
  }

  inline void i2c_slave::bind_to(vp::port *_port, vp::config *config)
  {
    slave_port::bind_to(_port, config);
    i2c_master *port = (i2c_master *)_port;
    port->slave_port = this;
    if (port->slave_sync_mux == NULL)
    {
      this->slave_sync_meth = port->slave_sync;
      this->set_remote_context(port->get_context());
    }
    else
    {
      this->slave_sync_meth_mux = port->slave_sync_mux;
      this->slave_sync_meth = (i2c_sync_cycle_meth_t *)&i2c_slave::sync_muxed_stub;

      set_remote_context(this);
      comp_mux = (vp::component *)port->get_context();
      sync_mux = port->mux_id;
    }
  }

  inline i2c_slave::i2c_slave() : sync_meth(NULL), sync_mux_meth(NULL) {
    sync_meth = (i2c_sync_meth_t *)&i2c_slave::sync_default;
    sync_cycle_meth = (i2c_sync_cycle_meth_t *)&i2c_slave::sync_cycle_default;
  }

  inline void i2c_slave::set_sync_meth(i2c_sync_meth_t *meth)
  {
    sync_meth = meth;
    sync_mux_meth = NULL;
  }

  inline void i2c_slave::set_sync_cycle_meth(i2c_sync_cycle_meth_t *meth)
  {
    sync_cycle_meth = meth;
    sync_cycle_mux_meth = NULL;
  }

  inline void i2c_slave::set_sync_meth_muxed(i2c_sync_meth_muxed_t *meth, int id)
  {
    sync_mux_meth = meth;
    sync_meth = NULL;
    mux_id = id;
  }

  inline void i2c_slave::set_sync_cycle_meth_muxed(i2c_sync_cycle_meth_muxed_t *meth, int id)
  {
    sync_cycle_mux_meth = meth;
    sync_cycle_meth = NULL;
    mux_id = id;
  }

  inline void i2c_slave::sync_default(i2c_slave *, int scl, int sda)
  {
  }

  inline void i2c_slave::sync_cycle_default(i2c_slave *, int sda)
  {
  }




};

#endif
