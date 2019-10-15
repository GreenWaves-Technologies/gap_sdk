/* vim: set ts=2 sw=2 expandtab: */
/*
 * Copyright (C) 2019 TU Kaiserslautern
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Author: Éder F. Zulian, TUK (zulian@eit.uni-kl.de)
 */

#ifndef __EMS_COMMON_H__
#define __EMS_COMMON_H__

#include <time.h>
#include <iomanip>
#include <cstdio>
#include <iostream>
#include <memory>

#include <tlm.h>

static ofstream fout("/dev/stdout");
#define debug(msg)  fout << "[" << std::setfill('0') << std::setw(12) << sc_core::sc_time_stamp().to_string() << "]" << "(" << __FILE__ << ":" << __LINE__ << ") -- " << __func__ << " -- " << msg << endl;

namespace ems {

std::string sh_exec(const char *cmd)
{
  std::string r;
  std::array<char, 128> buf;
  // The std::unique_ptr pipe owns the standard I/O stream returned by
  // popen(3) and uses pclose(3) as deleter when it goes out of context
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
  if (!pipe) {
    throw std::runtime_error("popen() failed");
  }
  while (fgets(buf.data(), buf.size(), pipe.get()) != nullptr) {
    r += buf.data();
  }
  return r;
}

} // namespace ems

#endif /* __EMS_COMMON_H__ */

