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

#include <vp/vp.hpp>
#include <vp/itf/io.hpp>
#include <vp/itf/wire.hpp>
#include <stdio.h>
#include <math.h>

class router;

class Perf_counter {
public:
  int64_t nb_read;
  int64_t nb_write;
  int64_t read_stalls;
  int64_t write_stalls;

  vp::wire_slave<uint32_t> nb_read_itf;
  vp::wire_slave<uint32_t> nb_write_itf;
  vp::wire_slave<uint32_t> read_stalls_itf;
  vp::wire_slave<uint32_t> write_stalls_itf;
  vp::wire_slave<uint32_t> stalls_itf;

  static void nb_read_sync_back(void *__this, uint32_t *value);
  static void nb_read_sync(void *__this, uint32_t value);
  static void nb_write_sync_back(void *__this, uint32_t *value);
  static void nb_write_sync(void *__this, uint32_t value);
  static void read_stalls_sync_back(void *__this, uint32_t *value);
  static void read_stalls_sync(void *__this, uint32_t value);
  static void write_stalls_sync_back(void *__this, uint32_t *value);
  static void write_stalls_sync(void *__this, uint32_t value);
  static void stalls_sync_back(void *__this, uint32_t *value);
  static void stalls_sync(void *__this, uint32_t value);
};

class MapEntry {
public:
  MapEntry() {}
  MapEntry(unsigned long long base, MapEntry *left, MapEntry *right);

  void insert(router *router);

  string target_name;
  MapEntry *next = NULL;
  int id = -1;
  unsigned long long base = 0;
  unsigned long long lowestBase = 0;
  unsigned long long size = 0;
  unsigned long long remove_offset = 0;
  unsigned long long add_offset = 0;
  uint32_t latency = 0;
  int64_t nextPacketTime = 0;
  MapEntry *left = NULL;
  MapEntry *right = NULL;
  vp::io_slave *port = NULL;
  vp::io_master *itf = NULL;
};

class io_master_map : public vp::io_master
{

  inline void bind_to(vp::port *port, vp::config *config);

};

class router : public vp::component
{

  friend class MapEntry;

public:

  router(js::config *config);

  int build();

  static vp::io_req_status_e req(void *__this, vp::io_req *req);


  static void grant(void *_this, vp::io_req *req);

  static void response(void *_this, vp::io_req *req);

private:
  vp::trace     trace;

  io_master_map out;
  vp::io_slave in;
  bool init = false;

  void init_entries();
  MapEntry *firstMapEntry = NULL;
  MapEntry *defaultMapEntry = NULL;
  MapEntry *errorMapEntry = NULL;
  MapEntry *topMapEntry = NULL;
  MapEntry *externalBindingMapEntry = NULL;

  std::map<int, Perf_counter *> counters;

  int bandwidth = 0;
  int latency = 0;
};

router::router(js::config *config)
: vp::component(config)
{

}

MapEntry::MapEntry(unsigned long long base, MapEntry *left, MapEntry *right) : next(NULL), base(base), lowestBase(left->lowestBase), left(left), right(right) {
}

void MapEntry::insert(router *router)
{
  lowestBase = base;

  if (size != 0) {
    if (port != NULL || itf != NULL) {    
      MapEntry *current = router->firstMapEntry;
      MapEntry *prev = NULL;

      while(current && current->base < base) {
        prev = current;
        current = current->next;
      }
      
      if (prev == NULL) {
        next = router->firstMapEntry;
        router->firstMapEntry = this;
      } else {
        next = current;
        prev->next = this;
      }
    } else {
      router->errorMapEntry = this;
    }
  } else {
    router->defaultMapEntry = this;
  }
}

