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

#include <iostream>
#include <string>
#include <cstdlib>
#include <QPushButton>
#include <QScrollBar>
#include <QApplication>
#include <QStyle>
#include <QSpacerItem>
#include <QTimer>
#include <QIcon>
#include <QToolBar>
#include <QGraphicsSimpleTextItem>
#include <QAbstractSlider>
#include <QSizePolicy>
#include <QMessageBox>
#include <QColor>
#include <QPalette>
#include <QFile>
#include <QHeaderView>
#include <unistd.h>
#include <QtDebug>
#include <assert.h>

#include "math.h"

#include "timeline.hpp"

#include "profiler_backend_user.hpp"
#include "tlgview.hpp"
#include "hlegendwidget.hpp"
//#include "stattable.pp"
#include <QAbstractSlider>

using std::cout; using std::getenv;
using std::string; using std::cerr;
using std::endl;

class TLGView;

const double  TLGView::rectRelativeSize = 0.8;
const double  TLGView::rectRelativeOffset = (1 - TLGView::rectRelativeSize) / 2;
const int     TLGView::ticStep = 200; // pixels
const int     TLGView::legendWidth = 100;
const int     TLGView::legendTextSpace = 3;
const int     TLGView::ticSize = 8;
const int     TLGView::functionNameMinSize = 50;
const int     TLGView::autoRedrawPeriod = 2000; // ms
const int     TLGView::scrollBarMaxRange = 100000;
const double  TLGView::scrollCoeff = 2. / 10000;

void Timeline::setBackend(Profiler_backend_user* backendUser){
    backend = backendUser;
}

void Timeline::gotoGivenTimestamp(){
    std::string s = timestamp->text().toStdString();
    char* end;
    int64_t t = strtoll(s.c_str(), &end, 10);
    // ??? we get the value in ns. needs to be converted in ps.
    t=t*1000;
    /* invalid string -> does nothing */
    if (end == s.c_str()) return;
    gview->gotoTimestamp(t);
}

void Timeline::completeToolBar(){

    const char *tmp = getenv("GAP_SDK_HOME");
    string env_var(tmp ? tmp : "");
    if (env_var.empty()) {

        QMessageBox::information(this, "[ERROR] Environmental variable not found " , "GAP_SDK_HOME");
        exit(EXIT_FAILURE);
    }

    QString zoomInFile = QString::fromStdString(env_var) +  "/tools/profiler_v2/gui/images/Apps-Zoom-In-icon.png";
    QString zoomOutFile = QString::fromStdString(env_var) + "/tools/profiler_v2/gui/images/Apps-Zoom-Out-icon.png";
    QString zoomFitFile = QString::fromStdString(env_var) + "/tools/profiler_v2/gui/images/Apps-Zoom-Fit-icon.png";

    const QIcon zoomInIcon = QIcon::fromTheme("zoom-in", QIcon(zoomInFile));
    const QIcon zoomOutIcon = QIcon::fromTheme("zoom-out", QIcon(zoomOutFile));
    const QIcon viewAllIcon = QIcon::fromTheme("zoom-fit", QIcon(zoomFitFile));

    QAction *zoomInAct = new QAction(zoomInIcon, tr("Zoom In"), this);
    QAction *zoomOutAct = new QAction(zoomOutIcon, tr("Zoom Out"), this);
    QAction *viewAllAct = new QAction(viewAllIcon, tr("Zoom Fit"), this);

    connect(zoomInAct, &QAction::triggered, gview, &TLGView::verticalLineZoomIn);
    connect(zoomOutAct, &QAction::triggered, gview, &TLGView::verticalLineZoomOut);
    connect(viewAllAct, &QAction::triggered, gview, &TLGView::viewAll);

    timestamp = new QLineEdit("0 µs");
    timeInterval = new QLineEdit("0");
    timestamp->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    timeInterval->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(timestamp, &QLineEdit::returnPressed, this, &Timeline::gotoGivenTimestamp);

    toolBar->addAction(zoomInAct);
    toolBar->addAction(zoomOutAct);
    toolBar->addAction(viewAllAct);

    //toolBar->addSeparator();
    timeLabel = new QLabel("Timestamp: ");
    toolBar->addWidget(timeLabel);
    toolBar->addWidget(timestamp);
    //toolBar->addSeparator();
    intervalLabel = new QLabel("Time Interval: ");
    toolBar->addWidget(intervalLabel);
    toolBar->addWidget(timeInterval);

    /*toBeRemoveAfterExec.append(
    toolBar->addWidget(new QLabel("   your code is being profiled...   ")));
  toBeRemoveAfterExec.append(toolBar->addWidget(progressBar));
*/
    update();
}


