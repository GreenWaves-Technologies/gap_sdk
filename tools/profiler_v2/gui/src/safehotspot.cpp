#include "safehotspot.hpp"


#include <cstdlib>
#include <iostream>
#include <string>



SafeHotspot::SafeHotspot(const uint64_t pc,
                         const int64_t duration,
                         const int64_t count,
                         std::string const& funcName,
                         std::string const& inlineFuncName,
                         std::string const& fileName,
                         int line
                         );
