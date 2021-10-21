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

#include <iostream>
#include <algorithm>
#include <sstream>

#include "profiler_server.hpp"
#include <string.h>

Profiler_server::Profiler_server(std::string file, uint64_t f_max,
                                 double tolerance)
    : trace_dumper_server(file)
{
  this->f_max = (double)f_max / 1'000'000'000'000; //convertion seconds to ps
  this->tolerance = tolerance;
  /* compile regex used for path interpretation */
  if (regcomp(&dma_regex, dma_c_regex, REG_EXTENDED))
  {
    std::cout << "[-] error: impossible to compile dma regex" << std::endl;
  }
  std::string stall_str_regex = "((" + std::string(stall_fc_regex) + ")|(";
  stall_str_regex += std::string(stall_cluster_regex) + "))";
  stall_str_regex += "(";
  for (int i = 0; i < N_STALL_REASONS; i++)
  {
    if (i != 0)
      stall_str_regex += "|";
    stall_str_regex += "(";
    stall_str_regex += stall_strings[i];
    stall_str_regex += ")";
  }
  stall_str_regex += ")$";
  std::cout << "[.] stall regex is " << stall_str_regex << std::endl;
  if (regcomp(&stall_regex, stall_str_regex.c_str(), REG_EXTENDED))
  {
    std::cout << "[-] Impossible to compile stall regex : " << stall_str_regex;
    std::cout << std::endl;
  }
}

Profiler_server::~Profiler_server()
{
  regfree(&dma_regex);
  regfree(&stall_regex);
}

static int parse_core_id(const char *path, const char *ref)
{
  int core_id = -1;
  int L = strlen(ref);
  int M = strlen(path);
  if (M < L + 2)
    return core_id;
  path += M - L - 2;
  if (path[0] == 'f' && path[1] == 'c')
    core_id = 0;
  else
    core_id = (int)strtol(path + 1, NULL, 10) + 1;
  return core_id;
}

static bool match_path(const char *path, int path_len, const char *ref)
{
  int T = path_len;
  int L = strlen(ref);
  return (T >= L + 2 && strcmp(ref, path + (T - L)) == 0 && ((path[T - L - 2] == 'e' && path[T - L - 1] >= '0' && path[T - L - 1] <= '9') || (path[T - L - 2] == 'f' && path[T - L - 1] == 'c')));
}

void print_regerror(int errcode, size_t length, regex_t *compiled)
{
  char buffer[length];
  (void)regerror(errcode, compiled, buffer, length);
  fprintf(stderr, "Regex match failed: %s\n", buffer);
}

int first_non_empty(regmatch_t pmatch[], int nmatch, int start = 1)
{
  for (int i = start; i < nmatch; i++)
  {
    if (pmatch[i].rm_so != pmatch[i].rm_eo)
      return i;
  }
  return -1;
}

void Profiler_server::path_analysis(const char *path, const trace_packet *packet)
{
  /* store additional info in hash tables to speed up packet analysis */
  if (match_path(path, packet->reg_trace.path_len, "/pc"))
  {
    is_pc[packet->reg_trace.id] = parse_core_id(path, "/pc");
  }
  else
    is_pc[packet->reg_trace.id] = -1;

  if (match_path(path, packet->reg_trace.path_len, "/state"))
  {
    is_state[packet->reg_trace.id] = parse_core_id(path, "/state");
  }
  else
    is_state[packet->reg_trace.id] = -1;

  if (path == std::string("/sys/board/chip/cluster_clock/period"))
  {
    cluster_period_tr_id = packet->reg_trace.id;
  }
  else if (path == std::string("/sys/board/chip/soc_clock/period"))
  {
    fc_period_tr_id = packet->reg_trace.id;
  }
  else if (path == std::string("/sys/board/chip/cluster_clock/cycles"))
  {
    cluster_cycle_id = packet->reg_trace.id;
  }
  else if (path == std::string("/sys/board/chip/soc_clock/cycles"))
  {
    fc_cycle_id = packet->reg_trace.id;
  }

  if (!regexec(&dma_regex, path, 0, NULL, 0))
  {
    is_dma[packet->reg_trace.id] = true;
  }
  else
    is_dma[packet->reg_trace.id] = false;

  regmatch_t pmatch[STALL_NMATCH];
  if (!regexec(&stall_regex, path, STALL_NMATCH, pmatch, 0))
  {
    int core = first_non_empty(pmatch, 4, 2) - 2;
    int type = first_non_empty(pmatch, STALL_NMATCH, 6) - 6;
    if (type >= 0 && type < N_STALL_REASONS)
    {
      stall_mapping[packet->reg_trace.id] = (stall_trace_t){
          .type = (stall_e)type,
          .core_id = (core ? (int)strtol(path + pmatch[4].rm_so, NULL, 10) + 1
                           : 0)};
    }
  }
}