vp::io_req_status_e router::req(void *__this, vp::io_req *req)
{
  router *_this = (router *)__this;
  
  if (!_this->init)
  {
    _this->init = true;
    _this->init_entries();
  }

  MapEntry *entry = _this->topMapEntry;
  uint64_t offset = req->get_addr();
  bool isRead = !req->get_is_write();
  uint64_t size = req->get_size();  

  _this->trace.msg(vp::trace::LEVEL_TRACE, "Received IO req (offset: 0x%llx, size: 0x%llx, isRead: %d)\n", offset, size, isRead);

  if (entry)
  {
    while(1) {
    // The entry does not have any child, this means we are at a final entry
      if (entry->left == NULL) break;

      if (offset >= entry->base) entry = entry->right;
      else entry = entry->left;
    }

    if (entry && (offset < entry->base || offset > entry->base + entry->size - 1)) {
      entry = NULL;
    }
  }

  if (!entry) {
    if (_this->errorMapEntry && offset >= _this->errorMapEntry->base && offset + size - 1 <= _this->errorMapEntry->base + _this->errorMapEntry->size - 1) {
    } else {
      entry = _this->defaultMapEntry;
    }
  }

  if (!entry) {
    //_this->trace.msg(&warning, "Invalid access (offset: 0x%llx, size: 0x%llx, isRead: %d)\n", offset, size, isRead);
    return vp::IO_REQ_INVALID;
  }

  if (entry == _this->defaultMapEntry) {
    _this->trace.msg(vp::trace::LEVEL_TRACE, "Routing to default entry (target: %s)\n", entry->target_name.c_str());
  } else {
    _this->trace.msg(vp::trace::LEVEL_TRACE, "Routing to entry (target: %s)\n", entry->target_name.c_str());
  }
  
  if (0) { //_this->bandwidth != 0 and !req->is_debug()) {
    
#if 0
  // Compute the duration from the specified bandwidth
  // Don't forget to compare to the already computed duration, as there might be a slower router
  // on the path
    req->set_duration((float)size / _this->bandwidth);

    // This is the time when the router is available
    int64_t routerTime = max(getCycles(), entry->nextPacketTime);

    // This is the time when the packet is available for the next module
    // It is either delayed by the router in case of bandwidth overflow, and in this case
    // we only apply the router latency, or it is delayed by the latency of the components 
    // on the path plus the router latency.
    // Just select the maximum
    int64_t packetTime = max(routerTime + entry->latency, getCycles() + req->getLatency() + entry->latency);

    // Compute the latency to be reported from the estimated packet time at the output
    req->setLatency(packetTime - getCycles());

    // Update the bandwidth information
    entry->nextPacketTime = routerTime + req->getLength();

#endif
  } else {
    req->inc_latency(entry->latency + _this->latency);
  }

  // Forward the request to the target port
  if (entry->remove_offset) req->set_addr(offset - entry->remove_offset);
  if (entry->add_offset) req->set_addr(offset + entry->add_offset);
  vp::io_req_status_e result = vp::IO_REQ_OK;
  if (entry->port)
  {
    req->arg_push(NULL);
    result = _this->out.req(req, entry->port);
    if (result == vp::IO_REQ_OK)
      req->arg_pop();
  }
  else if (entry->itf)
  {
    if (!entry->itf->is_bound())
    {
      _this->warning.msg(vp::trace::LEVEL_WARNING, "Invalid access, trying to route to non-connected interface (offset: 0x%llx, size: 0x%llx, is_write: %d)\n", offset, size, !isRead);
      return vp::IO_REQ_INVALID;
    }
    req->arg_push(req->resp_port);
    result = entry->itf->req(req);
    if (result == vp::IO_REQ_OK)
      req->arg_pop();
  }

  if (entry->id != -1) 
  {
    int64_t latency = req->get_latency();
    int64_t duration = req->get_duration();
    if (duration > 1) latency += duration - 1;

    Perf_counter *counter = _this->counters[entry->id];

    if (isRead)
      counter->read_stalls += latency;
    else
      counter->write_stalls += latency;
  
    if (isRead)
      counter->nb_read++;
    else
      counter->nb_write++;

  }

  return result;
}

void router::grant(void *__this, vp::io_req *req)
{
  router *_this = (router *)__this;

  vp::io_slave *port = (vp::io_slave *)req->arg_pop();
  if (port != NULL)
  {
    port->grant(req);
  }

  req->arg_push(port);
}

