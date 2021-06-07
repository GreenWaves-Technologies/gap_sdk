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