/*
  WARNING match_rle(e1, e2) != match_rle(e2, e1) !!
  the conditions might seem very restrictive, but the rle compression is first
  designed for clock signals (for which it returns true!)
*/
bool Profiler_server::match_rle(const Data_with_time<g_compressed_data_t> &e1,
                                const Data_with_time<g_compressed_data_t> &e2)
{
  if (e1.end != e2.begin)
    return false;
  if (e1.d.value != e2.d.value)
    return false;
  if (e1.d.n_items == 0 || e2.d.n_items == 0)
    return false;
  if ((e1.end - e1.begin) / (e1.d.n_items * e1.d.rle_coeff) !=
      (e2.end - e2.begin) / (e2.d.n_items * e2.d.rle_coeff))
    return false;

  return true;
}

/**
    adds e to the rle compression buffer, in order for that event to be rle
    compressed before being sent to its TLData structure.
*/
int Profiler_server::flush_to_rle_buffer(uint32_t trace_id, const Event_record &e)
{
  if (rle_buffer.find(trace_id) == rle_buffer.end())
  {
    rle_buffer[trace_id] = e;
    return 1;
  }

  if (match_rle(rle_buffer[trace_id].compr_data, e.compr_data)
      && rle_buffer[trace_id].compr_data.d.rle_coeff != 0xff){
    // cool, we can RLE-compress the new item
    //std::cout << "--- MATCH ----" << std::endl;
    //print_dWT(rle_buffer[trace_id].compr_data);
    //print_dWT(e.compr_data);
    //std::cout << std::endl;
    rle_buffer[trace_id].compr_data.d.rle_coeff++;
    rle_buffer[trace_id].compr_data.end = e.compr_data.end;
    return 0;
  } else {
  
  /*std::cout << "--- FAILED ----" << std::endl;
    print_dWT(rle_buffer[trace_id].compr_data);
    print_dWT(e.compr_data);
    std::cout << std::endl;*/
  // too bad, cannot compress
  // so we send the event to its TLData structure
  //std::cout << "Add Event " << trace_id << " to events[] buffer" << std::endl;
  events[trace_id].add_item(rle_buffer[trace_id].compr_data.begin,
                            rle_buffer[trace_id].compr_data.end,
                            rle_buffer[trace_id].compr_data.d);
  // then write the new item into the buffer
  rle_buffer[trace_id] = e;
  return 1;

  }
}

void Profiler_server::update_avg_threshold(uint32_t trace_id)
{
  auto &et = entry_timestamps[trace_id];
  // first get the current threshold
  uint32_t thr = average_buffer[trace_id].average_threshold;
  if (thr == 0)
  { // should not happen
    average_buffer[trace_id].average_threshold = 1;
    thr = 1;
  }

  while (et.size() > thr)
    et.pop_front();
  et.push_back(timestamp);
  // hence, thr + 1 == et.size()
  uint64_t delta_t = timestamp - et.front();
  if (delta_t == 0)
    return;

  // we got thr items in delta_t ps, and only one representant of these thr items
  // was stored. So the current storing frequency is:
  double f = 1. / (double)delta_t;

  if (trace_id == 62)
  {
    //for (auto x: et) std::cout << x << " ";
    //std::cout << "f = " << f << "; f_max = " << f_max << std::endl;
    //std::cout << "current thr = " << thr << std::endl;
  }

  if (thr < (1 << (MEMBER_SIZE(g_compressed_data_t, n_items) * 8)) / 2 && f > tolerance * f_max)
  {
    average_buffer[trace_id].average_threshold *= 2;
  }
  else if (thr > 1 && f < 1. / tolerance * f_max)
  {
    average_buffer[trace_id].average_threshold /= 2;
  }
}