void Timeline::foo(int x){
    std::cout << "foo called " << x << std::endl;
}


void Timeline::setGviewVScrollValue() {

    qDebug() << "[-] setGViewVScrollValue () " ;
    int value = signalsTreeView->verticalScrollBar()->value();
    signalsTreeView->verticalScrollBar()->setValue(signalsTreeView->verticalScrollBar()->minimum());
    signalsTreeView->verticalScrollBar()->setValue(value);
    //setGviewVScrollValue(signalsTreeView->verticalScrollBar()->value());
    //scrollArea->syncWithTreeView();
    gview->setReloadData(true);
    gview->setHighlightRow(-1);
    gview->update();
    gview->show();
    //scrollArea->update();
    //scrollArea->show();
}

void Timeline::setGviewVScrollValue(int value1) {
    // syncs the Scrollarea vertical scroll bar value according to the
    // value of the signalsTreeView scrollbar
    qDebug() << "[-] setGViewVScrollValue (int value1) "  << value1;
    //scrollArea->syncWithTreeView();
    gview->setReloadData(true);
    gview->setHighlightRow(-1);
    gview->update();
    gview->show();
    //scrollArea->update();
    //scrollArea->show();
    return;
    // First block the scrollArea vertical bar signals
    scrollArea->verticalScrollBar()->blockSignals(true);

    // First calculate the percentage of move on the signalsTreeView
    double  min1 = (double) signalsTreeView->verticalScrollBar()->minimum();
    double  max1 = (double) signalsTreeView->verticalScrollBar()->maximum();

    if (max1-min1<=0)
        return;
    // Then get min and max of the   Vartical bar
    double min2 = (double) scrollArea->verticalScrollBar()->minimum();
    double max2 = (double) scrollArea->verticalScrollBar()->maximum();
    double factor = (max2 - min2) / (max1 - min1) ;
    double value3= round(factor * value1);
    int value2 = value3;
    // set the scrollArea scroll bar value
    scrollArea->verticalScrollBar()->setValue(value2);

    // update widget position
    QPoint topLeft = scrollArea->viewport()->rect().topLeft();
    scrollArea->widget()->move(0, topLeft.y() - value2);

    // Finally unblock scrollarea signals
    scrollArea->verticalScrollBar()->blockSignals(false);

    gview->update();
    gview->show();
    scrollArea->update();
    scrollArea->show();

}

/*

void Timeline::updateSignalsStatView( SignalTree* signalsTreeView,
                                      uint64_t t0,
                                      uint64_t t1) {
  // Display signals statistics between x1 and x2
  //displaySignalsStats(auto m_line.p1().x(),m_line.p2().x());

  if ((gview!= NULL) && (statModel != NULL) && (statTableView != NULL)){
    delete statModel;
    statModel= new StatModel(this);
    statModel->updateTreeParameters(signalsTreeView);
    statModel->populateData(t0, t1);
    statTableView->setModel(statModel);
    statTableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    //statTableView->setColumnWidth(0,statTableView->width());
    statTableView->resizeColumnToContents(2);
    statTableView->horizontalHeader()->setStretchLastSection(true);
    //statTableView->setVisible(false);
    mw->update();
    mw->repaint();
    mw->show();
    emit timeUpValueChanged();

  }
}

void Timeline::updateSignalsStatView(SignalTree* signalsTreeView) {
  //std::cout << "[-] Timeline::updateSignalsStatView()  " << std::endl;
  // Display signals statistics between previously set range [t0Select, t1Select]
  if ((gview!= NULL) && (statModel != NULL) && (statTableView != NULL)) {
    delete statModel;
    statModel= new StatModel(this);
    statModel->updateTreeParameters(signalsTreeView);
    statModel->populateData(gview->t0Select,gview->t1Select);
    statTableView->setModel(statModel);
    statTableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    //statTableView->setColumnWidth(0,statTableView->width());
    statTableView->resizeColumnToContents(2);
    statTableView->horizontalHeader()->setStretchLastSection(true);
    //statTableView->setVisible(false);
    mw->update();
    mw->repaint();
    mw->show();
    emit timeUpValueChanged();
  }

}

void Timeline::changeSignalsStatVisibility(bool visible){
   // sets the signals Table View visible or not
  QLayoutItem *item = 0;
  QWidget *widget = 0;
   for(int row = 0; row < 2; ++row)
    {
      item = tlLayout->itemAtPosition(row,2);
      widget=item?item->widget():0;
      if(widget)
        widget->setVisible(visible);
    }
 }
*/
void Timeline::createTlLabels(){
    // create labels
    treeLabel = new QLabel();
    treeLabel->setText("Time (us)");
    treeLabel->setFixedHeight(signalHeight);
    statLabel = new QLabel();
    statLabel->setText("Signal Time Up");
    statLabel->setFixedHeight(signalHeight);
}

