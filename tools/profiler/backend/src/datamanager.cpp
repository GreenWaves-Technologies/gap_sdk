/*
 * Copyright (C) 2019 Green_waves Technologies
 * All rights reserved.
 */

#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <numeric>
#include <algorithm>

#include "string.h"
#include "datamanager.hpp"

// TODO: has to change for GAP9 
const int Data_manager::TLHeight = 9 + 1 + 1;
const int Data_manager::fc_idx = 0;
const int Data_manager::dma_idx = 10;
Data_manager* core;

Data_manager::Data_manager(){
  server = nullptr;
}

Data_manager::Data_manager(std::string path_to_fifo, std::string path_to_elf,
                          uint64_t f_max){
  elf_file = path_to_elf;
  std::cout << "[.] Executable file is: " << path_to_elf << std::endl;
  gdb = new Gdb_interface(path_to_elf);
  server = new Profiler_server(path_to_fifo, f_max);
  std::cout << "[.] gdb child set up" << std::endl;
  if (server->open())
  {
    std::cout << "Failed to open file " << path_to_fifo;
    std::cout << " with error: " << strerror(errno) << std::endl;
    server = nullptr;
    return;
  }
  parse_elf(path_to_elf, pcm);
  init_tldata();
}

Data_manager::~Data_manager(){
  delete gdb;
  gdb = nullptr;
  delete server;
  server = nullptr;
  delete[] cycles_per_core;
  cycles_per_core = nullptr;
}

void Data_manager::init_tldata(){

  // i just used to initialize all elements of the the signal_list 
  // with TLData 
  // This is just for containing core signals ...

  for (int i = 0; i < Data_manager::TLHeight; i++){
    signal_list.push_back(TLData<const char*>());
    // initialize corresponding signal id
    signal_id.push_back(0); 
    isStateTraceActiv.push_back(false);
    isPcTraceActiv.push_back(false);
    stall_timestamp.push_back(0);
    current_pc.push_back(0);
    insertion_buffer.push_back((Function_exec)
        { .function_name = "", .t_start = 0, .t_end = 0});
  }
  cycles_per_core = new uint64_t[dma_idx];
  for (int i = 0; i< dma_idx; i++) cycles_per_core[i] = 0;
}

const TLData<g_compressed_data_t>* Data_manager::get_event_timestamps(int id) const {
  if (server && server->events.find(id) != server->events.end()) {
    return & server->events[id];
  }
  return nullptr;
}

std::string Data_manager::get_trace_txt_list() const {
  if (server == nullptr) return "";
  return server->get_trace_txt_list();
}

bool Data_manager::trace_registration_done() const {
  return (server != nullptr && server->trace_registration_done());
}

bool Data_manager::traceFound(int trace_id) {
  // searching for trace_id in the displayed_id
  for (int id : displayed_id) {
    if (trace_id == id)
      return true;
  }
  return false;  
}


bool Data_manager::add_trace_to_timeline(std::string path) {
  //std::cout << "add_trace_to_timeline " << path << std::endl;
  if (server == nullptr) {
    std::cout << "[-] Server not started " << path << std::endl;
    return false;
  }
  /*
  if (server->path_mapping.find(path) == server->path_mapping.end()){
    std::cout << "[-] Error: impossible to find trace " << path << std::endl;
    return false;
  }
  
  if (server->events.find(server->path_mapping[path]) == server->events.end()){
    std::cout << "[-] Error: events not found for trace " << path << std::endl;
    return false;
  }
  */
  //std::cout << "[-] Add trace " << path << " to display " << std::endl;
  // Here, we need to add them just once! So we need to check that the trace 
  // has not been entered already
  if (!traceFound(server->path_mapping[path]))
    displayed_id.push_back(server->path_mapping[path]);
  return true;
}

void Data_manager::remove_trace_from_timeline(int index){
  if (index < 0) return;
  if (index >= (int) displayed_id.size()) return;
  displayed_id.erase(displayed_id.begin() + index);
}

int Data_manager::getSignalId(std::string path) {
  return server->getSignalId(path);
}

bool Data_manager::start_listening(){
  if (server == nullptr){
    printf("Error: impossible for the profiling server to listen\n");
    return false;
  }
  listening_thd = std::thread(&Data_manager::listen, this);
  stop = false;
  return true;
}

