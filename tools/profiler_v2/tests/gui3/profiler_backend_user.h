#ifndef PROFILER_BACKEND_USER_H
#define PROFILER_BACKEND_USER_H

#include <QMap>

#include "profiler_backend.hpp"

class Profiler_backend_user : public Profiler_backend_user_interface
{
public:
    //Profiler_backend_user();
    void start(std::string fifo_path);
        void update_view(   void* widget,std::vector<std::vector<Profiler_backend_slot_info>> &trace_slots,
                            int64_t start_timestamp,
                            int64_t end_timestamp,
                            int nb_slots);
        void new_trace(Profiler_backend_trace &trace);
        Profiler_backend* backend;
        QMap<int, QString> traceMap;
        std::vector<int> traceId;
        std::vector<int> getIds();
        bool enoughTraces = false;
        bool canGetView() {return enoughTraces;};
};

#endif // PROFILER_BACKEND_USER_H
