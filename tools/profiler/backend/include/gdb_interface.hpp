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


#ifndef GDB_INTERFACE_HPP
#define GDB_INTERFACE_HPP

class Gdb_interface{
public:
  Gdb_interface(std::string elf_file);
  ~Gdb_interface();
  std::string gdb_request(std::string request);

private:
  std::string gdb_read();
  void setup_gdb_child(std::string elf_file);

  int pipe_to_gdb;
  int pipe_from_gdb;
  pid_t gdb_process;

};

#endif  //GDB_INTERFACE_HPP
