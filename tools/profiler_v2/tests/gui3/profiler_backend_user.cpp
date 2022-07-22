#include "profiler_backend_user.h"
#include <iostream>

#include "timeline.h"

std::vector<int> Profiler_backend_user::getIds() {
    return traceId;
}

void Profiler_backend_user::start(std::string gvsoc_config)
{
    std::cout << "start " << gvsoc_config << std::endl;
    backend = new Profiler_backend();

    backend->open(this, gvsoc_config);

}


void Profiler_backend_user::new_trace(Profiler_backend_trace &trace)
{
    //std::cout << "new_trace" << std::endl;

    traceMap.insert(trace.id, QString::fromStdString(trace.path)); // stores a trace Map

    if (trace.path=="/chip/soc/fc/is_active"){
        traceId.push_back(trace.id); // stores the ids in a vector
        enoughTraces = true;
        std::cout << "new_trace: enough" << std::endl;
    }

}


void Profiler_backend_user::update_view(void* widget, std::vector<std::vector<Profiler_backend_slot_info>> &trace_slots,
                                        int64_t start_timestamp,
                                        int64_t end_timestamp,
                                        int nb_slots)
{
    std::cout << "update_View" << std::endl;
    // Here, we need to send a signal to the Timeline instead of making a direct call to the
    // a timeline procedure.
    // trace_slots and other data need to be stored within the Profiler_backend_user object and
    // a get procedure needs to be called once the signal is received in the timeline window.
    // signal will be called dataReady .
    ((TimeLine*)widget)->updateMe(trace_slots, start_timestamp, end_timestamp, nb_slots);
}