void Timeline::defineHotSpot(const QModelIndex &index) {
    //At this point, user has double-clicked in the signals Tree View on a signal
    // and we get the QModelIndex of the signal
    // Get the sibling of this index in column 1 and then the value containing the path of
    // the signal


    QModelIndex sibling = index.sibling(index.row(),1);
    // Now, get the path of the signal
    QString signalPath = sibling.data().toString();
    //QMessageBox::information(this,"Info:", signalPath );

    // once we have the path of the signal, we need to check if it's a pc
    // If not, we just do nothing
    std::cout << "Signal Double Clicked: " << signalPath.toStdString() << std::endl;
}

void Timeline::updateHotspotsWidget(uint64_t t0Select, uint64_t t1Select){
    // context: user has selected a timeRange and we need to
    // update the hotspots Widget to reflect stats only regarding this time range

    htw->getHotspots(t0Select,t1Select);

}


Timeline::Timeline( QMainWindow* mw,
                    SignalTree* signalsTreeView,
                    HotspotsWidget* htw,
                    QToolBar* toolBar,
                    StallChart* stallchart,
                    js::config *json,
                    Profiler_backend_user* backend,
                    std::string fifo_path,
                    QMap<QString,QCheckBox*> groupBoxesMap):
    mw(mw),
    signalsTreeView(signalsTreeView),
    htw(htw),
    toolBar(toolBar),
    stallchart(stallchart),
    backend(backend),
    json(json)
{
    std::cout << "Timeline Creator" << std::endl;
    // Assertions
    assert(mw != NULL);
    assert(toolBar != NULL);

    // construct hierarchy

    //Define Timeline layout as a Grid Layout -- filled later on
    tlLayout = new QGridLayout();
    this->setLayout(tlLayout);

    // Create SignalsTree View
    //backend->open(fifo_path, this->json,groupBoxesMap);
    //signalsTreeView = new SignalTree(json);
    //std::cout << "timeline::Model " << signalsTreeView->model << std::endl;
    //backend->setTreeModel(signalsTreeView->model);

    // create TL layout labels
    createTlLabels();
    // Create the Model for the signals statistics
    //statModel= new StatModel(this);
    // Create the TableView for signals statistics
    //statTableView = new StatTable(signalsTreeView);


    // create a widget with vertical layout for containing the scrollarea and
    // the toolbar
    verViewW = new QWidget();
    verViewLayout = new QVBoxLayout();
    verViewW->setLayout(verViewLayout);
    qDebug() << "[-] Fill up  Timeline Grid Layout";
    tlLayout->setHorizontalSpacing(0);
    tlLayout->setVerticalSpacing(0);
    tlLayout->addWidget(treeLabel,0,0);
    tlLayout->addWidget(signalsTreeView,1,0);

    //tlLayout->addWidget(statTableView,1,2, Qt::AlignRight);
    //tlLayout->addWidget(statLabel,0,2, Qt::AlignRight);
    tlLayout->setColumnStretch(2,0);
    tlLayout->setColumnStretch(0,0);
    tlLayout->setColumnStretch(1,800);
    tlLayout->setColumnMinimumWidth(2,50);


    QObject::connect(signalsTreeView, SIGNAL(doubleClicked(QModelIndex)),
                     this, SLOT(defineHotSpot(QModelIndex)),Qt::QueuedConnection);
    QObject::connect(this, SIGNAL(timeUpValueChanged()),
                     this, SLOT(refreshTimelineWidgets()),
                     Qt::QueuedConnection);



    // Update timeline window sub-windows
    qDebug() << "[-] Timeline window  created";
    signalsTreeView->update();
    this->update();
    // Show timeline window sub-windows
    qDebug() << "[-] Timeline window  updates done";
    signalsTreeView->show();
    this->show();

    qDebug() << "[-] Timeline window  displayed";

}

