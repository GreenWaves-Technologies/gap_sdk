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

#ifndef __VP_ITF_IMPLEMEN_CLOCK_HPP__
#define __VP_ITF_IMPLEMEN_CLOCK_HPP__

namespace vp {

  inline clock_master::clock_master()
  {
    this->sync_meth = &clock_master::sync_default;
    this->set_frequency_meth = &clock_master::set_frequency_default;
  }

  inline void clock_master::bind_to(vp::port *_port, vp::config *config)
  {
    this->remote_port = _port;

    if (slave_port != NULL)
    {
      // Case where a slave port is already connected.
      // Just connect the new one to the list so that all are called.
      clock_master *master = new clock_master;
      master->bind_to(_port, config);
      master->next = this->next;
      this->next = master;
    }
    else
    {
      clock_slave *port = (clock_slave *)_port;
      if (port->sync_mux == NULL)
      {
        sync_meth = port->sync;
        set_frequency_meth = port->set_frequency;
        set_remote_context(port->get_context());
      }
      else
      {
        sync_meth_mux = port->sync_mux;
        sync_meth = (void (*)(void *, bool))&clock_master::sync_muxed;
        set_frequency_meth_mux = port->set_frequency_mux;
        set_frequency_meth = (void (*)(void *, int64_t))&clock_master::set_frequency_muxed;
        set_remote_context(this);
        comp_mux = (vp::component *)port->get_context();
        sync_mux = port->sync_mux_id;
      }

    }
  }

  inline void clock_master::sync_default(void *, bool value)
  {
  }

  inline void clock_master::set_frequency_default(void *, int64_t value)
  {
  }

  inline void clock_master::sync_muxed(clock_master *_this, bool value)
  {
    return _this->sync_meth_mux(_this->comp_mux, value, _this->sync_mux);
  }

  inline void clock_master::sync_freq_cross_stub(clock_master *_this, bool value)
  {
    // The normal callback was tweaked in order to get there when the master is sending a
    // request. 
    // First synchronize the target engine in case it was left behind,
    // and then generate the normal call with the mux ID using the saved handler
    _this->remote_port->get_owner()->get_clock()->sync();
    return _this->sync_meth_freq_cross((component *)_this->slave_context_for_freq_cross, value);
  }

  inline void clock_master::set_frequency_freq_cross_stub(clock_master *_this, int64_t value)
  {
    // The normal callback was tweaked in order to get there when the master is sending a
    // request. 
    // First synchronize the target engine in case it was left behind,
    // and then generate the normal call with the mux ID using the saved handler
    if (_this->remote_port->get_owner()->get_clock())
      _this->remote_port->get_owner()->get_clock()->sync();
    return _this->set_frequency_meth_freq_cross((component *)_this->slave_context_for_freq_cross, value);
  }

  inline void clock_master::set_frequency_muxed(clock_master *_this, int64_t frequency)
  {
    return _this->set_frequency_meth_mux(_this->comp_mux, frequency, _this->sync_mux);
  }

  inline void clock_master::finalize()
  {
    // We have to instantiate a stub in case the binding is crossing different
    // frequency domains in order to resynchronize the target engine.
    if (this->get_owner()->get_clock() != this->remote_port->get_owner()->get_clock())
    {
      // Just save the normal handler and tweak it to enter the stub when the
      // master is pushing the request.
      this->sync_meth_freq_cross = this->sync_meth;
      this->sync_meth = (void (*)(void *, bool))&clock_master::sync_freq_cross_stub;

      this->set_frequency_meth_freq_cross = this->set_frequency_meth;
      this->set_frequency_meth = (void (*)(void *, int64_t))&clock_master::set_frequency_freq_cross_stub;

      this->slave_context_for_freq_cross = this->get_remote_context();
      this->set_remote_context(this);
    }
  }



  inline void clock_slave::bind_to(vp::port *_port, vp::config *config)
  {
    slave_port::bind_to(_port, config);
    clock_master *port = (clock_master *)_port;

    if (port->next) port = port->next;

    clock_slave *slave_port = new clock_slave();

    port->slave_port = NULL;

    port->slave_port = slave_port;
    port->slave_port->set_context(port->get_context());
    port->slave_port->set_remote_context(port->get_context());
  }

  inline void clock_slave::set_sync_meth(void (*meth)(void *, bool))
  {
    sync = meth;
    sync_mux = NULL;
  }

  inline void clock_slave::set_sync_meth_muxed(void (*meth)(void *, bool, int), int id)
  {
    sync = NULL;
    sync_mux = meth;
    sync_mux_id = id;
  }

  inline void clock_slave::set_set_frequency_meth(void (*meth)(void *, int64_t))
  {
    set_frequency = meth;
    set_frequency_mux = NULL;
  }

  inline void clock_slave::set_set_frequency_meth_muxed(void (*meth)(void *, int64_t, int), int id)
  {
    set_frequency = NULL;
    set_frequency_mux = meth;
    sync_mux_id = id;
  }

  inline clock_slave::clock_slave() : sync(NULL), sync_mux(NULL), set_frequency(NULL), set_frequency_mux(NULL)
  {
    this->sync = &clock_master::sync_default;
    this->set_frequency = &clock_master::set_frequency_default;
  }

};

#endif