void router::response(void *_this, vp::io_req *req)
{
  vp::io_slave *port = (vp::io_slave *)req->arg_pop();
  if (port != NULL)
    port->resp(req);
}

int router::build()
{
  traces.new_trace("trace", &trace, vp::DEBUG);

  in.set_req_meth(&router::req);
  new_slave_port("input", &in);

  out.set_resp_meth(&router::response);
  out.set_grant_meth(&router::grant);
  new_master_port("out", &out);

  bandwidth = get_config_int("bandwidth");
  latency = get_config_int("latency");

  js::config *mappings = get_js_config()->get("mappings");

  if (mappings != NULL)
  {
    for (auto& mapping: mappings->get_childs())
    {
      js::config *config = mapping.second;

      MapEntry *entry = new MapEntry();
      entry->target_name = mapping.first;

      vp::io_master *itf = new vp::io_master();

      itf->set_resp_meth(&router::response);
      itf->set_grant_meth(&router::grant);
      new_master_port(mapping.first, itf);

      if (mapping.first == "error")
        entry->itf = NULL;
      else
        entry->itf = itf;

      js::config *conf;
      conf = config->get("base");
      if (conf) entry->base = conf->get_int();
      conf = config->get("size");
      if (conf) entry->size = conf->get_int();
      conf = config->get("remove_offset");
      if (conf) entry->remove_offset = conf->get_int();
      conf = config->get("add_offset");
      if (conf) entry->add_offset = conf->get_int();
      conf = config->get("latency");
      if (conf) entry->latency = conf->get_int();
      conf = config->get("id");
      if (conf) entry->id = conf->get_int();

      if (this->counters.find(entry->id) == this->counters.end())
      {
        Perf_counter *counter = new Perf_counter();
        this->counters[entry->id] = counter;

        counter->nb_read_itf.set_sync_back_meth(&Perf_counter::nb_read_sync_back);
        counter->nb_read_itf.set_sync_meth(&Perf_counter::nb_read_sync);
        new_slave_port((void *)counter, "nb_read[" + std::to_string(entry->id) + "]", &counter->nb_read_itf);

        counter->nb_write_itf.set_sync_back_meth(&Perf_counter::nb_write_sync_back);
        counter->nb_write_itf.set_sync_meth(&Perf_counter::nb_write_sync);
        new_slave_port((void *)counter, "nb_write[" + std::to_string(entry->id) + "]", &counter->nb_write_itf);

        counter->read_stalls_itf.set_sync_back_meth(&Perf_counter::read_stalls_sync_back);
        counter->read_stalls_itf.set_sync_meth(&Perf_counter::read_stalls_sync);
        new_slave_port((void *)counter, "read_stalls[" + std::to_string(entry->id) + "]", &counter->read_stalls_itf);

        counter->write_stalls_itf.set_sync_back_meth(&Perf_counter::write_stalls_sync_back);
        counter->write_stalls_itf.set_sync_meth(&Perf_counter::write_stalls_sync);
        new_slave_port((void *)counter, "write_stalls[" + std::to_string(entry->id) + "]", &counter->write_stalls_itf);

        counter->stalls_itf.set_sync_back_meth(&Perf_counter::stalls_sync_back);
        counter->stalls_itf.set_sync_meth(&Perf_counter::stalls_sync);
        new_slave_port((void *)counter, "stalls[" + std::to_string(entry->id) + "]", &counter->stalls_itf);
      }  

      entry->insert(this);
    }
  }
  return 0;
}

extern "C" vp::component *vp_constructor(js::config *config)
{
  return new router(config);
}




#define max(a, b) ((a) > (b) ? (a) : (b))

