#include "profiler_backend_user.hpp"
#include <iostream>
#include <sstream>
#include <QDebug>

#include "timeline.hpp"
#include "hotspotswidget.hpp"
#include "treemodel.hpp"
#include "treeitem.hpp"

std::vector<int> Profiler_backend_user::getIds() {
    return traceId;
}

void Profiler_backend_user::signal_group_setup(std::string name, bool enabled)
{
    for (js::config *group: this->json->get("**/signal_groups")->get_elems())
    {
        if (group->get("name")->get_str() == name)
        {
            for (js::config *signal: group->get("signals")->get_elems())
            {
                this->backend->trace_setup(signal->get_str(), enabled);
            }
        }
    }
}

void Profiler_backend_user::open(std::string gvsoc_config, js::config *json,
                                 QMap<QString,QCheckBox*> groupBoxesMap)
{
    std::cout << "Open " << gvsoc_config << std::endl;
    backend = new Profiler_backend();

    this->json = json;

    backend->open(this, gvsoc_config, json);

    if (doItOnce) {
        // initialize upon the json values
        for (js::config *group: json->get("**/signal_groups")->get_elems())
        {
            if (group->get("enabled")->get_bool())
            {
                this->signal_group_setup(group->get("name")->get_str(), true);
            }
        }
        doItOnce = false;
    }
    else {
        // initialize upon the gvsoc Settings of the front end
        QMapIterator<QString, QCheckBox*> i(groupBoxesMap);
        while (i.hasNext()) {
            i.next();
            if(i.value()->isChecked())
                this->signal_group_setup(i.key().toStdString(), true);
            else
                this->signal_group_setup(i.key().toStdString(), false);

        }
    }



}

void Profiler_backend_user::start(QCheckBox* fcButton,
                                  const std::array<QCheckBox*, 9> &pcButtons)
{


    if (fcButton->isChecked())
        backend->hotspot_enable("/chip/soc/fc/pc");
    else
        backend->hotspot_disable("/chip/soc/fc/pc");

    for (int i=0; i<9;i++) {
        std::string pe_str = "/chip/cluster/pe" + std::to_string(i);
        if (pcButtons[i]->isChecked())
            backend->hotspot_enable(pe_str + "/pc");
        else
            backend->hotspot_disable(pe_str + "/pc");
    }

    backend->hotspot_bynary_traces({"/chip/soc/fc/binaries"});

    backend->start();
}

void Profiler_backend_user::close()
{
    std::cout << "Close Backend " <<  std::endl;
    backend->close();
    // Clear the signals lists
    traceMap.clear();
    sigWidthMap.clear();
    idMap.clear();
    traceId.clear();
    enoughTraces=false;
    std::cout << " End Close Backend " << std::endl;

}

void Profiler_backend_user::stop()
{
    std::cout << "Close Backend " << std::endl;
    backend->stop();

    std::cout << " End Close Backend " << std::endl;

}

void Profiler_backend_user::new_trace(Profiler_backend_trace &trace)
{
    //std::cout << "new_trace" << std::endl;
    //std::cout << "--- NEW TRACE " << trace.path << " " << trace.id << " " << trace.width << std::endl;
    traceMap.insert(QString::fromStdString(trace.path), trace.id); // stores a trace Map
    sigWidthMap.insert(QString::fromStdString(trace.path), trace.width); // stores a trace Map
    idMap.insert(trace.id,QString::fromStdString(trace.path));



    if (model==NULL){
        std::cout << "Model " << model << std::endl;
        return;
    }

    // Now, check if this trace is part of a dynamic group of signals
    // For this, we check if the path of the signal contains the root of
    // a dynamic group
    QString signalPath = QString::fromStdString(trace.path);
    TreeItem* parentItem= NULL;
    QString rootPath;
    QMap<QString, TreeItem*>::iterator i;
    for (i = model->dynamicGroups.begin(); i != model->dynamicGroups.end(); ++i){
        //std::cout << "Checking group " <<  i.key().toStdString() << std::endl;
        if (signalPath.contains(i.key())) {
            parentItem=i.value();
            rootPath=i.key();
            //std::cout << "signal " << signalPath.toStdString() << " in group" <<  i.key().toStdString() << std::endl;
            break;
        }
    }


    if (parentItem!=NULL){
        // We foud a group this signal fits in
        // We need to include it in the hierarchy of the TreeModel under the parent Item
        // but only if it doesn't exist already in the hierarchy
        QString pathResidue = signalPath.remove(rootPath);
        QStringList list = pathResidue.split(QLatin1Char('/'));
        QStringList::iterator iterator;
        QStringList::iterator iteratorLast;
        for (iterator = list.begin(); iterator != list.end();++iterator)
            iteratorLast=iterator;
        for (iterator = list.begin(); iterator != list.end();++iterator) {
            if (*iterator!="") {
                if (iterator != iteratorLast) {
                    //std::cout << "node " << (*iterator).toStdString() << " Created" << std::endl;
                    parentItem=model->insertChild(parentItem,*iterator,QString("null"));
                }
                else {
                    //std::cout << "End node " << (*iterator).toStdString() << " Created" << std::endl;
                    model->insertChild(parentItem,*iterator,QString::fromStdString(trace.path));
                }

            }

        }
        signalsTreeView->setModel(model);
        signalsTreeView->update();
        signalsTreeView->show();
    }
    //std::cout << " End new_trace" << std::endl;
}


void Profiler_backend_user::update_view(Profiler_view_results *results)
{
    qDebug() << "update_View";
    // Here, we need to send a signal to the Timeline instead of making a direct call to the
    // a timeline procedure.
    // trace_slots and other data need to be stored within the Profiler_backend_user object and
    // a get procedure needs to be called once the signal is received in the timeline window.
    // signal will be called dataReady .
    //std::cout << "[" << start_timestamp << "," <<  end_timestamp << "]" << std::endl;
    //std::cout << "#################### traces received " << trace_slots.size() << std::endl;
    ((TLGView*)results->tag)->updateMe(results);
    qDebug() << " End update_View";
}

void Profiler_backend_user::update_hotspots(void *tag, Hospot_results *result,
                                            int64_t start_timestamp, int64_t end_timestamp, int64_t max_timestamp)
{
    ((HotspotsWidget*)tag)->updateHotspotsTable(result,start_timestamp,end_timestamp,max_timestamp);
    return;


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

void Profiler_backend_user::setTreeModel(TreeModel* _model){
    model=_model;
}

void Profiler_backend_user::setSignalsTreeView(SignalTree* _signalsTreeView){
    signalsTreeView = _signalsTreeView;
}
