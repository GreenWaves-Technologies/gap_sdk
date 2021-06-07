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


#include <iostream>
#include <sys/prctl.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

#include "string.h"

#include "gdb_interface.hpp"

Gdb_interface::Gdb_interface(std::string elf_file){
  setup_gdb_child(elf_file);
}

Gdb_interface::~Gdb_interface(){
  kill(gdb_process, SIGKILL);
  waitpid(gdb_process, NULL, 0);
}

void Gdb_interface::setup_gdb_child(std::string elf_file){
  /* create a gdb child process to answer requests in real time using pipes */
  pid_t pid = 0;
  int inpipefd[2];
  int outpipefd[2];

  if (pipe(inpipefd) || pipe(outpipefd)){
    printf("[-] Error: impossible to open pipes to communicate with gdb: %s\n",
          strerror(errno));
  }
  pid = fork();
  if (pid == 0)
  {
    dup2(outpipefd[0], STDIN_FILENO);
    dup2(inpipefd[1], STDOUT_FILENO);
    dup2(inpipefd[1], STDERR_FILENO);

    //ask kernel to deliver SIGTERM in case the parent dies
    prctl(PR_SET_PDEATHSIG, SIGTERM);
    execlp("riscv32-unknown-elf-gdb", elf_file.c_str(),
        "--interpreter=mi", elf_file.c_str(), (char*) NULL);
    /* send error message through the pipe */
    printf("[-] Error: failed to launch gdb child process %s\n", strerror(errno));
    exit(1);
  }

  close(outpipefd[0]);
  close(inpipefd[1]);
  pipe_to_gdb = outpipefd[1];
  pipe_from_gdb = inpipefd[0];
  gdb_process = pid;
  // read and drop gdb initialization text
  gdb_read();
}

/* gdb mi output format -> classical format */
static std::string unescape_string(const std::string& in){
  int T = in.size();
  std::string out;
  for (int i = 0, j = 2; j && i < T; i++){
    if (in[i] == '\\'){
      if (i + 1 == T) break;
      i++;
      switch(in[i]){
        case 't':
          out.push_back('\t'); break;
        case 'n':
          out.push_back('\n'); break;
        case 'r':
          out.push_back('\r'); break;
        default:
          out.push_back(in[i]);
      }
    }
    // drop all non escaped "
    else if (in[i] != '\"') out.push_back(in[i]);
    else j--;
  }
  if (out.size() > 0 && out.back() == '"') out.pop_back();
  return out;
}

std::string Gdb_interface::gdb_read(){
  std::string answer;
  char buf[1024];
  const char* end_marker = "(gdb)";
  char data_prefix = '~';
  FILE* in = fdopen(pipe_from_gdb, "r");
  if (in == NULL){
    printf("[-] reading from gdb failed: %s\n", strerror(errno));
    return "";
  }
  while (fgets(buf, 1023, in)){
    if (strncmp(buf, end_marker, strlen(end_marker)) == 0) break;
    if (buf[0] != data_prefix) continue;
    answer += unescape_string(buf + 1);
  }
  return answer;
}

std::string Gdb_interface::gdb_request(std::string request){
  if (request.size() == 0) return "";
  if (request.back() != '\n') {
    request.push_back('\r');
    request.push_back('\n');
  }
  if (write(pipe_to_gdb, request.c_str(), request.size()) == -1){
    printf("[-] Sending command to gdb failed: %s\n", strerror(errno));
    return "";
  }
  return gdb_read();
}
