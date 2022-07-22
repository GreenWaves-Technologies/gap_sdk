/*
 * Copyright (C) 2020  GreenWaves Technologies, SAS
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef TIMELINE_HPP
#define TIMELINE_HPP

#include <string>
#include <unordered_map>

#include <QWidget>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QPaintEvent>
#include <QMdiSubWindow>
#include <QPushButton>
#include <QButtonGroup>
#include <QRadioButton>
#include <QScrollArea>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QResizeEvent>
#include <QLabel>
#include <QGraphicsItem>
#include <QValueAxis>
#include <QToolBar>
#include <QMainWindow>
#include <QLineEdit>
#include <QMutex>
#include <QTreeView>
#include <QPoint>
#include <QDockWidget>
#include <QScrollArea>
#include <QMessageBox>
#include <QFile>

#include <QTableView>

#include <thread>
#include <mutex>
#include <QMutex>
#include <unistd.h>

#include "functiondetails.hpp"
#include "stallchart.hpp"
#include "prolabel.hpp"

//#include "tldata.hpp"
//#include "statmodel.hpp"
#include "signaltree.hpp"

#include "adjustingscrollarea.hpp"
#include "profiler_backend_user.hpp"
#include "hotspotswidget.hpp"
#include "hlegendwidget.hpp"

class TLGView;
class StatModel;
class AdjustingScrollArea;

/**
  @brief class that regroups controls elements and timeline view
SignalTree
  class that regroups
    - the view of the timeline (TLGView)
    - the legend of the view
    - the toolBar to control the view
    - the scrollbar
    - the signalTree to add traces to the view
    */
class Timeline : public QWidget
{
    Q_OBJECT
public:
    Timeline( QMainWindow* mw,
              SignalTree* signalsTreeView,
              HotspotsWidget* htw,
              QToolBar* toolBar,
              StallChart* stallchart,
              js::config *json,
              Profiler_backend_user* backend,
              std::string fifo_path,
              QMap<QString,QCheckBox*> groupBoxesMap);
    ~Timeline();
    void finishTimeline();
    void setBackend(Profiler_backend_user* backendUser);
    Profiler_backend_user* getBackend() {return backend;}
    void setTimestamp(const QString& s) {timestamp->setText(s); }
    void setTimeInterval(const QString& s) {timeInterval->setText(s); }
    void resizeEvent(QResizeEvent *event) override;
    void setModeButtonText(QString s);
    void foo(int x);
    void clearCurrentExecToolBar();

    TLGView* getTLGView(){return gview;};

    void updateSignalsStatView( SignalTree* signalsTreeView,
                                uint64_t t0,
                                uint64_t t1);
    void updateSignalsStatView( SignalTree* signalsTreeView);
    void changeSignalsStatVisibility(bool visible);
    void updateFlags(bool gvsocRun) {gvsocRunFlag = gvsocRun;};
    void updateHotspotsWidget(uint64_t t0Select, uint64_t t1Select);
    // Flags
    // gvsocRunFlag is set to True when gvsoc has started running
    // gvsocRunFlag is set to False when a new gvsoc process is opened
    bool gvsocRunFlag = false;
    // Timeline Scroll Area
    //QScrollArea* scrollArea;
    AdjustingScrollArea* scrollArea;
    HLegendWidget* hLegendW =NULL;
private:
    void makeLegend();
    void completeToolBar();
    void gotoGivenTimestamp();
    void createSignalsTree(QString signalsTreeFileName);
    void createTlLabels();

    js::config *json=NULL;
    Profiler_backend_user* backend=NULL;
    QMainWindow* mw=NULL;
    HotspotsWidget* htw=NULL;
    QWidget *viewW=NULL; // widget for the signalsTreeView + verViewV
    QVBoxLayout* mainLayout=NULL;
    QToolBar* toolBar=NULL;
    QVBoxLayout* legendLayout=NULL;
    QHBoxLayout* viewLayout=NULL;
    QGridLayout* tlLayout=NULL;
    SignalTree* signalsTreeView=NULL;

    QWidget* bottomW=NULL; //bottom timeline view
    QWidget* signalsW=NULL; // headerW + signalsTreeView
    QVBoxLayout* bottomLayout=NULL;

    // widget for the gview + scrollbar
    QWidget* verViewW=NULL;
    QVBoxLayout* verViewLayout=NULL;
    QLabel *treeLabel=NULL;
    QLabel *statLabel=NULL;
    QVBoxLayout* verTreeLayout=NULL;



    // Timeline Graphic View
    TLGView *gview = NULL;

    // Statistics Table
    StatModel *statModel = NULL;
    QTableView *statTableView = NULL;
    // Dock windows we need to communicate with
    QDockWidget* functionsDock=NULL;
    StallChart* stallchart = NULL;

    // For the tool Bar
    QPushButton* zoomIn=NULL;
    QPushButton* zoomOut=NULL;
    QPushButton* viewAll=NULL;
    QLineEdit* timestamp= NULL;
    QLineEdit* timeInterval=NULL;

    QAction* zoomInAct;
    QAction* zoomOutAct;
    QAction* viewAllAct;

    QLabel* timeLabel= NULL;
    QLabel* intervalLabel=NULL;

    QScrollBar* scrollbar=NULL;

    QList<QAction*> toBeRemoveAfterExec;

signals:
    void timeUpValueChanged();

public  slots:

    //void setGviewVScrollValue(int value1);
    void setGviewVScrollValue();
    void setGviewVScrollValue(int value1);
    void refreshTimelineWidgets();

private slots:
    void defineHotSpot(const QModelIndex &index);

};

#endif // TIMELINE_HPP
