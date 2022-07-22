#ifndef PROFILER_BACKEND_USER_H
#define PROFILER_BACKEND_USER_H

#include <QMap>

#include "profiler_backend.hpp"
#include <QCheckBox>
#include "json.hpp"
#include "treemodel.hpp"
#include "signaltree.hpp"

class Profiler_backend_user : public Profiler_backend_user_interface
{
public:
    //Profiler_backend_user();
    void open(std::string fifo_path, js::config *json,
              QMap<QString,QCheckBox*> groupBoxesMap);
    void start(QCheckBox *fcButton, const std::array<QCheckBox*, 9> &pcButtons);
    void close();
    void stop();
    void getHotspotsData();
    Profiler_backend* getBackend() {return backend;}
    void update_view(Profiler_view_results *results);
    bool canGetView() {return enoughTraces;}
    void update_hotspots(void *tag, Hospot_results *result,
                         int64_t start_timestamp, int64_t end_timestamp, int64_t max_timestamp);

    void new_trace(Profiler_backend_trace &trace);

    void signal_group_setup(std::string name, bool enabled);
    Profiler_backend* backend;
    QMap<QString,int> traceMap; // stores the Ids of all the signals
    QMap<QString,int> sigWidthMap; // stores the width in bits of all the signals
    QMap<int,QString> idMap; // stores the path signals for all signal Ids
    std::vector<int> traceId;
    std::vector<int> getIds();
    bool enoughTraces = false;
    void setTreeModel(TreeModel* _model);
    void setSignalsTreeView(SignalTree* _signalsTreeView);

private:
    js::config *json;
    bool doItOnce = true;
    TreeModel* model;
    SignalTree* signalsTreeView;
};

#endif // PROFILER_BACKEND_USER_H
