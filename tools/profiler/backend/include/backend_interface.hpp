#ifndef BACKEND_INTERFACE_HPP
#define BACKEND_INTERFACE_HPP

#include <string>
#include <vector>
#include <deque>
#include <cstdint>

#include "datamanager.hpp"

/* when the GUI reads data from the backend, the same data might be written at
 * the same time by an other thread if new data comes from gvsoc. Therefore, you
 * must use the following mutex based functions when reading non thread safe
 * data. For example, it makes sure that a vector is not read while a new element
 * is pushed back (push_back might cause a realloc, so such a scenerio is
 * definitively not thread safe).
*/
void lock_data_mtx();
void unlock_data_mtx();


Function_table get_function_table();
const char* function_at(int core_id, uint64_t timestamp);

const std::vector<TLData<const char*>>& get_timeline_data();
const std::vector<int> & get_timeline_id();
uint get_nPe();

std::string get_source_code(std::string function_name);
std::string get_asm_code(std::string function_name);
std::string file_of(std::string function_name);
int function2line_number(std::string function_name);


bool tldata_has_changed();
void ack_tld_changes();

uint64_t get_max_time();
const Function_stat* get_function_stat(std::string function_name);
const TLData<g_compressed_data_t>* event_timestamps(int id);
std::string get_trace_txt_list();

bool add_trace_to_timeline(std::string path);
void remove_trace_from_timeline(int index);
const std::vector<int>& id_to_display();

uint64_t get_cluster_period();
uint64_t get_fc_period();

std::vector<uint64_t> get_cycles_per_pe();
bool datamanager_done();

int getSignalIdFromBackend(std::string path);

#endif //BACKEND_INTERFACE_HPP
