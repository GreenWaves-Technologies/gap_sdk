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
#include <vector>
#include <sstream>



typedef struct
{
  uint32_t tag;
  bool dirty;
  uint8_t *data;
  vp::trace tag_event;
} cache_line_t;



class Cache : public vp::component {

  friend class CacheSet;
  friend class CacheLine;

public:

  Cache(js::config *config);

  unsigned int nb_ways_bits = 2;
  unsigned int line_size_bits = 5;
  unsigned int nb_sets_bits = 352;
  unsigned int nb_ways;
  unsigned int nb_sets;
  unsigned int line_size;
  unsigned int widthBits = 2;
  int          nb_ports = 1;

  bool enabled = false;

  int build();
  void start();

private:

  vp::trace     trace;

  std::vector<vp::io_slave> input_itf;
  vp::io_master refill_itf;
  vp::wire_slave<bool>      enable_itf;
  vp::wire_slave<bool>      flush_itf;
  vp::wire_slave<bool>      flush_line_itf;
  vp::wire_slave<uint32_t>  flush_line_addr_itf;

  vp::io_req refill_req;

  int64_t nextPacketStart;
  unsigned int R1;
  unsigned int R2;

  uint8_t lru_out;

  uint32_t line_offset_mask;
  uint32_t line_index_mask;
  uint32_t flush_line_addr;

  vp::trace refill_event;
  std::vector<vp::trace> io_event;

  cache_line_t *lines;

  static void enable_sync(void *_this, bool active);
  static void flush_sync(void *_this, bool active);
  static void flush_line_sync(void *_this, bool active);
  static void flush_line_addr_sync(void *_this, uint32_t addr);

  static vp::io_req_status_e req(void *__this, vp::io_req *req, int port);
  static vp::io_req_status_e req_l16_w4(void *__this, vp::io_req *req, int port);

  inline unsigned int getLineAddr(unsigned int addr) {return addr >> line_size_bits;}
  inline unsigned int get_line_base(unsigned int addr) {return addr & ~((1<<line_size_bits)-1);}
  inline unsigned int get_tag(unsigned int addr) {return addr >> (line_size_bits + nb_sets_bits);}
  inline unsigned int get_line_index(unsigned int addr) {return (addr >> (line_size_bits)) & ((1 << nb_sets_bits) - 1);}
  inline unsigned int get_line_offset(unsigned int addr) {return addr & ((1 << line_size_bits) - 1);}
  inline unsigned int getAddr(unsigned int index, unsigned int tag) {return (tag << (line_size_bits + nb_sets_bits)) | (index << line_size_bits);}

  cache_line_t *refill(int line_index, unsigned int addr, unsigned int tag, vp::io_req *req);

  unsigned int stepLru();
  bool ioReq(vp::io_req *req, int i);
  void enable(bool enable);
  void flush();
  void flush_line(unsigned int addr);
};



int Cache::build()
{
  this->nb_ports = this->get_js_config()->get_child_int("nb_ports");
  this->nb_sets_bits = this->get_js_config()->get_child_int("nb_sets_bits");
  this->nb_ways_bits = this->get_js_config()->get_child_int("nb_ways_bits");
  this->line_size_bits = this->get_js_config()->get_child_int("line_size_bits");
  this->nb_ways = 1 << this->nb_ways_bits;
  this->nb_sets = 1 << this->nb_sets_bits;
  this->line_size = 1 << this->line_size_bits;

  this->input_itf.resize(this->nb_ports);

  this->R1 = 0xd3b6;
  this->R2 = 0x7775;

  this->lru_out = 0;

  for (int i=0; i<nb_ports; i++)
  {
    if (this->nb_ways == 4 && this->line_size == 16)
      this->input_itf[i].set_req_meth_muxed(&Cache::req_l16_w4, i);
    else
      this->input_itf[i].set_req_meth_muxed(&Cache::req, i);
    this->new_slave_port("input_" + std::to_string(i), &this->input_itf[i]);
  }

  this->traces.new_trace("trace", &this->trace, vp::DEBUG);

  this->enable_itf.set_sync_meth(Cache::enable_sync);
  this->new_slave_port("enable", &this->enable_itf);

  this->flush_itf.set_sync_meth(Cache::flush_sync);
  this->new_slave_port("flush", &this->flush_itf);

  this->flush_line_itf.set_sync_meth(Cache::flush_line_sync);
  this->new_slave_port("flush_line", &this->flush_line_itf);

  this->flush_line_addr_itf.set_sync_meth(Cache::flush_line_addr_sync);
  this->new_slave_port("flush_line_addr", &this->flush_line_addr_itf);

  this->new_master_port("refill", &this->refill_itf);

  this->io_event.resize(this->nb_ports);

  for (int i=0; i<this->nb_ports; i++)
  {
    traces.new_trace_event("port_" + std::to_string(i), &this->io_event[i], 32);
  }

  traces.new_trace_event("refill", &this->refill_event, 32);

  lines = new cache_line_t[(1<<this->nb_sets_bits)*this->nb_ways];
  for (int i=0; i<1<<this->nb_sets_bits; i++)
  {
    for (int j=0; j<this->nb_ways; j++)
    {
      cache_line_t *line = &lines[i*this->nb_ways+j];
      line->tag = -1;
      line->data = new uint8_t[1<<this->line_size_bits];
      traces.new_trace_event("set_" + std::to_string(j) + "/line_" + std::to_string(i), &line->tag_event, 32);
    }
  }

  this->line_index_mask = (1 << this->nb_sets_bits) - 1;
  this->line_offset_mask = (1 << this->line_size_bits) - 1;

  return 0;
}



