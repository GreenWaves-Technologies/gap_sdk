/* vim: set ts=2 sw=2 expandtab: */
/*
 * Copyright (C) 2019 TU Kaiserslautern
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
 * Author: Éder F. Zulian, TUK (zulian@eit.uni-kl.de)
 */

#ifndef __EMS_GEM5_TLM_BR_H__
#define __EMS_GEM5_TLM_BR_H__

#include <regex>
#include <iostream>
#include <fstream>

#include <tlm.h>

#include "ems_common.h"
#include "ems_addr_adapter.h"

#include "report_handler.hh"
#include "sc_target.hh"
#include "sim_control.hh"
#include "slave_transactor.hh"
#include "stats.hh"

namespace ems {

typedef void (*cb_func)(void);
class gem5_sim_ctrl : public Gem5SystemC::Gem5SimControl
{
public:
  // name:  Gem5SystemC::Gem5SimControl instance SystemC module name
  // g5cfg: gem5 configuration file
  // ticks: zero to simulate until workload is finished, otherwise the
  //        specified amount of time
  // g5dbg: gem5 debug flags
  // bs_cb: callback to be called before simulation
  // as_cb: callback to be called after simulation
  gem5_sim_ctrl(std::string name,
                std::string g5cfg,
                uint64_t ticks = 0,
                std::string g5dbg = "MemoryAccess",
                cb_func bs_cb = nullptr,
                cb_func as_cb = nullptr) :
    Gem5SystemC::Gem5SimControl(name.c_str(), g5cfg, ticks, g5dbg),
    before_sim_cb(bs_cb),
    after_sim_cb(as_cb)
  {
    debug(this->name() << " config file: " << g5cfg);
    debug(this->name() << " ticks: " << ticks);
    debug(this->name() << " debug flags: " << g5dbg);
  }

private:
  void beforeSimulate()
  {
    if (before_sim_cb) {
      debug(this->name() << " calling before-simulation callback");
      before_sim_cb();
    }
    debug(this->name() << " gem5 simulation is about to start");
  }

  void afterSimulate()
  {
    debug(this->name() << " gem5 simulation finished");
    if (after_sim_cb) {
      debug(this->name() << " calling after-simulation callback");
      after_sim_cb();
    }
  }
  cb_func before_sim_cb;
  cb_func after_sim_cb;
};

class gem5_tlm_br
{
public:
  gem5_tlm_br(std::string name, std::string cfg) : name(name)
  {
    unsigned int np = get_num_ports(cfg);
    assert(np > 0);

    sc_dt::uint64 offset = get_ext_mem_base_addr(cfg);

    std::ifstream file(cfg);
    if (!file.is_open()) {
      debug(this->name << " Unable to open file " << cfg);
      throw std::runtime_error("Unable to open file");
    }

    std::string sctrl_name = this->name + "_sctrl";
    sctrl = new ems::gem5_sim_ctrl(sctrl_name, cfg);
    Gem5SystemC::Gem5SlaveTransactor *t;
    ems::addr_adapter *adapt;
    std::string line;
    std::regex rgx("port_data=(\\w+)");
    std::smatch match;
    while (std::getline(file, line)) {
      if (std::regex_search(line, match, rgx)) {
        std::string tn = match.str(1);
        t = new Gem5SystemC::Gem5SlaveTransactor(tn.c_str(), tn.c_str());
        t->sim_control.bind(*sctrl);
        transactors.push_back(t);

        std::string an = tn + "_addr_adapt";
        adapt = new ems::addr_adapter(an.c_str(), static_cast<sc_dt::uint64>(offset));
        adapters.push_back(adapt);
        t->socket.bind(adapt->tsocket);
      }
    }
    file.close();
    assert(transactors.size() == np);
  }

  ~gem5_tlm_br()
  {
    for (auto a : adapters) {
      delete a;
    }
    for (auto t : transactors) {
       delete t;
    }
    delete sctrl;
  }

  // External bindings
  std::vector<ems::addr_adapter *> adapters;

private:
  unsigned int get_num_ports(std::string cfg)
  {
    std::string cmd = "grep port_data= " + cfg + " | wc -l";
    unsigned int np = std::stoul(ems::sh_exec(cmd.c_str()));
    debug(this->name << " ports found: " << np);
    return np;
  }

  uint64_t get_ext_mem_base_addr(std::string cfg)
  {
    uint64_t emba = 0;
    std::ifstream file(cfg);
    if (!file.is_open()) {
      debug(this->name << " Unable to open file " << cfg);
      throw std::runtime_error("Unable to open file");
    }
    std::string line;
    std::regex rgx1("\\[system.external_memory\\]");
    std::regex rgx2("addr_ranges=(\\d+):");
    std::smatch match;
    while (std::getline(file, line)) {
      if (std::regex_search(line, match, rgx1)) {
        while (std::getline(file, line)) {
          if (std::regex_search(line, match, rgx2)) {
            emba = static_cast<uint64_t>(std::stoull(match.str(1).c_str()));
            debug(this->name << " gem5 external memory base address: 0x" << std::setfill('0') << std::setw(16) << std::hex << emba << std::dec);
            goto found;
          }
        }
      }
    }
found:
    file.close();
    return emba;
  }

  ems::gem5_sim_ctrl *sctrl;
  std::vector<Gem5SystemC::Gem5SlaveTransactor *> transactors;
  std::string name;
};

} // namespace ems

#endif /* __EMS_GEM5_TLM_BR_H__ */

