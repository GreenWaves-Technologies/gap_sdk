#ifndef SAFE_HOTSPOT_FUNCTION_INFO_HPP
#define SAFE_HOTSPOT_FUNCTION_INFO_HPP

#include <string>
#include "profiler_backend_user.hpp"

struct SafeHotspotFunctionInfo
{
    SafeHotspotFunctionInfo(const int64_t duration_,
                            const float percentage_,
                            char* funcName_,
                            char* fileName_,
                            int line_): duration(duration_), percentage(percentage_), line(line_)
    {
        if(funcName_!=NULL)
            funcName=std::string(funcName_);
        else
            funcName=std::string("");

        if(fileName_!=NULL)
            fileName=std::string(fileName_);
        else
            fileName=std::string("");
    }

    SafeHotspotFunctionInfo(Hotspot_function_info * h):duration(h->duration), percentage(h->percentage), line(h->line) {

        if(h->function!=NULL)
            funcName=std::string(h->function);
        else
            funcName=std::string("");
        if(h->file!=NULL)
            fileName=std::string(h->file);
        else
            fileName=std::string("");
    }

    SafeHotspotFunctionInfo(const SafeHotspotFunctionInfo& h):duration(h.duration),percentage(h.percentage),
        funcName(std::string(h.funcName)),
        fileName(std::string(h.fileName)),
        line(h.line) {

    }
    int64_t duration;
    float percentage;
    std::string funcName;
    std::string fileName;
    int line;
};

#endif // SAFE_HOTSPOT_FUNCTION_INFO_HPP