/**
    instead of compressing events to their TLData, we can use this function
    just to add events to the TLData
*/
void Profiler_server::just_add(uint32_t trace_id, uint64_t begin,
                               uint64_t end, generic_data_t data)
{

  //std::cout << " Just add trace_id: " << trace_id << std::endl;

  g_compressed_data_t cdata = {
      .value = data,
      .rle_coeff = 1,
      .n_items = 1};
  Data_with_time<g_compressed_data_t> dwt = {
      .d = cdata,
      .begin = begin,
      .end = end,
      .mixed_up = false};

  average_buffer[trace_id] = {
      .compr_data = dwt,
      .average_threshold = 1};

  Event_record &e = average_buffer[trace_id];
  flush_to_rle_buffer(trace_id, e);
}

/**
    instead of adding events directly to their TLData, we can use this function
    to compress events before being added
*/
void Profiler_server::compress_and_add(uint32_t trace_id, uint64_t begin,
                                       uint64_t end, generic_data_t data)
{
  g_compressed_data_t cdata = {
      .value = data,
      .rle_coeff = 1,
      .n_items = 1};
  Data_with_time<g_compressed_data_t> dwt = {
      .d = cdata,
      .begin = begin,
      .end = end,
      .mixed_up = false};

  /* first we handle the compression using the average */
  // initialisation case
  if (average_buffer.find(trace_id) == average_buffer.end())
  {
    average_buffer[trace_id] = {
        .compr_data = dwt,
        .average_threshold = 1};
    return;
  }

  Event_record &e = average_buffer[trace_id];

  if (e.compr_data.d.n_items >= e.average_threshold)
  {
    /*
       we have reached the desired compression level using the average
       so now we send data to the rle buffer for the next layer of compression
    */
    e.compr_data.d.value /= e.compr_data.d.n_items;
    if (flush_to_rle_buffer(trace_id, e))
    {
      // if the new item cannot be rle compressed, we might need to change the
      // average threshold
      update_avg_threshold(trace_id);
    }
    e.compr_data = dwt;
  }
  else
  {
    /* not enough items in the average buffer, we add one more */
    e.compr_data.d.value += data;
    e.compr_data.d.n_items++;
    e.compr_data.end = timestamp;
    update_avg_threshold(trace_id);
  }
}

void Profiler_server::add_event(const trace_packet *packet)
{

  if (packet->trace->type == ED_TRACE_VARLEN)
  {
    //std::cout << "add_event -- varlen trace is not supported -- trace_id: " << packet->trace->id << std::endl;
    return;
  }

  //packet->dump();

  Data_with_time<generic_data_t> dwt;

  if (event_buffer.find(packet->trace->id) != event_buffer.end())
  {
    dwt = event_buffer[packet->trace->id];

    /* ----------   tricky point   ---------
       in an ideal world, we would simply have added the item to events[id]
       however, if we do that, memory explodes rapidly
       so, we monitor in real time memory usage of each signal
       and if this usage becomes too large, we apply a compression strategy:
          - add the average instead of several samples
          - RLE compression (perfect for clocks)
    */

    //compress_and_add(packet->trace->id, dwt.begin, timestamp, dwt.d);
    //std::cout << "just_add trace " << packet->trace->id << " at " << timestamp << std::endl;
    just_add(packet->trace->id, dwt.begin, timestamp, dwt.d);
  }
  //else
  //  std::cout << "packet not added trace_id: " << packet->trace->id << std::endl;

  dwt.begin = timestamp;
  switch (packet->size)
  {
  case 1:
    dwt.d = (generic_data_t) * (uint8_t *)packet->data;
    break;
  case 2:
    dwt.d = (generic_data_t) * (uint16_t *)packet->data;
    break;
  case 4:
    dwt.d = (generic_data_t) * (uint32_t *)packet->data;
    break;
  case 8:
    dwt.d = (generic_data_t) * (uint64_t *)packet->data;
    break;
  default:
    // if packet data cannot be casted to uint32_t = compressed_data_t,
    // then ignore the packet
    return;
  }
  event_buffer[packet->trace->id] = dwt;
}

int Profiler_server::getSignalId(std::string path)
{
  // Returns the signal ID of the signal, depending on its path
  //std::cout << "path_mapping vector" << std::endl;
  /*for (auto elem : path_mapping) {
    if (elem.first.compare(std::string("/chip/soc/fc/state")) == 0 )
      std::cout << "*" << elem.first << " " << elem.second << std::endl;
  }
  */
  return path_mapping[path];
}