void Cache::start()
{
  this->trace.msg("Instantiating cache (nb_sets: %d, nb_ways: %d, line_size: %d)\n", 1<<this->nb_sets_bits, this->nb_ways, 1<<this->line_size_bits);
}



cache_line_t *Cache::refill(int line_index, unsigned int addr, unsigned int tag, vp::io_req *req)
{
  unsigned int refillWay;

#if 1
  refillWay = this->stepLru() % this->nb_ways;
#else
  int electedAge = 0;
  int elected = -1;
  // Go through all lines to find the oldest one
  // At the same, decrease the age for everyone
  for (int i=0; i<1<<cache->nb_ways_bits; i++) {
    if (elected == -1 || linesAge[i] < electedAge) {
      electedAge = linesAge[i];
      elected = i;
    }
    if (linesAge[i]) linesAge[i]--;
  }
  // Set age of elected one to the max
  linesAge[elected] = (1<<cache->nb_ways_bits) - 1;
  refillWay = elected;
#endif

  cache_line_t *line = &this->lines[line_index*this->nb_ways + refillWay];

  uint32_t full_addr = this->get_line_base(addr);

  this->trace.msg("Refilling line (addr: 0x%x, index: %d)\n", full_addr, line_index);
  // Flush the line in case it is dirty to copy it back outside
  //flush();

  line->tag_event.event((uint8_t *)&full_addr);

  // And get the data from outside
  vp::io_req *refill_req = &this->refill_req;
  refill_req->init();
  refill_req->set_addr(full_addr);
  refill_req->set_is_write(false);
  refill_req->set_size(1<<this->line_size_bits);
  refill_req->set_data(line->data);

  vp::io_req_status_e err = this->refill_itf.req(refill_req);
  if (err != vp::IO_REQ_OK)
  {
    this->warning.force_warning("UNIMPLEMENTED AT %s %d\n", __FILE__, __LINE__);
    return NULL;
  }

  req->set_latency(refill_req->get_full_latency());

  line->tag = tag;

  return line;
}



void Cache::flush_line(unsigned int addr)
{
  this->trace.msg("Flushing cache line (addr: 0x%x)\n", addr);
  unsigned int tag = addr >> this->line_size_bits;
  unsigned int line_index = this->get_line_index(addr);
  for (int i=0; i<this->nb_ways; i++)
  {
    cache_line_t *line = &this->lines[line_index*this->nb_ways + i];
    if (line->tag == tag)
      line->tag= -1;
  }
}



void Cache::flush()
{
  this->trace.msg("Flushing whole cache\n");
  for (int i=0; i<1<<this->nb_sets; i++)
  {
    for (int j=0; i<j<<this->nb_ways; j++)
    {
      this->lines[i*this->nb_ways+j].tag = -1;;
    }
  }
}



void Cache::enable(bool enable) {
  this->enabled = enable;
  if (enable)
    this->trace.msg("Enabling cache\n");
  else
    this->trace.msg("Disabling cache\n");
}

vp::io_req_status_e Cache::req(void *__this, vp::io_req *req, int port)
{
  Cache *_this = (Cache *)__this;

  uint64_t offset = req->get_addr();
  uint8_t *data = req->get_data();
  uint64_t size = req->get_size();
  bool is_write = req->get_is_write();

  _this->trace.msg("Received req (port: %d, is_write: %d, offset: 0x%x, size: 0x%x)\n", port, is_write, offset, size);

  if (!_this->enabled)
    return _this->refill_itf.req_forward(req);
  
  _this->io_event[port].event((uint8_t *)&offset);

  unsigned int nb_sets = _this->nb_sets;
  unsigned int nb_ways = _this->nb_ways;
  unsigned int line_size = 1 << _this->line_size_bits;

  unsigned int tag = offset >> _this->line_size_bits;
  unsigned int line_index = tag & (nb_sets - 1);
  unsigned int line_offset = offset & (line_size - 1);

  _this->trace.msg("Cache access (is_write: %d, offset: 0x%x, size: 0x%x, tag: 0x%x, line_index: %d, line_offset: 0x%x)\n", is_write, offset, size, offset, line_index, line_offset);

  cache_line_t *hit_line = NULL;
  cache_line_t *line = &_this->lines[line_index*nb_ways];

  for (int i=0; i<nb_ways; i++)
  {
    if (line->tag == tag)
    {
      _this->trace.msg("Cache hit (way: %d)\n", i);
      hit_line = line;
      break;
    }
    line++;
  }

  if (hit_line == NULL)
  {
    _this->trace.msg("Cache miss\n");
    _this->refill_event.event((uint8_t *)&offset);
    hit_line = _this->refill(line_index, offset, tag, req);
    if (hit_line == NULL)
      return vp::IO_REQ_INVALID;
  }

  if (data)
  {
    if (!is_write) {
      memcpy(data, (void *)hit_line->data, size);
    } else {
      //hitLine->setDirty();
      memcpy((void *)hit_line->data, data, size);
    }
  }

  return vp::IO_REQ_OK;
}



