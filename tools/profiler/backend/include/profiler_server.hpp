/*
 * Copyright (C) 2019 Green_waves Technologies
 * All rights reserved.
 */

#ifndef PROFILER_SERVER_HPP
#define PROFILER_SERVER_HPP


#include "trace_dumper.hpp"
#include "trace_dumper_profiling.hpp"
#include "tldata.hpp"

/* define to active runtime checks when receiving stall information */
//#define STALL_WARNING

#define MAX_CORE_ID 1000

/**
    @brief type to store an event of a trace of any type.

    If sizeof(compressed_data_t) is large, it increases memory usage significantly
    If sizeof(compressed_data_t) is small (as here), some trace cannot be stored
*/
typedef uint32_t generic_data_t;
typedef compressed_data_t<generic_data_t> g_compressed_data_t;
typedef decompressed_data_t<generic_data_t> g_decompressed_data_t;

/**
  @brief structure to mark the beginning of an event of any type
*/
typedef struct {
  Data_with_time<g_compressed_data_t> compr_data;
  uint32_t average_threshold;
} Event_record;

/**
  @brief  an extension of trace_dumper_server with advanced packet analysis
          methods.
 */
class Profiler_server : public trace_dumper_server
{
public:
  /**
    creates a new Profiler_server
    @param file: the file to read packets from
    @param f_max: maximum number of packets stored per second per signal.
            Beyond this threshold, there are 2 possibilities:
              - a lossless compression method (e.g. RLE) can be used to
                store all packets without storing more than the equivalent of
                f_max packets per second
              - otherwise, a lossy compression is used to maintain the storing
                rate at f_max packets per second
    @param tolerance: tolerated relative difference between measured frequency
           of a trace and the ratio f_max / average_threshold. Don't change it
           if you are not familiar with the average compression method we use
  */
  Profiler_server(std::string file, uint64_t f_max, double tolerance=1.3);
  virtual ~Profiler_server();

  int get_packet(trace_packet* packet) override;


  /**
    returns true and sets core_id and pc_value if packet contains a pc for core
    core_id. Returns false and does nothing otherwise
  */
  bool contains_pc(const trace_packet& packet, int& core_id, uint32_t& pc_value);

  /** same as contains_pc but for a state (active / inactive) */
  bool contains_state(const trace_packet& packet, int& core_id, bool& active_value);

  /** same as contains_pc but for dma activity */
  bool contains_dma(const trace_packet& packet, bool& active_value);

  /**
      if packet contains a rising edge of a stall signal, sets trace accordingly
      and returns true.
      Otherwise, does nothing and returns false.
      If STALL_WARNING is defined, some sanity checks are performed (eg, a core
      cannot receive a stall rising edge if it is already stalled)
    */
  bool contains_begin_stall(const trace_packet& packet, stall_trace_t& trace);

  /** same as contains_end_stall but for a negative edge */
  bool contains_end_stall(const trace_packet& packet, stall_trace_t& trace);

  /** if packet contains the fc period, sets period and returns true. */
  bool contains_fc_period(const trace_packet& packet, uint64_t& period);

  /** same as contains_fc_period, butfor the cluster */
  bool contains_cluster_period(const trace_packet& packet, uint64_t& period);

  /** returns true if the packet indicates that the cluster begins a new cycle */
  bool contains_cluster_cycle(const trace_packet& packet);

  /**
      @return a text description of all registered traces.
              traces whose prefixes match are grouped in a hierarchical manner.
              (print one output of these function to understand)
  */
  std::string get_trace_txt_list();

  /** @returns true iff all traces have been registered in the server (does not
              mean that all data for all traces has arrived) */
  bool trace_registration_done() const { return registration_done; };

  // Not implemented
  int id_from_line_number(int line_number);

  // events Buffer
  std::unordered_map<uint32_t, TLData<g_compressed_data_t>> events;

  std::unordered_map<std::string, int> path_mapping;
  std::unordered_map< int, std::string> id_mapping;

  std::unordered_map<uint32_t, uint64_t> event_count;

  int getSignalId(std::string path);

private:
  void path_analysis(const char* path, const trace_packet* packet);
	bool contains_stall(const trace_packet& packet, stall_trace_t& trace);
  void add_event(const trace_packet* packet);
  uint64_t data_2_uint64(const trace_packet& packet);
  double data_2_double(const trace_packet& packet);

  /**
    @return true iff it's possible to store e1 and e2 using a rle compression
            In other words, returns true if e1 and e2 describe the same kind
            of event at a different timestamp. The end of e1
            must match the beginning of e2 for the compression to work.
            Warning: match_rle(e1, e2) != math_rle(e2, e1)
  */
  bool match_rle(const Data_with_time<g_compressed_data_t>& e1,
                 const Data_with_time<g_compressed_data_t>& e2);

  /**
    adds e to the rle compression buffer, in order for that event to be rle
    compressed before being sent to its TLData structure.
  */
  int flush_to_rle_buffer(uint32_t trace_id, const Event_record& e);

  /**
    instead of compressing events to their TLData, we can use this function
    just to add events to the TLData
  */
  void just_add(uint32_t trace_id, uint64_t begin,
                uint64_t end, generic_data_t data);

  /**
    instead of adding events directly to their TLData, we can use this function
    to compress events before being added
  */
  void compress_and_add(uint32_t trace_id, uint64_t begin, uint64_t end,
                        generic_data_t data);


  void update_avg_threshold(uint32_t trace_id);

  /* is_pc[id] = i 	if trace id tracks pc of core i
               = -1 	otherwise
     is_state: idem but for the activity of a core */
 std::unordered_map<uint32_t, int> is_pc;
 std::unordered_map<uint32_t, int> is_state;
 std::unordered_map<uint32_t, uint32_t> is_dma;
 std::unordered_map<uint32_t, stall_trace_t> stall_mapping;
 uint32_t cluster_period_tr_id;
 uint32_t cluster_cycle_id;
 uint32_t fc_period_tr_id;
 uint32_t fc_cycle_id;
 regex_t dma_regex;
 regex_t stall_regex;

 /*
    when an event comes from gvsoc, it follows the following path:
    gvsoc => event_buffer => average_buffer => rle_buffer => events[trace_id]
    average_buffer and rle_buffer are used to compress data before storing it
 */
 std::unordered_map<uint32_t, Data_with_time<generic_data_t> > event_buffer;
 std::unordered_map<uint32_t, Event_record> average_buffer;
 std::unordered_map<uint32_t, Event_record> rle_buffer;
 std::unordered_map<uint32_t, std::deque<uint64_t> > entry_timestamps;
 double f_max;
 double tolerance;

 volatile bool registration_done = false;
 bool core_stalled[MAX_CORE_ID] = {0};
 bool core_init[MAX_CORE_ID][N_STALL_REASONS] = {{0}};

 // specifies we are getting packets in slow or fast mode
 // In fast mode, events are not compressed anymore
 bool gvsocSlowMode = false; 

};


#endif  //PROFILER_SERVER_HPP