int Profiler_server::get_packet(trace_packet *packet)
{
  int x = trace_dumper_server::get_packet(packet);
  if (x == -1)
  {
    //std::cout << "[-] Receiving no packet " << std::endl;
    return -1; // read failed
  }

  //std::cout << "[-] Receiving packet " << std::endl;
  //packet->dump();
  // Packets are filtered depending on their type
  if (packet->header.type == ED_TYPE_REG_TRACE)
  {
    //std::cout << "Packet Header Type: " << "ED_TYPE_REG_TRACE" << std::endl;
    path_analysis(packet->trace->path.c_str(), packet);
    //std::cout << "mapping[" << packet->trace->path << "]= " << packet->reg_trace.id << std::endl;
    path_mapping[packet->trace->path] = packet->reg_trace.id;
    id_mapping[packet->reg_trace.id] = packet->trace->path;
    //packet->dump();
    //std::cout << "[-] Receiving packet ED_TYPE_REG_TRACE" << std::endl;
  }
  else if (packet->header.type == ED_TYPE_TRACE ||
           packet->header.type == ED_TYPE_TRACE_SET_0 ||
           packet->header.type == ED_TYPE_TRACE_SET_1)
  {
    //std::cout << "Packet Header Type: " << "ED_TYPE_TRACE || ED_TYPE_TRACE_SET_0 || ED_TYPE_TRACE_SET_1" << std::endl;
    // TODO add a filter to prevent memory from being filled to fast...
    //if (packet->trace->id == 635){
    //static uint64_t old_tmstp = 0;
    //std::cout << "cycle @ " << timestamp << "; diff = " << (timestamp - old_tmstp) << std::endl;
    //old_tmstp = timestamp;
    // << "!!! add_event " << packet->trace->path << " id" << packet->trace->id << std::endl;
    add_event(packet);
    //std::cout << "[-] Receiving packet: add_event " << std::endl;
    //}
    event_count[packet->trace->id]++;
    registration_done = true;
  }
  else if (packet->header.type == ED_TYPE_CONF)
  {
    //std::cout << "Packet Header Type: " << "ED_TYPE_CONF -- doing nothing" << std::endl;
    //std::cout << "!!! add_event " << packet->trace->path << std::endl;
    //add_event(packet);
  }
  else if (packet->header.type == ED_TYPE_TIMESTAMP8 ||
           packet->header.type == ED_TYPE_TIMESTAMP16 ||
           packet->header.type == ED_TYPE_TIMESTAMP32 ||
           packet->header.type == ED_TYPE_TIMESTAMP64)
  {
    //std::cout << "Packet Header Type: " << "ED_TYPE_TIMESTAMPXX -- doing nothing" << std::endl;
    //std::cout << "!!! add_event " << packet->trace->path  << std::endl;
    //add_event(packet);
  }
  //else
  //std::cout << "Packet Header Type: " << packet->header.type << std::endl;
  return x;
}

std::string Profiler_server::get_trace_txt_list()
{
  std::string txt = "";
  std::string token;
  std::vector<std::string> prefix;
  std::vector<Trace *> v;
  char delim = '/';
  for (auto t : traces)
  {
    v.push_back(t.second);
  }
  std::sort(v.begin(), v.end(), [](const Trace *t1, const Trace *t2) {
    return t1->path.compare(t2->path) < 0;
  });
  for (Trace *t : v)
  {
    std::stringstream ss(t->path);
    std::string cat;
    uint i = 0;
    //drop first '/'
    std::getline(ss, token, delim);
    while (std::getline(ss, token, delim) && i < prefix.size() && token == prefix[i])
    {
      i++;
    }
    prefix.resize(i);
    do
    {
      for (uint j = 0; j < prefix.size(); j++)
        txt += " ";
      prefix.push_back(token);
      txt += token + "\n";
    } while (std::getline(ss, token, delim));
  }
  return txt;
}

uint64_t Profiler_server::data_2_uint64(const trace_packet &packet)
{
  if (packet.size == 1)
    return *(uint8_t *)packet.data;
  if (packet.size == 2)
    return *(uint16_t *)packet.data;
  if (packet.size == 4)
    return *(uint32_t *)packet.data;
  if (packet.size == 8)
    return *(uint64_t *)packet.data;
  std::cout << "[-] Warning: decoding packet data to integer failed" << std::endl;
  return (uint64_t)-1; //error
}