void Timeline::finishTimeline()
{

    // Create TL horizontal scroll bar
    qDebug() << "[-] Create Horizontal ScrollBar";
    scrollbar = new QScrollBar(Qt::Horizontal);
    scrollbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    scrollbar->setTracking(false);

    // For an update --
    if (gview == NULL) {
        gview = new TLGView(this,
                            scrollbar,
                            legendLayout,
                            signalsTreeView,
                            backend,
                            json);
        completeToolBar();

        //create scroll area containing the gview and vertically piloted by the signalsTreeView
        //vertical scrollbar.
        scrollArea = new AdjustingScrollArea(signalsTreeView,gview,this);

        // Create horizontal Legend for the gview and its connections
        // to the gview and scrollbar events

        hLegendW = new HLegendWidget(gview);
        hLegendW->setStyleSheet("border: 2px solid red");
        connect(gview, SIGNAL(zoomOccured()), hLegendW, SLOT(update()));
        connect(scrollbar,  SIGNAL(valueChanged(int)), gview, SLOT(handleSBarChange(int)) );
        connect(scrollbar, SIGNAL(actionTriggered(int)),gview, SLOT(handleSliderAction(int)) );
        QObject::connect(scrollbar, &QScrollBar::sliderMoved, gview, &TLGView::sliderMoved);

        qDebug() << "[-] Create scrollArea + scrollBar container widget";
        verViewLayout->addWidget(scrollArea);
        verViewLayout->addWidget(scrollbar);
        verViewLayout->setMargin(0);
        verViewLayout->setSpacing(0);
        verViewLayout->setAlignment(scrollbar,Qt::AlignBottom);
        verViewW->adjustSize();

        // for an update
        tlLayout->addWidget(verViewW,1,1);
        tlLayout->addWidget(hLegendW,0,1);

        // Connect different signals to corresponding slots
        // using the connection type Qt::QueuedConnection ensures that the slot code gets
        // executed after whatever operation emits the signal is completed and control
        // is given back to QT's event loop
        QObject::connect(signalsTreeView, SIGNAL(expanded(QModelIndex)),
                         gview, SLOT(handleSignalNodeExpanded()),Qt::QueuedConnection);
        QObject::connect(signalsTreeView, SIGNAL(collapsed(QModelIndex)),
                         gview, SLOT(handleSignalNodeCollapsed()),Qt::QueuedConnection);

        QObject::connect(signalsTreeView->verticalScrollBar(),SIGNAL(rangeChanged(int,int)),
                         this, SLOT(setGviewVScrollValue()),Qt::QueuedConnection);
        QObject::connect(signalsTreeView->verticalScrollBar(), SIGNAL(valueChanged(int)),
                         this, SLOT(setGviewVScrollValue(int)),Qt::QueuedConnection);
        QObject::connect(signalsTreeView, SIGNAL(clicked(QModelIndex)),
                         gview, SLOT(highlightSignal(QModelIndex)),Qt::QueuedConnection);

        QObject::connect(this, SIGNAL(timeUpValueChanged()),
                         this, SLOT(refreshTimelineWidgets()),
                         Qt::QueuedConnection);

    }

    // Update timeline window sub-windows
    qDebug() << "[-] Timeline window  created";
    scrollbar->update();
    hLegendW->update();
    signalsTreeView->update();
    gview->update();
    gview->raise();
    verViewW->update();
    scrollArea->update();
    this->update();

    // Show timeline window sub-windows
    qDebug() << "[-] Timeline window  updates done";
    scrollbar->show();
    hLegendW->show();
    gview->show();
    verViewW->show();
    scrollArea->show();
    signalsTreeView->show();
    this->show();

    qDebug() << "[-] Timeline window  updated";
}



Timeline::~Timeline(){

    if(timestamp)
        delete timestamp;
    if(timeInterval)
        delete timeInterval;
    if(timeLabel)
     delete timeLabel;
    if(intervalLabel)
        delete intervalLabel;
    if(treeLabel)
        delete treeLabel;
    if (statLabel)
        delete statLabel;
    if (signalsTreeView)
        delete signalsTreeView;
    if (scrollbar)
        delete scrollbar;
    if (verViewW)
        delete verViewW;

    clearCurrentExecToolBar();
}


void Timeline::resizeEvent(QResizeEvent *event){

}

void Timeline::clearCurrentExecToolBar(){
    for (auto c: toBeRemoveAfterExec){
        toolBar->removeAction(c);
    }
    toBeRemoveAfterExec.clear();
}

void Timeline::refreshTimelineWidgets() {
    //statTableView->repaint();
    gview->repaint();
    scrollArea->repaint();
    this->repaint();
    mw->repaint();
    //statTableView->show();
    gview->show();
    scrollArea->show();
    this->show();
    mw->show();
}