bool Data_manager::is_buffer_empty(uint idx){
  return (    insertion_buffer[idx].t_start == 0
          &&  insertion_buffer[idx].t_end == 0);
}

void Data_manager::clear_buffer_slot(uint idx){
  insertion_buffer[idx].t_start = 0;
  insertion_buffer[idx].t_end = 0;
}

void Data_manager::begin_function_call(uint idx, uint64_t timestamp, const char* s, int trace_id){
  //std::cout << " begin-function-call" << std::endl;
  insertion_buffer[idx] = {
    .function_name = s,
    .t_start = timestamp,
    .t_end = 0    // actually we don't know yet
  };
  signal_id[idx]=trace_id;
}

void Data_manager::end_function_call(uint idx, uint64_t timestamp, int trace_id){
  Function_exec& fe = insertion_buffer[idx];
  fe.t_end = timestamp;

  signal_list[idx].add_item(fe.t_start, fe.t_end, fe.function_name);
  signal_id[idx]= trace_id;
  
  fctm[std::string(fe.function_name)].tot_time += fe.t_end - fe.t_start;
  // TODO : update tot_cycle

#ifdef STALL_WARNING
  auto& s = fctm[std::string(fe.function_name)];
  uint64_t t = 0;
  for (int i = 0; i< N_STALL_REASONS; i++){
    t += s.stall_info.lost_cycles[i];
  }
  if (t > s.tot_time){
    std::cout << "WARNING stat error for function " << fe.function_name << std::endl;
    std::cout << t << " > " << s.tot_time << std::endl;
    for (int i = 0; i< N_STALL_REASONS; i++){
      std::cout << s.stall_info.lost_cycles[i] << "; ";
    }
    std::cout << std::endl;
    std::cout << "Time range: " << fe.t_start << " -> " << fe.t_end << std::endl;
    //exit(1);
  }
#endif

  clear_buffer_slot(idx);
}

void Data_manager::set_end_in_tl(int trace_id, uint64_t timestamp, int i){
  // i represents the core Nb
  //std::cout << "set_end_in_tl" << std::endl;
  int maxi = (i >= 0 ? i + 1 : signal_list.size());
  if (i < 0) i = 0;
  for (; i < maxi; i++) {
    if (!is_buffer_empty(i)){
      end_function_call(i,timestamp,trace_id);
    }
  }
}

void Data_manager::update_data(const trace_packet& packet, const Pc_info& pi,
                               int core_id, int trace_id){
  //std::cout << "update_data" << std::endl;
  data_mutex.lock();
  tld_has_changed = true;
  std::string name = std::string(pi.func);
  if (fctm.find(name) != fctm.end()){
    fctm[name].n_calls++;
  } else {
    fctm[name] = {
      .name = name,
      .n_calls = 1,
      .tot_time = 0,
      .tot_cycle = 0,
      .file = std::string(pi.file),
      .stall_info = Stall_stat()
    };
  }
  if (!is_buffer_empty(core_id)) {
      end_function_call(core_id, packet.timestamp, trace_id);
  }

  signal_id[core_id]=trace_id;

  begin_function_call(core_id, packet.timestamp, pi.func, trace_id);

  // TODO is this really correct?
  stall_timestamp[core_id] = packet.timestamp;

  if (file_m.find(pi.file) == file_m.end()){
    file_m[pi.file] = read_file(pi.file);
  }
  data_mutex.unlock();
}

void Data_manager::update_dma(const trace_packet& packet, bool active, int trace_id){
  tld_has_changed = true;
  if (active && is_buffer_empty(Data_manager::dma_idx)){
    data_mutex.lock();
    signal_id[Data_manager::dma_idx]=trace_id;
    begin_function_call(Data_manager::dma_idx, packet.timestamp, "dma", trace_id);
    data_mutex.unlock();
    return;
  }
  if (!active && !is_buffer_empty(Data_manager::dma_idx)){
    data_mutex.lock();
    end_function_call( Data_manager::dma_idx, packet.timestamp, trace_id);
    data_mutex.unlock();
  }
}

void Data_manager::update_stall_stats(const stall_event_t& stall){
  if (pcm.find(stall.pc) != pcm.end()
      && fctm.find(pcm[stall.pc].func) != fctm.end()){
    auto& s = fctm[pcm[stall.pc].func].stall_info;
    s.lost_cycles[(uint) stall.trace.type] += stall.cycle_penalty;
  }
}

