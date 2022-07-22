#ifndef SAFEHOTSPOT_HPP
#define SAFEHOTSPOT_HPP

#include <string>
#include "profiler_backend_user.hpp"

struct SafeHotspot
{
    SafeHotspot(const uint64_t pc_,
                const int64_t duration_,
                const int64_t count_,
                float percentage_,
                char* funcName_,
                char* inlineFuncName_,
                char* fileName_,
                int line_): pc(pc_),duration(duration_),count(count_), percentage(percentage_), line(line_)
    {
        if(funcName_!=NULL)
            funcName=std::string(funcName_);
        else
            funcName=std::string("");
        if(inlineFuncName_!=NULL)
            inlineFuncName=std::string(inlineFuncName_);
        else
            inlineFuncName=std::string("");
        if(fileName_!=NULL)
            fileName=std::string(fileName_);
        else
            fileName=std::string("");
    }

    SafeHotspot(Hotspot_info * h):pc(h->pc),duration(h->duration),count(h->count), percentage(h->percentage),
        line(h->line) {

        if(h->function!=NULL)
            funcName=std::string(h->function);
        else
            funcName=std::string("");
        if(h->inlined_function!=NULL)
            inlineFuncName=std::string(h->inlined_function);
        else
            inlineFuncName=std::string("");

        if(h->file!=NULL)
            fileName=std::string(h->file);
        else
            fileName=std::string("");
    }

    SafeHotspot(const SafeHotspot& h):pc(h.pc),duration(h.duration),count(h.count), percentage(h.percentage),
        funcName(std::string(h.funcName)),
        inlineFuncName(std::string(h.inlineFuncName)),
        fileName(std::string(h.fileName)),
        line(h.line) {

    }

    uint64_t pc;
    int64_t duration;
    int64_t count;
    float percentage;
    std::string funcName;
    std::string inlineFuncName;
    std::string fileName;
    int line;
};

#endif // SAFEHOTSPOT_HPP
