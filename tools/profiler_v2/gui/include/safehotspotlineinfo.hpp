#ifndef SAFEHOTSPOTLINEINFO_HPP
#define SAFEHOTSPOTLINEINFO_HPP

#include <string>
#include "profiler_backend_user.hpp"

struct SafeHotspotLineInfo
{
    SafeHotspotLineInfo(const int64_t duration_,
                        const int64_t count_,
                        float percentage_,
                        char* funcName_,
                        char* fileName_,
                        int line_): duration(duration_),count(count_), percentage(percentage_), line(line_)
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

    SafeHotspotLineInfo(Hotspot_line_info * h):duration(h->duration),count(h->count), percentage(h->percentage),
        line(h->line) {

        if(h->function!=NULL)
            funcName=std::string(h->function);
        else
            funcName=std::string("");

        if(h->file!=NULL)
            fileName=std::string(h->file);
        else
            fileName=std::string("");
    }

    SafeHotspotLineInfo(const SafeHotspotLineInfo& h):duration(h.duration),count(h.count), percentage(h.percentage),
        funcName(std::string(h.funcName)),
        fileName(std::string(h.fileName)),
        line(h.line) {

    }


    int64_t duration;
    int64_t count;
    float percentage;
    std::string funcName;
    std::string fileName;
    int line;
};

#endif // SAFEHOTSPOTLINEINFO_HPP