void router::init_entries() {

  MapEntry *current = firstMapEntry;
  trace.msg(vp::trace::LEVEL_INFO, "Building router table\n");
  while(current) {
    trace.msg(vp::trace::LEVEL_INFO, "  0x%16llx : 0x%16llx -> %s\n", current->base, current->base + current->size, current->target_name.c_str());
    current = current->next;
  }
  if (errorMapEntry != NULL) {
    trace.msg(vp::trace::LEVEL_INFO, "  0x%16llx : 0x%16llx -> ERROR\n", errorMapEntry->base, errorMapEntry->base + errorMapEntry->size);
  }
  if (defaultMapEntry != NULL) {
    trace.msg(vp::trace::LEVEL_INFO, "       -     :      -     -> %s\n", defaultMapEntry->target_name.c_str());
  }

  MapEntry *firstInLevel = firstMapEntry;

  // Loop until we merged everything into a single entry
  // Start with the routing table entries
  current = firstMapEntry;

  // The first loop is here to loop until we have merged everything to a single entry
  while(1) {

    // Gives the map entry that should be on the left side of the next created entry
    MapEntry *left = NULL;

    // Gives the last allocated entry where the entry should be inserted
    MapEntry *currentInLevel=NULL;

    // The second loop is iterating on a single level to merge entries 2 by 2
    while(current) {
      if (left == NULL) left = current;
      else {

        MapEntry *entry = new MapEntry(current->lowestBase, left, current);

        left = NULL;
        if (currentInLevel) {
          currentInLevel->next = entry;
        } else {
          firstInLevel = entry;
        }
        currentInLevel = entry;
      }

      current = current->next;
    }

    current = firstInLevel;

    // Stop in case we got a single entry
    if (currentInLevel == NULL) break;

    // In case an entry is alone, insert it at the end, it will be merged with an upper entry
    if (left != NULL) {
      currentInLevel->next = left;
    }
  }

  topMapEntry = firstInLevel;
}

inline void io_master_map::bind_to(vp::port *_port, vp::config *config)
{
  MapEntry *entry = new MapEntry();
  vp::config *conf;
  entry->port = (vp::io_slave *)_port;
  if (config)
  {
    conf = config->get("base");
    if (conf) entry->base = conf->get_int();
    conf = config->get("size");
    if (conf) entry->size = conf->get_int();
    conf = config->get("remove_offset");
    if (conf) entry->remove_offset = conf->get_int();
    conf = config->get("add_offset");
    if (conf) entry->add_offset = conf->get_int();
    conf = config->get("latency");
    if (conf) entry->latency = conf->get_int();
  }
  entry->insert((router *)get_comp());
}

void Perf_counter::nb_read_sync_back(void *__this, uint32_t *value)
{
  Perf_counter *_this = (Perf_counter *)__this;
  *value = _this->nb_read;
}

void Perf_counter::nb_read_sync(void *__this, uint32_t value)
{
  Perf_counter *_this = (Perf_counter *)__this;
  _this->nb_read = value;
}

void Perf_counter::nb_write_sync_back(void *__this, uint32_t *value)
{
  Perf_counter *_this = (Perf_counter *)__this;
  *value = _this->nb_write;
}

void Perf_counter::nb_write_sync(void *__this, uint32_t value)
{
  Perf_counter *_this = (Perf_counter *)__this;
  _this->nb_write = value;
}

void Perf_counter::read_stalls_sync_back(void *__this, uint32_t *value)
{
  Perf_counter *_this = (Perf_counter *)__this;
  *value = _this->read_stalls;
}

void Perf_counter::read_stalls_sync(void *__this, uint32_t value)
{
  Perf_counter *_this = (Perf_counter *)__this;
  _this->read_stalls = value;
}

void Perf_counter::write_stalls_sync_back(void *__this, uint32_t *value)
{
  Perf_counter *_this = (Perf_counter *)__this;
  *value = _this->write_stalls;
}

void Perf_counter::write_stalls_sync(void *__this, uint32_t value)
{
  Perf_counter *_this = (Perf_counter *)__this;
  _this->write_stalls = value;
}

void Perf_counter::stalls_sync_back(void *__this, uint32_t *value)
{
  Perf_counter *_this = (Perf_counter *)__this;

  *value = _this->read_stalls + _this->write_stalls;
}

void Perf_counter::stalls_sync(void *__this, uint32_t value)
{
  Perf_counter *_this = (Perf_counter *)__this;
  _this->read_stalls = value;
  _this->write_stalls = value;
}
