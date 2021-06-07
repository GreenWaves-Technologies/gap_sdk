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
#include <cstring>
#include <unistd.h>

#include "backend_interface.hpp"
#include "datamanager.hpp"

extern Data_manager* core;

int count=0;

Function_table get_function_table(){
  //std::cout << "** get_function_table ** " << count << std::endl;
  count +=1;
  Function_table r;
  for (auto it : core->fctm) r.push_back(it.second);
  sort(r.begin(), r.end());
  return r;
}

/* return the name of the function that was executed on a given core at a given
 * timestamp. nullptr is returned if no such function exists.
*/
const char* function_at(int core_id, uint64_t timestamp){
  if (core_id >= (int) core->signal_list.size()) return nullptr;
  auto x = core->signal_list[core_id].between(timestamp, timestamp + 1, 1);
  if (! x.done()) {
    return (*x).d;
  }
  return nullptr;
}

const std::vector<TLData<const char*>> & get_timeline_data(){
  return core->signal_list;
}

const std::vector<int> & get_timeline_id() {
  return core->signal_id;
}

/* useful to know whether the GUI must refresh the timeline or not */
bool tldata_has_changed(){
  return core->tld_has_changed;
}

/* the gui can acknowledge */
void ack_tld_changes(){
  core->tld_has_changed = false;
}

/* useful to know the total length of the displayed timeline */
uint64_t get_max_time(){
  //std::cout << "** get_max_time **" << std::endl;
  uint64_t m = 1;
  for (auto& c: core->signal_list){
    uint64_t n = c.get_end_of_last_event();
    if (n > m) m = n;
  }
  return m;
}

uint get_nPe(){
  std::cout << "** get_nPe **" << std::endl;
  return Data_manager::TLHeight;
}

std::string get_source_code(std::string function_name){
  //std::cout << "** get_source_code **" << std::endl;
  std::string file = file_of(function_name);
  if (file == ""){
    std::cout << "** get_source_code **" << std::endl;
    std::cout << function_name << std::endl;
    std::cout << "no file" << std::endl;
    return std::string("");
  }
    
  return core->file_m[file];
}

int function2line_number(std::string function_name){
  // ask the gdb child process where the given function is in the source code
  std::string command = "info line " + function_name;

  std::string answer = core->gdb->gdb_request(command);
  char* line = strdup(answer.c_str());
  char *token = strtok(line, " ");

  if (token == nullptr || (token = strtok(nullptr, " ")) == nullptr){
    std::cout << "[-] Error: cannot interpret gdb output: " << line << std::endl;
    free(line);
    return 0;
  }

  char* endptr;
  int line_number = strtol(token, &endptr, 10);
  
  if (endptr == token || ! line_number){
    std::cout << "[-] Warning: no debugging information (please compile with -g option):"  << std::endl;
    return 0;
  }
  free(line);
  return line_number;
}

std::string file_of(std::string function_name){
  if (core->fctm.find(function_name) == core->fctm.end()) 
    return std::string("");
  return core->fctm[function_name].file;
}

std::string get_asm_code(std::string function_name){
  //std::cout << "** get_asm_code **" << std::endl;
  return core->gdb->gdb_request("disassemble " + function_name);
}

void lock_data_mtx(){
  //std::cout << "** lock_data_mtx **" << std::endl;
  core->data_mutex.lock();
}

void unlock_data_mtx(){
  //std::cout << "** unlock_data_mtx **" << std::endl;
  core->data_mutex.unlock();
}

const Function_stat* get_function_stat(std::string function_name){
  //std::cout << "** get_function_stat **" << std::endl;
  if (core->fctm.find(function_name) == core->fctm.end())
    return nullptr;
  return & core->fctm[function_name];
}

const TLData<g_compressed_data_t>* event_timestamps(int id){
  //std::cout << "** get_event_timestamps **" << std::endl;
  return core->get_event_timestamps(id);
}

const std::vector<int>& id_to_display(){
  return core->displayed_id;
}

std::string get_trace_txt_list() {
  //std::cout << "** get_trace_txt_list **" << std::endl;
  // we wait until the backend gets the registration of all traces
  while (!core->trace_registration_done()){
    usleep(100000);
  }
  return core->get_trace_txt_list();
}

bool add_trace_to_timeline(std::string path){
  //std::cout << "[+] adding " << path << std::endl;
  return core->add_trace_to_timeline(path);
}

void remove_trace_from_timeline(int index){
  core->remove_trace_from_timeline(index);
}

uint64_t get_cluster_period(){
  //std::cout << "** get_cluster_period **" << std::endl;
  return core->get_cluster_period();
}

uint64_t get_fc_period(){
  //std::cout << "** get_fc_period **" << std::endl;
  return core->get_fc_period();
}

std::vector<uint64_t> get_cycles_per_pe(){
  //std::cout << "** get_cycles_per_pe **" << std::endl;
  const uint64_t* x = core->get_cycles_per_core();
  if (x == nullptr){
    return std::vector<uint64_t>(std::max(0, Data_manager::dma_idx - 1), 0);
  }
  return std::vector<uint64_t>(x + 1, x + Data_manager::dma_idx);
}

bool datamanager_done(){
  return core->is_done();
}


int getSignalIdFromBackend(std::string path) {
  return core->getSignalId(path);
}