vp::io_req_status_e Cache::req_l16_w4(void *__this, vp::io_req *req, int port)
{
  Cache *_this = (Cache *)__this;

  uint64_t offset = req->get_addr();
  uint8_t *data = req->get_data();
  uint64_t size = req->get_size();
  bool is_write = req->get_is_write();

  _this->trace.msg("Received req (port: %d, is_write: %d, offset: 0x%x, size: 0x%x)\n", port, is_write, offset, size);

  if (!_this->enabled)
    return _this->refill_itf.req_forward(req);
  
  _this->io_event[port].event((uint8_t *)&offset);

  const unsigned int line_size_bits = 4;
  const unsigned int nb_sets = _this->nb_sets;
  const unsigned int nb_ways = 4;
  const unsigned int line_size = 16;

  const unsigned int tag = offset >> line_size_bits;
  const unsigned int line_index = tag & (nb_sets - 1);
  const unsigned int line_offset = offset & (line_size - 1);

  _this->trace.msg("Cache access (is_write: %d, offset: 0x%x, size: 0x%x, tag: 0x%x, line_index: %d, line_offset: 0x%x)\n", is_write, offset, size, offset, line_index, line_offset);

  cache_line_t *line = &_this->lines[line_index*nb_ways];

  // This case is the mostly used by the ISS and is thus the most optimized.
  // The ISS will most of the time call the cache without data, just the model the timing
  // in case there is a miss.
  if (likely(!data))
  {
    if (tag != line[0].tag && tag != line[1].tag && tag != line[2].tag && tag != line[3].tag)
    {
      _this->trace.msg("Cache miss\n");
      _this->refill_event.event((uint8_t *)&offset);
      if (_this->refill(line_index, offset, tag, req) == NULL)
        return vp::IO_REQ_INVALID;
    }
  }
  else
  {
    cache_line_t *hit_line;

    if (tag == line[0].tag) hit_line = &line[0];
    else if (tag == line[1].tag) hit_line = &line[1];
    else if (tag == line[2].tag) hit_line = &line[2];
    else if (tag == line[3].tag) hit_line = &line[3];
    else
    {
      _this->trace.msg("Cache miss\n");
      _this->refill_event.event((uint8_t *)&offset);
      hit_line = _this->refill(line_index, offset, tag, req);
      if (hit_line == NULL)
        return vp::IO_REQ_INVALID;
    }

    if (!is_write) {
      memcpy(data, (void *)hit_line->data, size);
    } else {
      //hitLine->setDirty();
      memcpy((void *)hit_line->data, data, size);
    }
  }

  return vp::IO_REQ_OK;
}

unsigned int Cache::stepLru()
{
  if (1)
  {
    // 8 bits LFSR used on GAP FC icache
    int linear_feedback = !(((this->lru_out >> 7) & 1) ^ ((this->lru_out >> 3) & 1) ^ ((this->lru_out >> 2) & 1) ^ ((this->lru_out >> 1) & 1)); // TAPS for XOR feedback

    this->lru_out = (this->lru_out << 1) | (linear_feedback & 1);

    int way = (this->lru_out >> 1) & (this->nb_ways - 1);

    return way;
  }
  else
  {
    unsigned int b1 = (((R1 & 0x00000008) >>  3) ^ ((R1 & 0x00001000) >> 12) ^ ((R1 & 0x00004000) >> 14) ^ ((R1 & 0x00008000) >> 15)) & 1;
    unsigned int b2 = (((R2 & 0x00000008) >>  3) ^ ((R2 & 0x00001000) >> 12) ^ ((R2 & 0x00004000) >> 14) ^ ((R2 & 0x00008000) >> 15)) & 1;
    R1 <<= 1;
    R1 |= b1;
    R2 <<= 1;
    R2 |= b2;
    return (((R2 & 0x00010000) >> 15) | ((R1 & 0x00010000) >> 16));
  }
}



void Cache::enable_sync(void *__this, bool active)
{
  Cache *_this = (Cache *)__this;
  _this->enable(active);
}



void Cache::flush_sync(void *__this, bool active)
{
  Cache *_this = (Cache *)__this;
  if (active)
    _this->flush();
}



void Cache::flush_line_sync(void *__this, bool active)
{
  Cache *_this = (Cache *)__this;
  if (active)
    _this->flush_line(_this->flush_line_addr);
}



void Cache::flush_line_addr_sync(void *__this, uint32_t addr)
{
  Cache *_this = (Cache *)__this;
  _this->flush_line_addr = addr;
}



Cache::Cache(js::config *config)
: vp::component(config)
{

}



extern "C" vp::component *vp_constructor(js::config *config)
{
  return new Cache(config);
}