void Data_manager::begin_stall(stall_trace_t trace, uint64_t timestamp){
  stall_timestamp[trace.core_id] = timestamp;
}

void Data_manager::end_stall(stall_trace_t trace, uint64_t timestamp){
  int penalty = timestamp - stall_timestamp[trace.core_id]; //TODO convert penalty in cycles
  if (penalty <= 0) penalty = 1; // // should not happen
  update_stall_stats({
    .trace = trace,
    .pc = current_pc[trace.core_id],
    .cycle_penalty = penalty
  });
}

void Data_manager::manageStateTraceStarts(const trace_packet& packet, 
                            int core_id,
                            int trace_id) {
  // Dealing with state trace starting for different 
  // configurations
  //std::cout << "[.] Listen: manageStateTraceStarts core_id " << core_id << std::endl;
  //std::cout << "isStateTraceActiv[core_id]: " << isStateTraceActiv[core_id]<< std::endl;
  //std::cout << "isPcTraceActiv[core_id]: " << isPcTraceActiv[core_id]<< std::endl;
  tld_has_changed =true;
  if(!isStateTraceActiv[core_id] && isPcTraceActiv[core_id]){
      // starts a new insertion buffer for pc traces
      // As pc trace is active, the insertion buffer exists 
      // and we just have to update the timestamp
      insertion_buffer[core_id].t_start = packet.timestamp;
  } else if (!isStateTraceActiv[core_id] && !isPcTraceActiv[core_id]) {
      // starts a new insertion buffer for pc traces
      insertion_buffer[core_id] = {
          .function_name = "",
          .t_start = packet.timestamp,
          .t_end = 0    // actually we don't know yet
      };
      //std::cout << "signal_id " << core_id << " " << trace_id << std::endl;
      signal_id[core_id]=trace_id;
  }

  // Finally set state trace active
  isStateTraceActiv[core_id] = true;

}

void Data_manager::manageStateTraceStops( const trace_packet& packet, 
                            int core_id,
                            int trace_id){
  // Dealing with a state trace that stops in different 
  // configurations
  //std::cout << "[.] Listen: manageStateTraceStops " << core_id << std::endl;
  //std::cout << "isStateTraceActiv[core_id]: " << isStateTraceActiv[core_id]<< std::endl;
  //std::cout << "isPcTraceActiv[core_id]: " << isPcTraceActiv[core_id]<< std::endl;

  if ((isStateTraceActiv[core_id] && isPcTraceActiv[core_id]) ||
      (isStateTraceActiv[core_id] && !isPcTraceActiv[core_id])) {
    // Close the current insertion buffer
    Function_exec& fe = insertion_buffer[core_id];
    fe.t_end = packet.timestamp;
    signal_list[core_id].add_item(fe.t_start, fe.t_end, fe.function_name);
    //std::cout << "Function " << fe.function_name << " " << fe.t_start << " " << fe.t_end  << std::endl;
    //std::cout << "signal_id " << core_id << " " << trace_id << std::endl;
    signal_id[core_id]= trace_id;
    fctm[std::string(fe.function_name)].tot_time += fe.t_end - fe.t_start;

    if (isPcTraceActiv[core_id])
      // update the insertion buffer for the timestamp
      insertion_buffer[core_id].t_start=packet.timestamp;
    else 
      clear_buffer_slot(core_id);
    }
  isStateTraceActiv[core_id] = false;
}

