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


#include "pc2source.hpp"
#include <string.h>
#include <algorithm>
#include <vector>
#include <iostream>
#include <regex.h>


void iss_register_debug_info(const char *binary, Pc_mapping& pcm)
{
  FILE *file = fopen(binary, "r");
  if (file == nullptr){
    printf("[-] Impossible to open %s\n", binary);
    printf("%s\n", strerror(errno));
    return;
  }

  char* line = NULL;
  size_t len = 0;
  while (getline(&line, &len, file) != -1)
  {
    char *token = strtok(line, " ");
    char *tokens[5];
    int index = 0;
    while (token)
    {
      tokens[index++] = token;
      token = strtok(NULL, " ");
    }
    if (index == 5) {
      pcm[strtol(tokens[0], NULL, 16)] = (Pc_info) {
        .line = (int) atoi(tokens[4]),
        .func = strdup(tokens[1]),
        .inline_func = strdup(tokens[2]),
        .file = strdup(tokens[3]),
        .tot_time = 0,
        .count = 0
      };
    }
  }
  free(line);
}

void parse_elf(const std::string elf_file, Pc_mapping& pcm){
  std::cout << "[.] parsing elf " << std::endl;
  std::string info_file = std::string(basename(elf_file.c_str())) + ".debug_info";
  std::string command = "pulp-pc-info --file " + elf_file;
  command += " --all-file " + info_file;
  std::cout << "[.] " << command << std::endl;
  if (system(command.c_str())){
    std::cout << "[-] Error: impossible to generate .Debug_info file" << std::endl;
    return;
  }
  std::cout << "[+] parse_elf OK" << std::endl;
  iss_register_debug_info(info_file.c_str(), pcm);
}

std::string read_file(std::string filename){
  std::ifstream t(filename);
  if (!t) return "";
  std::stringstream buf;
  buf << t.rdbuf();
  return buf.str();
}