double Profiler_server::data_2_double(const trace_packet &packet)
{
  if (packet.size == 4)
    return *(float *)packet.data;
  if (packet.size == 8)
    return *(double *)packet.data;
  return -1;
}

bool Profiler_server::contains_pc(const trace_packet &packet,
                                  int &core_id, uint32_t &pc_value)
{
  if (packet.header.type != ED_TYPE_TRACE)
    return false;
  if (is_pc.find(packet.trace->id) == is_pc.end() || (core_id = is_pc[packet.trace->id]) == -1)
    return false;
  if (packet.size == 4)
    pc_value = *((uint32_t *)packet.data);
  else
  {
    printf("Impossible to read pc from packet: invalid size (%d != 4)\n",
           (int)packet.size);
    pc_value = 0;
    return false;
  }
  return true;
}

bool Profiler_server::contains_state(const trace_packet &packet,
                                     int &core_id, bool &active_value)
{
  if (packet.header.type != ED_TYPE_TRACE)
    return false;
  if (is_state.find(packet.trace->id) == is_state.end() || (core_id = is_state[packet.trace->id]) == -1)
  {
    return false;
  }
  if (packet.size != 1)
  {
    printf("Impossible to read state from packet: invalid size (%d != 1)\n"
           "Packet is :",
           (int)packet.size);
    //packet.dump();

    return false;
  }
  active_value = (*((uint8_t *)packet.data) != 0);
  return true;
}

bool Profiler_server::contains_dma(const trace_packet &packet, bool &active_value)
{
  if (packet.header.type != ED_TYPE_TRACE)
    return false;
  if (is_dma[packet.trace->id])
  {
    active_value = (*((uint8_t *)packet.data) != 0);
    return true;
  }
  return false;
}


bool Profiler_server::contains_begin_stall(const trace_packet &packet,
                                           stall_trace_t &trace)
{
  if (packet.header.type == ED_TYPE_TRACE_SET_1)
  {
    if (contains_stall(packet, trace))
    {
#ifdef STALL_WARNING
      /* a core should not be stalled twice */
      if (trace.core_id < MAX_CORE_ID && core_stalled[trace.core_id])
      {
        std::cout << "[-] WARNING: core " << trace.core_id << " stalled twice";
        std::cout << std::endl;
        packet.dump();
      }
#endif
      core_stalled[trace.core_id] = true;
      return true;
    }
  }
  return false;
}

bool Profiler_server::contains_end_stall(const trace_packet &packet,
                                         stall_trace_t &trace)
{
  if (packet.header.type == ED_TYPE_TRACE_SET_0)
  {
    if (contains_stall(packet, trace))
    {
      if (trace.core_id < MAX_CORE_ID && !core_stalled[trace.core_id])
      {
#ifdef STALL_WARNING
        /* first packet might be intialization */
        if (!core_init[trace.core_id][trace.type])
        {
          core_init[trace.core_id][trace.type] = true;
        }
        else
        {
          std::cout << "[-] WARNING: core " << trace.core_id;
          std::cout << " unstalled be was not stalled" << std::endl;
          packet.dump();
        }
#endif // STALL_WARNING 
    // do not register a stall with undefined beginning
        return false;
      }
      core_stalled[trace.core_id] = false;
      return true;
    }
  }
  return false;
}

bool Profiler_server::contains_stall(const trace_packet &packet,
                                     stall_trace_t &trace)
{
  if (stall_mapping.find(packet.trace->id) != stall_mapping.end())
  {
    trace = stall_mapping[packet.trace->id];
    return true;
  }
  return false;
}

bool Profiler_server::contains_fc_period(const trace_packet &packet,
                                         uint64_t &period)
{
  if (packet.header.type != ED_TYPE_TRACE)
    return false;
  if (packet.trace->id != (int)fc_period_tr_id)
    return false;
  period = data_2_double(packet);
  return true;
}

bool Profiler_server::contains_cluster_period(const trace_packet &packet,
                                              uint64_t &period)
{
  if (packet.header.type != ED_TYPE_TRACE)
    return false;
  if (packet.trace->id != (int)cluster_period_tr_id)
    return false;
  period = data_2_double(packet);
  return true;
}

bool Profiler_server::contains_cluster_cycle(const trace_packet &packet)
{
  if (packet.header.type != ED_TYPE_TRACE)
    return false;
  return packet.trace->id == (int)cluster_cycle_id;
}
