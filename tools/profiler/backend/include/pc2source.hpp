/*
 * Copyright (C) 2019 Green_waves Technologies
 * All rights reserved.
 */

#ifndef PC2SOURCE_HPP
#define PC2SOURCE_HPP

#include <unordered_map>
#include <string>
#include <fstream>
#include <sstream>

/**
  @brief struct to store information related to a given pc value (i.e. an
         assembly instr)
*/
typedef struct {
  int line;
  const char* func;
  const char* inline_func;
  const char* file;
  uint64_t tot_time;
  uint64_t count;
  /* add assembly ? */
} Pc_info;


/** @brief maps each pc value to the associated pieces of information */
typedef std::unordered_map<uint32_t, Pc_info> Pc_mapping;


void parse_elf(std::string elf_file, Pc_mapping& pcm);
std::string read_file(std::string filename);

#endif // PC2SOURCE_HPP
