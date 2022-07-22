#include "profiler_backend.hpp"
#include <unistd.h>

class Profiler_backend_user: public Profiler_backend_user_interface
{
public:
    void start(std::string gvsoc_config_path);
    void update_view(void *tag, std::vector<std::vector<Profiler_backend_slot_info>> &trace_slots, int64_t start_timestamp, int64_t end_timestamp, int64_t max_timestamp, int nb_slots);
    void new_trace(Profiler_backend_trace &trace);
    void update_hotspots(void *tag, Hospot_results *result,
        int64_t start_timestamp, int64_t end_timestamp, int64_t max_timestamp);

private:
    int fc_active_id;
};


void Profiler_backend_user::start(std::string gvsoc_config_path)
{
    Profiler_backend *backend = new Profiler_backend();

    backend->open(this, gvsoc_config_path);

    backend->hotspot_enable("/chip/soc/fc/pc");
    backend->hotspot_bynary_traces({"/chip/soc/fc/binaries"});
    backend->start(4000000000);

    sleep(1);

    backend->get_hotspots(NULL, -1, 1000000000, 3000000000);

    // std::vector<int> traces = {this->fc_active_id};
    // backend->get_view(this, traces, -1, -1, 10);

    sleep(1);

    backend->close();
}


void Profiler_backend_user::new_trace(Profiler_backend_trace &trace)
{
    //printf("New trace id %d path %s type %d width %d\n", trace.id, trace.path.c_str(), trace.type, trace.width);
    if (trace.path == "/chip/soc/fc/is_active")
    {
        this->fc_active_id = trace.id;
        printf("Found ID %d\n",  this->fc_active_id);
    }
}


void Profiler_backend_user::update_view(void *tag, std::vector<std::vector<Profiler_backend_slot_info>> &trace_slots, int64_t start_timestamp, int64_t end_timestamp, int64_t max_timestamp, int nb_slots)
{
    printf("Updated VIEW size %ld\n", trace_slots[0].size());

    for (int i=0; i<trace_slots[0].size(); i++)
    {
        printf("%f\n", trace_slots[0][i].value);
    }

}

void Profiler_backend_user::update_hotspots(void *tag, Hospot_results *result,
    int64_t start_timestamp, int64_t end_timestamp, int64_t max_timestamp)
{
    printf("Updated hotspots\n");

    std::list<Hotspot_info *> hotspots;
    result->get_hotspots(hotspots);

    int64_t time = 0;
    int count = 0;
    for (auto x: hotspots)
    {
        time += x->duration;
        count++;
        if (count < 20)
        {
            printf("0x%lx: %ld %s\n", x->pc, x->duration, x->function);
        }
    }

    printf("TIME %ld\n", time);

    std::vector<const char *> files;
    result->get_files(files);
    for (auto file: files)
    {
        std::vector<const char *> functions;
        printf("FILE %s\n", file);
        result->get_functions(functions, file);
        for (auto function: functions)
        {
            printf("    FUNCTION %s %p\n", function, function);
            std::list<Hotspot_info *> hotspots;
            result->get_hotspots(hotspots, file, function);

            int64_t time = 0;
            int count = 0;
            for (auto x: hotspots)
            {
                time += x->duration;
                count++;
                if (count < 5)
                {
                    printf("            0x%lx: duration=%ld function=%s count=%ld line=%d\n", x->pc, x->duration, x->function, x->count, x->line);
                }
            }
        }
    }

}

int main(int argc, char *argv[])
{
    Profiler_backend_user *backend = new Profiler_backend_user();

    backend->start(argv[1]);

    return 0;
}