void Data_manager::managePcTraceStops(const trace_packet& packet, 
                        int core_id,
                        int trace_id) {
  //std::cout << "[.] Listen: managePcTraceStops " << core_id << std::endl;
  //std::cout << "isStateTraceActiv[core_id]: " << isStateTraceActiv[core_id]<< std::endl;
  //std::cout << "isPcTraceActiv[core_id]: " << isPcTraceActiv[core_id]<< std::endl;
  if (isStateTraceActiv[core_id] && isPcTraceActiv[core_id]) {
      // Close the insertion buffer
      Function_exec& fe = insertion_buffer[core_id];
      fe.t_end = packet.timestamp;
      signal_list[core_id].add_item(fe.t_start, fe.t_end, fe.function_name);
      //std::cout << "Function " << fe.function_name << " " << fe.t_start << " " << fe.t_end  << std::endl;
      //std::cout << "signal_id " << core_id << " " << trace_id << std::endl;
      signal_id[core_id]= trace_id;
      // Maybe not to do in the second case ..
      fctm[std::string(fe.function_name)].tot_time += fe.t_end - fe.t_start;
      clear_buffer_slot(core_id);
      // Open a new insertion buffer with no function name
      insertion_buffer[core_id] = {
          .function_name = "",
          .t_start = packet.timestamp,
          .t_end = 0    // actually we don't know yet
      };
  }
  if (!isStateTraceActiv[core_id] && isPcTraceActiv[core_id]) {
    // just clearing the insertion_buffer
    clear_buffer_slot(core_id);
  }
  // Finally set PC trace not active
  isPcTraceActiv[core_id] = false;
}

void Data_manager::managePcTraceStarts( const trace_packet& packet, 
                          const Pc_info& pi,
                          int core_id, 
                          int trace_id) {
    // In this function, we take care of the different configurations 
    // in which a pc trace can start
    //std::cout << "[.] Listen: managePcTraceStarts " << core_id << std::endl;
    //std::cout << "isStateTraceActiv[core_id]: " << isStateTraceActiv[core_id]<< std::endl;
    //std::cout << "isPcTraceActiv[core_id]: " << isPcTraceActiv[core_id]<< std::endl;
    // Get the current debug info
    data_mutex.lock();
    tld_has_changed =true;
    // get current function name
    std::string name = std::string(pi.func);
    //std::cout << "function name: " << name << std::endl;
    // Checking if it's needed to register the function
    if (fctm.find(name) != fctm.end()){
      // new name has been found in fctm: count nb of times it has been called
      fctm[name].n_calls++;
    } else {
      // First time this function has been called
      // Create its data in the list
      fctm[name] = {
        .name = name,
        .n_calls = 1,
        .tot_time = 0,
        .tot_cycle = 0,
        .file = std::string(pi.file),
        .stall_info = Stall_stat()
      }; // finished getting debug info
    }

     if (!isStateTraceActiv[core_id] && isPcTraceActiv[core_id]) {
       // In this case, just change the function name in the insertion buffer
       // and t_start
       insertion_buffer[core_id] = {
            .function_name = pi.func,
            .t_start = packet.timestamp,  
            .t_end = 0    // actually we don't know yet
        };
     }

     if (!isStateTraceActiv[core_id] && isPcTraceActiv[core_id]) {
         insertion_buffer[core_id] = {
            .function_name = pi.func,
            .t_start = packet.timestamp,
            .t_end = 0    // actually we don't know yet
        };
     }

    if ((isStateTraceActiv[core_id] && !isPcTraceActiv[core_id]) ||
        (isStateTraceActiv[core_id] && isPcTraceActiv[core_id])) {

        // We need to add an item to the signal_list 
        // and close the current insertion buffer 
        Function_exec& fe = insertion_buffer[core_id];
        fe.t_end = packet.timestamp;
        signal_list[core_id].add_item(fe.t_start, fe.t_end, fe.function_name);
        //std::cout << "signal_id " << core_id << " " << trace_id << std::endl;
        signal_id[core_id]= trace_id;
        fctm[std::string(fe.function_name)].tot_time += fe.t_end - fe.t_start;
        clear_buffer_slot(core_id);

        // Open a new insertion buffer as the state trace remains activ
        // pcTrace becomes activ
        insertion_buffer[core_id] = {
            .function_name = pi.func,
            .t_start = packet.timestamp,
            .t_end = 0    // actually we don't know yet
        };

        // TODO is this really correct?
        stall_timestamp[core_id] = packet.timestamp;

        // read function source file
        if (file_m.find(pi.file) == file_m.end()){
          file_m[pi.file] = read_file(pi.file);
        }
    }

    if (!isStateTraceActiv[core_id] && !isPcTraceActiv[core_id]) {
      // In this case, we just create an insertion buffer
      // but its timestamp will be set later 
      // Open a new insertion buffer -- begin_function_call
      insertion_buffer[core_id] = {
          .function_name = pi.func,
          .t_start = 0, // not set yet
          .t_end = 0   // actually we don't know yet
      };
      //std::cout << "signal_id " << core_id<< " " << trace_id << std::endl;
      signal_id[core_id]=trace_id;
      // end begin_function_call
    }
    
  // Finally set PC trace active
  isPcTraceActiv[core_id] = true;

  // release mutex
  data_mutex.unlock();
}

