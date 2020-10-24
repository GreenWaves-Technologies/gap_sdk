/*
 * Copyright (C) 2019 Green_waves Technologies
 * All rights reserved.
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
