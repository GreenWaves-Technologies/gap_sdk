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

/* 
 * Authors: Germain Haugou, GreenWaves Technologies (germain.haugou@greenwaves-technologies.com)
 */

#include "vp/vp.hpp"
#include "vp/trace/event_dumper.hpp"
#include <string.h>

static unsigned int get_bit(uint8_t *value, int i) {
  return (value[i/8] >> (i%8)) & 1;
}

vp::Vcd_file::Vcd_file(vp::Event_dumper *dumper, string path)
{
  file = fopen(path.c_str(), "w");
  if (file == NULL)
  {
    dumper->comp->get_engine()->fatal("Error while opening VCD file (path: %s, error: %s)\n", path.c_str(), strerror(errno));
  }

  fprintf(file, "%s", "\n\
$timescale 1ps $end\n\
");
}


string vp::Vcd_file::parse_path(string path, bool begin)
{
  std::string delim = "/";
  
  auto start = 0U;
  auto end = path.find(delim);
  while (end != std::string::npos)
  {
    if (end != start)
    {
      if (begin) {
        fprintf(file, "$scope module %s $end\n", path.substr(start, end - start).c_str());
      } else {
        fprintf(file, "$upscope $end\n");
      }
    }
    start = end + delim.length();
    end = path.find(delim, start);
  }
  return path.substr(start, end);
}

void vp::Vcd_file::add_trace(string path, int id, int width, bool is_real, bool is_string)
{
  string name = parse_path(path, true);
  
  if (is_real)
    fprintf(file, "$var real 64 %d %s $end\n", id, name.c_str());
  else
    fprintf(file, "$var wire %d %d %s $end\n", width, id, name.c_str());

  parse_path(string(path), false);
}

void vp::Vcd_file::dump(int64_t timestamp, int id, uint8_t *event, int width, bool is_real, bool is_string, uint8_t flags, uint8_t *flag_mask)
{
  if (!header_dumped)
  {
    header_dumped = true;
    fprintf(file, "%s", "\n\
$enddefinitions $end\n\
$dumpvars\n");    
    fprintf(file, "$end\n");
  }

  if (last_timestamp != timestamp) {
    last_timestamp = timestamp;
    fprintf(file, "#%ld\n", timestamp);
  }

  if (is_real)
  {
    fprintf(file, "r%f %d\n", *(double *)event, id);
  }
  else if (is_string)
  {
    fprintf(file, "s");
    fwrite(event, 1, width/8-1, file);
    fprintf(file, " %d\n", id);
  }
  else if (width > 1) {
    int i;
    char str[width+1+1];
    char *str_ptr = str;
    *str_ptr++ = 'b';

    if (event) {
      for (int i=width-1; i>=0; i--)
      {
        *str_ptr++ = get_bit(event, i) + '0';
      }
      *str_ptr = 0;
    }
    else
    {
      for (i=0; i<width; i++)
        str[i+1] = 'x';
    }

    fprintf(file, "%s %d\n", str, id);
  }
  else
  {
    if (event) {
      fprintf(file, "%u%d\n", get_bit(event, 0), id);
    } else {
      fprintf(file, "x%d\n", id);
    }
  }
}

void vp::Vcd_file::close()
{
  printf("CLOSE\n");
  fclose(file);
}
