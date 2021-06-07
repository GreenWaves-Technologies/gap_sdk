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
 *          Martin Croome, GreenWaves Technologies (martin.croome@greenwaves-technologies.com)
 */

#include "gdb-server/gdb-server.hpp"
#include <unistd.h>

class Memory_wall
{
public:
  Memory_wall(uint32_t start, uint32_t len);
  bool overlaps(uint32_t start, uint32_t len);

private:
  uint32_t start;
  uint32_t len;
};



class Target_cache
{
public:
  virtual void flush() { }
  virtual ~Target_cache() {}
};



class Target_cluster_cache : public Target_cache
{
public:
  Target_cluster_cache(Gdb_server * top, uint32_t addr);
  ~Target_cluster_cache() {}
  void flush();
private:
  Gdb_server * m_top;
  uint32_t addr;
};



class Target_fc_cache : public Target_cache
{
public:
  Target_fc_cache(Gdb_server * top, uint32_t addr);
  ~Target_fc_cache() {}
  void flush();
private:
  Gdb_server * m_top;
  uint32_t addr;
};


class Target_cluster_power
{
public:
  virtual bool is_on() { return true; }
  virtual void init() {}
  virtual ~Target_cluster_power() {}
};



class Target_cluster_power_bypass : public Target_cluster_power
{
public:
  Target_cluster_power_bypass(Gdb_server * top, uint32_t reg_addr, int bit);
  ~Target_cluster_power_bypass() {}
  bool is_on();
  void init() { m_stay_on_indicated = false; }

private:
  Gdb_server * m_top;
  uint32_t reg_addr;
  int bit;
  bool m_stay_on_indicated = false;
};



class Target_cluster_ctrl
{
public:
  virtual ~Target_cluster_ctrl() {}
  virtual void init() {}
  virtual bool has_xtrigger() { return false; }
};



class Target_cluster_ctrl_xtrigger : public Target_cluster_ctrl
{
public:
  Target_cluster_ctrl_xtrigger(Gdb_server * top, uint32_t cluster_ctrl_addr);
  ~Target_cluster_ctrl_xtrigger() {}
  void init();
  bool has_xtrigger() { return true; }
  void set_halt_mask(uint32_t mask);
  uint32_t get_halt_mask();
  uint32_t get_halt_status();
  void set_resume(uint32_t mask);

private:
  Gdb_server * m_top;
  uint32_t cluster_ctrl_addr;
  uint32_t current_mask = 0;
};



class Target_cluster_common
{
  friend class Target_cluster;
  friend class Target_fc;

public:
  Target_cluster_common(js::config *config, Gdb_server * top, uint32_t cluster_addr, uint32_t xtrigger_addr, int cluster_id);
  virtual ~Target_cluster_common();
  int get_nb_core() { return nb_core; }
  std::shared_ptr<Target_core> get_core(int i) { return cores[i]; }
  void update_power();
  void set_power(bool is_on);
  bool get_power();
  bool decide_resume();
  void resume();
  void halt();
  void flush();
  void init();
  void clear_resume();
  void commit_resume();
  void prepare_resume(bool step);
  int get_id() { return cluster_id; }
  virtual std::shared_ptr<Target_core> check_stopped(uint32_t *stopped_cause);
  void add_memory_wall(uint32_t addr, uint32_t len);
  bool memory_wall_overlaps(uint32_t addr, uint32_t len);
protected:
  Gdb_server * m_top;
  std::vector<std::shared_ptr<Target_core>> cores;
  bool is_on = false;
  std::shared_ptr<Target_cluster_power> power;
  std::shared_ptr<Target_cluster_ctrl> ctrl;
  int nb_on_cores = 0;
  int nb_core = 0;
  int cluster_id;
  uint32_t cluster_addr;
  uint32_t xtrigger_addr;
  std::shared_ptr<Target_cache> cache = NULL;
  bool resume_prepared = false;
  std::vector<std::shared_ptr<Memory_wall>> mem_walls;
};



class Target_cluster : public Target_cluster_common
{
public:
  Target_cluster(js::config *system_config, js::config *config, Gdb_server * top, uint32_t cluster_base, uint32_t xtrigger_base, int cluster_id);
};



class Target_fc : public Target_cluster_common
{
public:
  Target_fc(js::config *system_config, js::config *config, Gdb_server * top, uint32_t fc_dbg_base, uint32_t fc_cache_base, int cluster_id);
  std::shared_ptr<Target_core> check_stopped(uint32_t *stopped_cause);
private:
  bool event_unit_core_gated();
  uint32_t m_fc_eu_status = 0;
};
