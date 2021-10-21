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

 #ifndef DATAMANAGER_HPP
#define DATAMANAGER_HPP

#include <string>
#include <thread>

#include <unordered_map>
#include <map>
#include <vector>
#include <deque>
#include <mutex>

#include "profiler_server.hpp"
#include "pc2source.hpp"
#include "gdb_interface.hpp"
#include "tldata.hpp"

/**
  @brief basic structure to record one execution of a given function
*/
typedef struct {
  const char* function_name;
  uint64_t t_start; /**< when the function is called */
  uint64_t t_end;   /**< when the function returns */
} Function_exec;

/**
  @brief statistics on stall reasons
*/
typedef struct {
  uint64_t lost_cycles[N_STALL_REASONS] = {0};
} Stall_stat;

/**
  @brief execution statistics for a function
*/
class Function_stat {
public:
  std::string name;
  uint n_calls;
  uint64_t tot_time;
  uint64_t tot_cycle;
  std::string file;
  Stall_stat stall_info;

  bool operator< (const Function_stat& f){
    return this->tot_time > f.tot_time;
  }
};


typedef std::vector<Function_stat> Function_table;

typedef std::map<std::string, Function_stat> Function_mapping;
typedef std::unordered_map<std::string, std::string> File_mapping;

/**
  @brief class to manage profiling data collected by a profiler_server
*/
class Data_manager {
public:
  const static int TLHeight;
  const static int fc_idx;
  const static int dma_idx;

  Data_manager();
  ~Data_manager();
  Data_manager(std::string path_to_fifo, std::string path_to_elf, uint64_t f_max);
  bool start_listening();
  std::string gdb_request(std::string request);
  std::string gdb_read();
  const TLData<g_compressed_data_t>* get_event_timestamps(int id) const;
  std::string get_trace_txt_list() const;
  bool trace_registration_done() const;
  bool add_trace_to_timeline(std::string path);
  void remove_trace_from_timeline(int absolute_index);
  uint64_t get_cluster_period() const {return cluster_period;}
  uint64_t get_fc_period() const {return fc_period;}
  const uint64_t* get_cycles_per_core() const {return cycles_per_core; }
  bool is_done() const {return done;}
  int getSignalId(std::string path);

  std::thread listening_thd;
  bool stop = false;
  bool tld_has_changed;
  std::vector<TLData<const char*> > signal_list;
  std::vector<int> signal_id;
  Function_mapping fctm;
  File_mapping file_m;
  std::vector<bool> isStateTraceActiv;
  std::vector<bool> isPcTraceActiv;
  std::string elf_file;
  Gdb_interface* gdb;
  std::vector<int> displayed_id;

  std::mutex data_mutex;

  // For issuing statistics
  // For storing signal rising edge detected
  std::unordered_map<uint32_t, bool> risingEdgeDetected;
  // for storing signals rising edge timestamp
  std::unordered_map<uint32_t, int> risingEdgeTime;  
  // for storing cumulative duty time of a signal
  std::unordered_map<uint32_t, int> signalDutyTime;  

private:

  bool isRisingEdge(const trace_packet &packet);
  bool isFallingEdge(const trace_packet &packet);
  void fillUpStats(const trace_packet &packet);

  // Following functions are used to merge state & pc traces on the state trace
  // ! Only state traces record their trace_id
  void manageStateTraceStarts(const trace_packet& packet, 
                            int core_id,
                            int trace_id);
  void manageStateTraceStops( const trace_packet& packet, 
                            int core_id,
                            int trace_id);
  void managePcTraceStops(const trace_packet& packet, 
                        int core_id);
  void managePcTraceStarts( const trace_packet& packet, 
                          const Pc_info& pi,
                          int core_id);      
                                                                                         
  void set_end_in_tl(int trace_id, uint64_t timestamp, int i);
  void listen();
  void update_data(const trace_packet& packet, const Pc_info& pi, int core_id, int trace_id);
  void update_dma(const trace_packet& packet, bool active, int trace_id);
  void update_stall_stats(const stall_event_t& stall);
  void init_tldata();
  void begin_stall(stall_trace_t trace, uint64_t timestamp);
  void end_stall(stall_trace_t trace, uint64_t timestamp);
  void begin_function_call(uint idx, uint64_t timestamp, const char* s, int trace_id);
  void end_function_call(uint idx, uint64_t timestamp, int trace_id);

  bool is_buffer_empty(uint idx);
  void clear_buffer_slot(uint idx);
  bool traceFound(int trace_id);
  

  Profiler_server* server = nullptr;
  Pc_mapping pcm;
  std::vector<uint64_t> stall_timestamp;
  std::vector<uint32_t> current_pc;
  std::vector<Function_exec> insertion_buffer;
  uint64_t fc_period;
  uint cluster_period;
  uint64_t* cycles_per_core = nullptr;;
  bool done = false;

};

void stop_server();
bool init_backend(std::string path_to_fifo, std::string path_to_elf,
                  uint64_t f_max);
int getSignalId(std::string path);

#endif //DATAMANAGER_HPP