void Data_manager::listen(){
  int core_id;
  uint32_t pc;
  uint64_t hit = 0, miss = 0;
  uint64_t period;
  bool active; // ???
  //bool just_woke_up[isStateTraceActiv.size()]; // ???
  stall_trace_t stall;
  //bool isNotStateTrace=true;


 //std::cout << "[.] begin listen()" << std::endl;

 while(! stop)
  {
    
    trace_packet packet;
    if (server->get_packet(&packet)){
      //std::cout << "[.] Got last packet" << std::endl;
      break;
    }
    //std::cout << "[.] Listen: New Packet" << std::endl;
    //packet.dump();
    if (server->contains_state(packet, core_id, active)){
        if ((int) isStateTraceActiv.size() <= core_id) continue;
        // set it activ or not activ
        if (active && !isStateTraceActiv[core_id]){
            manageStateTraceStarts(packet, core_id, server->path_mapping[packet.trace->path]);
        }
        else if (!active && isStateTraceActiv[core_id])
            manageStateTraceStops(packet, core_id, server->path_mapping[packet.trace->path]);
    }// end treating state traces

    if (server->contains_pc(packet, core_id, pc)) {
        if (TLHeight <= core_id) { // should not happen
        std::cout << "[-] error with core " << core_id << " / " << TLHeight << std::endl;
        continue;
        }
        // set it activ or not activ 
        // fill in the program counter
        current_pc[core_id] = pc;
        if (core->pcm.find(pc) == core->pcm.end()){
            // in this case, it holds no information
            //isNotStateTrace=true;
            miss++;
            managePcTraceStops(packet, core_id, server->path_mapping[packet.trace->path]);
            // see what to do 
            //set_end_in_tl(server->path_mapping[packet.trace->path], packet.timestamp, core_id,isNotStateTrace);
            /* to display unrecognized sections on the timeline */
            /*data_mutex.lock();
            signal_list[10].push_back({.function_name = "unknown", .t_start = packet.timestamp / time_convertion, .t_end = packet.timestamp / time_convertion + 1});
            data_mutex.unlock();*/
        } else {
            // in this case, the programm counter (pc) holds some information
            hit++;
            core->pcm[pc].count++;
            // if buffer is empty or insertion_buffer function name is different
            // then PC trace starts
            if (is_buffer_empty(core_id) ||
               strcmp(core->pcm[pc].func, insertion_buffer[core_id].function_name)){
                managePcTraceStarts(packet, core->pcm[pc], core_id, server->path_mapping[packet.trace->path]);
            }
        }
    }
      
    // end treating pc traces

    if (server->contains_fc_period(packet, period)){
      fc_period = period;
    }
    if (server->contains_cluster_period(packet, period)){
      cluster_period = period;
    }

    if (server->contains_dma(packet, active)){
      update_dma(packet, active,server->path_mapping[packet.trace->path] );
    }

    if (server->contains_begin_stall(packet, stall)){
      //packet.dump();
      begin_stall(stall, server->get_timestamp());
    }
    else if (server->contains_end_stall(packet, stall) && isStateTraceActiv[stall.core_id]){
      //packet.dump();
      end_stall(stall, server->get_timestamp());
    }
    
    if (server->contains_cluster_cycle(packet)){
      for (int i = 1; i < dma_idx; i++){
        if (isStateTraceActiv[i]) cycles_per_core[i]++;
      }
    }
  }
  done = true;
  std::cout << "[+] metrics collect done (" << hit << " hits ; " << miss;
  std::cout << " misses" << std::endl;

}


bool init_backend(std::string path_to_fifo, std::string path_to_elf,
                  uint64_t f_max){
  std::cout << "init_backend " << std::endl;
  core = new Data_manager(path_to_fifo, path_to_elf, f_max);
  return core->start_listening();
}

void stop_server(){
  core->stop = true;
  delete core->gdb;
  core->listening_thd.join();
}

int getSignalId(std::string path) {
  // Returns the signal ID of the signal, depending on its path
  return core->getSignalId(path);
}
