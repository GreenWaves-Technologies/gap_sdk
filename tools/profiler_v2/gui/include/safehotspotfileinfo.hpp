#ifndef SAFE_HOTSPOT_FILE_INFO_HPP
#define SAFE_HOTSPOT_FILE_INFO_HPP

#include <string>
#include "profiler_backend_user.hpp"

struct SafeHotspotFileInfo
{
    SafeHotspotFileInfo(const int64_t duration_,
                        const float percentage_,
                        char* fileName_): duration(duration_), percentage(percentage_)
    {

        if(fileName_!=NULL)
            fileName=std::string(fileName_);
        else
            fileName=std::string("");
    }

    SafeHotspotFileInfo(Hotspot_file_info * h):duration(h->duration),percentage(h->percentage)
    {


        if(h->file!=NULL)
            fileName=std::string(h->file);
        else
            fileName=std::string("");
    }

    SafeHotspotFileInfo(const SafeHotspotFileInfo& h):duration(h.duration), percentage(h.percentage),
        fileName(std::string(h.fileName)) {

    }

    int64_t duration;
    float percentage;
    std::string fileName;
};

#endif // SAFE_HOTSPOT_FILE_INFO_HPP
