/*
 * Copyright (C) 2019 GreenWaves Technologies
 * All rights reserved.
 *
 */

#include <iostream>
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

#include "math.h"

#include "timeline.hpp"

#include "backend_interface.hpp"

const double  TLGView::rectRelativeSize = 0.8;
const double  TLGView::rectRelativeOffset = (1 - TLGView::rectRelativeSize) / 2;
const int     TLGView::ticStep = 200; // pixels
const int     TLGView::legendWidth = 100;
const int     TLGView::legendTextSpace = 3;
const int     TLGView::ticSize = 8;
const int     TLGView::functionNameMinSize = 50;
const int     TLGView::autoRedrawPeriod = 2000; // ms
const int     TLGView::scrollBarMaxRange = 100000;
const double  TLGView::scrollCoeff = 2. / 10'000;


bool AdjustingScrollArea::eventFilter(QObject * obj, QEvent * ev) {
  if (obj == widget() && ev->type() == QEvent::Resize) {
    //std::cout << "[-] eventFilter" << std::endl;

    // First block the scrollArea vertical bar signals
    this->verticalScrollBar()->blockSignals(true);

    // First calculate the percentage of move on the signalsTreeView
    float  min1 = (float) signalsTreeView->verticalScrollBar()->minimum(); 
    float  max1 = (float) signalsTreeView->verticalScrollBar()->maximum();
    if ((max1 - min1) == 0)
      return QScrollArea::eventFilter(obj, ev);
    float percentage = (100.0/(max1-min1)) * (float) signalsTreeView->verticalScrollBar()->value();
    // Then get min and max of the   Vartical bar
    float min2 = (float) verticalScrollBar()->minimum(); 
    float max2 = (float) verticalScrollBar()->maximum();
    float value3= round(percentage * (max2-min2)/100.0);
    int value2 = value3;
    // set the scrollArea scroll bar value
    this->verticalScrollBar()->setValue(value2);

    // update widget position
    QPoint topLeft = viewport()->rect().topLeft();
    this->widget()->move(0, topLeft.y() - value2);

    // Finally unblock scrollarea signals
    this->verticalScrollBar()->blockSignals(false);

    
    /*updateWidgetPosition();*/

  }
  //std::cout << "[-] end eventFilter" << std::endl;
  return QScrollArea::eventFilter(obj, ev);
}

void AdjustingScrollArea::updateWidgetPosition(){
  //std::cout << "[-] updateWidgetPosition" << std::endl;
  int vvalue = verticalScrollBar()->value();
  QPoint topLeft = viewport()->rect().topLeft();

  widget()->move(0, topLeft.y() - vvalue);
}

AdjustingScrollArea::AdjustingScrollArea(QTreeView* signalsView, 
                                         QWidget * parent) : QScrollArea{parent} {
  signalsTreeView = signalsView;
                                         }

void AdjustingScrollArea::setWidget(QWidget *w) {
    QScrollArea::setWidget(w);
    // It happens that QScrollArea already filters widget events,
    // but that's an implementation detail that we shouldn't rely on.
    w->installEventFilter(this);
}

void HLegendWidget::paintEvent(QPaintEvent* event) {

  //std::cout << "[-] Paint HLegendWidget 1" << std::endl;
  const QRect r = event->rect();
  int x0 = r.x();
  int x1 = r.x() + r.width();
  uint64_t t0, t1;

  gview->getTimeWindow(x0, x1, t0, t1);

  uint X = size().width();
  uint Y = gview->peHeight;

  QPainter painter(this);
  pen = QPen(Qt::black);
  pen.setWidthF(0);

  painter.setPen(pen);
  // Paint horizontal line of the legend
  painter.drawLine(0, gview->peHeight, X,Y );
  
  // calculating time interval for 10 ticsteps approximatly
  float vi= (gview->position2time(X) - gview->position2time(0))/10;
  int viRound = gview->extractPower10Round(vi);
  uint64_t xtmin= (uint64_t) (gview->position2time(0)/viRound) * viRound;
  uint64_t xtmax= (uint64_t) (gview->position2time(X)/viRound) * viRound;
  

  QString textSave="";

  for (uint64_t xt=xtmin; xt<=xtmax; xt+= viRound){
    int x = gview->time2position(xt);
    // paint vertical stick of the legend for the given timestamp
    painter.drawLine(x, Y, x, Y - gview->ticSize);
    QRect rect( x - gview->legendWidth / 2, 
                Y - gview->peHeight, 
                gview->legendWidth,
                gview->peHeight - gview->ticSize - 1);
    if (textSave != gview->time2stringBis(xt)) 
      painter.drawText(rect, Qt::AlignCenter, gview->time2stringBis(xt));
    textSave= gview->time2stringBis(xt);
  }
  //this->setMinimumHeight(1 * gview->peHeight);
  //this->setMaximumHeight(1 * gview->peHeight + 2);

  //std::cout << "[-] Paint HLegendWidget End" << std::endl; 
}

HLegendWidget::HLegendWidget(TLGView* gview): gview(gview) {
  
  //std::cout << "[-] create HLegendWidget 0" << std::endl;
  
}



TLGView::TLGView( FunctionDetails* fd, 
                  QDockWidget* functionsDock,
                  Timeline* tl, 
                  QScrollBar* sbar,
                  QVBoxLayout* legendLayout,
                  QPlainTextEdit* sourceCode,
                  QPlainTextEdit* asmCode,
                  StallChart* stallchart,
                  QScrollArea* sArea, 
                  TreeModel* signalsModel, 
                  QTreeView* signalsView
                  ):
                  functionsDock(functionsDock),
                  sourceCode(sourceCode),
                  asmCode(asmCode),
                  stallchart(stallchart),
                  sArea(sArea),
                  signalsModel(signalsModel),
                  signalsView(signalsView)
{
  this->fd = fd;
  this->tabw=tabw;
  this->tl = tl;
  this->sbar =sbar;
  this->legendLayout = legendLayout;
  currentMode = TIME_MODE;
  pen = QPen(Qt::black);
  greenPen = QPen(Qt::darkGreen);
  redPen = QPen(Qt::red);
  pen.setWidthF(0);
  greenPen.setWidthF(0);
  redPen.setWidthF(0);
  zoomFactor = 1. / 10'000'000;
  timeOffset = 0;
  maxTime = 1;
  maxScrollTime = 1;
  verticalLineTime = 0;
  timeIntervalValue = 0;
  sbar->setRange(0, TLGView::scrollBarMaxRange);
  setPageStep();

  m_nbMousePressed = false;

  connect(&timer, SIGNAL(timeout()), this, SLOT(autoReconstruct()));
  timer.start(TLGView::autoRedrawPeriod);
}

/*
void TLGView::show(){
  QWidget::show();
  QApplication::processEvents();
}
*/

QColor TLGView::getColor(std::string fname){
  if (functionColors.find(fname) != functionColors.end())
    return functionColors[fname];

  functionColors[fname] = QColor::fromHsl(currentHue, 0xff, 0xc0);
  currentHue += hueStep;
  if (currentHue >= 360){
    if( hueStep >= 2) hueStep /= 2;
    currentHue = (currentHue % 360) + hueStep;
  }
  return functionColors[fname];
}

void TLGView::drawBackground(QPainter *painter, const QRect &exposed, int nlines)
{
    QColor c1 = Qt::white;
    QColor c2 = QColor::fromRgb(0xf0, 0xf0, 0xf0);
    painter->setPen(Qt::NoPen);
    for (int i = 0; i < nlines; i++){
      painter->setBrush(i & 1 ? c1 : c2);
      painter->drawRect(exposed.x(), peHeight * i, exposed.width(), peHeight);
    }
}

void TLGView::setPageStep(){
  uint64_t t0, t1;
  getTimeWindow(t0, t1);
  sbar->setPageStep(TLGView::scrollBarMaxRange * (t1 - t0) / maxScrollTime);
  sbar->setSingleStep(std::max(1, (int) (TLGView::scrollCoeff / zoomFactor)));
}

void TLGView::setMaxScrollTime(){
  // << "[-]setMaxScrollTime " << std::endl;
  uint64_t w = timeWidth();
  // add a 30 pixel margin after last event (no margin looks weird)
  //std::cout << "maxTime " << maxTime << std::endl;
  //std::cout << "w " << w << std::endl;
  //std::cout << "zoomFactor" << zoomFactor << std::endl;

  if (maxTime > w) 
    maxScrollTime = maxTime - w + 30 / zoomFactor;
  else 
    maxScrollTime = 1;

  //std::cout << "maxScrollTime " << maxScrollTime << std::endl;
  // readjust sbar->value to be consistent with timeOffset
  sbar->setValue(timeOffset * TLGView::scrollBarMaxRange / maxScrollTime);
}

void TLGView::autoReconstruct(){
  //std::cout << "autoREconstruct  " << std::endl;
  if (tldata_has_changed()){
    ack_tld_changes();
    maxTime = get_max_time();
    setMaxScrollTime();
    setPageStep();
    update();
    //sArea->update();
  }
  if (datamanager_done()){
    tl->clearCurrentExecToolBar();
  }
}

void TLGView::getTimeWindow(uint64_t& t0, uint64_t& t1) const{
  getTimeWindow(0, size().width(), t0, t1);
}

void TLGView::getTimeWindow(int x0, int x1, uint64_t& t0, uint64_t& t1) const{
  t0 = timeOffset;
  t1 = timeOffset + (uint64_t) ((x1 - x0) / zoomFactor);
}

int TLGView::time2position(uint64_t t) const {
  if (t < timeOffset) return -1;   // not in display range
  return round((double) (t - timeOffset) * zoomFactor);
}

uint64_t TLGView::position2time(int x) const {
  return timeOffset + (uint64_t) ((double) x / zoomFactor);
}

uint64_t TLGView::width2time(int x) const {
  return (uint64_t) ((double) x / zoomFactor);
}

uint64_t TLGView::timeWidth() const{
  return (double) size().width() / zoomFactor;
}

QString TLGView::position2string(int x) const {
  return time2string(position2time(x));
}

QString TLGView::formatTimeStamp(QString ts) const {
  
  const int step = 3;
  const char mychar = ',';
  for (int i = ts.length()-step; i>0; i=i-step)
      ts.insert(i, mychar);

  return ts;
}

QString TLGView::time2string(uint64_t t) const {
  int T;
  switch(currentMode){
    default:
    case TIME_MODE:
      return formatTimeStamp(QString::number(t / 1000)) + " ns";
    case FC_CYCLE_MODE:
      T = get_fc_period();
      if (T == 0) {
        std::cout << "[-] Error: cluster period = 0" << std::endl;
        exit(1);
      }
      return formatTimeStamp(QString::number(t / T));
    case CLUSTER_CYCLE_MODE:
      T = get_cluster_period();
      if (T == 0) {
        std::cout << "[-] Error: cluster period = 0" << std::endl;
        exit(1);
      }
      return formatTimeStamp(QString::number(t / T));
  }
}

QString TLGView::time2stringBis(uint64_t t) const {
  int T;
  switch(currentMode){
    default:
    case TIME_MODE:
      return formatTimeStamp(QString::number(t / 1000000)) + " us";
    case FC_CYCLE_MODE:
      T = get_fc_period();
      if (T == 0) {
        std::cout << "[-] Error: cluster period = 0" << std::endl;
        exit(1);
      }
      return formatTimeStamp(QString::number(t / T));
    case CLUSTER_CYCLE_MODE:
      T = get_cluster_period();
      if (T == 0) {
        std::cout << "[-] Error: cluster period = 0" << std::endl;
        exit(1);
      }
      return formatTimeStamp(QString::number(t / T));
  }
}

template <typename T>
int TLGView::drawDwt(const Data_with_time<T>& dwt, int x1, int y,
                        QPainter& painter, int signalId){
  signalId +=1;
  signalId -=1 ;
  int displayedItems = 0; // nb of displayed items
  int avg_factor = 1;
  int xa = time2position(dwt.begin);
  int xb = time2position(dwt.end);
  if (xa == -1) xa = 0;
  if (xb == -1) xb = x1;
  if (xa > xb){
    std::cout << "[-] warning: invalid element to display (begin > end)" << x1 << std::endl;
    return 0;
  }
  //std::cout <<  "[-] printing Signal #" << signalId << std::endl;

  /*
     depending on the type T of the template, we build the text description
     of the item to be displayed
  */
  std::string txtValue;
  if (! dwt.mixed_up){
    if (std::is_same<T, const char*>::value){
      // pointer trick, because the compiler does not understand that T is
      // nothing but const char* in this "if"
      const char* s;
      memcpy(&s, &(dwt.d), sizeof(const char*));
      //txtValue = std::to_string(signalId) + "-" + std::string(s);
      txtValue =  std::string(s);
    }
    else if (std::is_same<T, generic_data_t>::value){
      generic_data_t g;
      memcpy(&g, &(dwt.d), sizeof(generic_data_t));
      //txtValue = std::to_string(signalId) + "-" + std::to_string(g);
      txtValue =  std::to_string(g);
    }
    else if (std::is_same<T, g_decompressed_data_t>::value){
      g_decompressed_data_t ud;
      memcpy(&ud, &(dwt.d), sizeof(g_decompressed_data_t));
      txtValue = std::to_string(ud.value);
      avg_factor = ud.n_items_in_avg;
      if (ud.value!=0 && avg_factor!=1)
        //txtValue = std::to_string(signalId) + "-" + (std::to_string(ud.value) + "(avg. "
        txtValue =  (std::to_string(ud.value) + "(avg. "
                 + std::to_string(avg_factor) + ")" );
      else if (ud.value!=0)
        //txtValue = std::to_string(signalId) + "-" + (std::to_string(ud.value));
        txtValue =   (std::to_string(ud.value));
    }
    else {
      std::cout << "[-] Warning data type not recognized by the GUI" << std::endl;
      return 0;
    }
    painter.setBrush(QBrush(getColor(txtValue)));
  }
  else {
    painter.setBrush(QBrush(Qt::black));
  }

  if (txtValue.compare(std::string("0")) != 0) {
    y += rectRelativeOffset * peHeight;
    //std::cout << "QRect: xa=" << xa << " y=" << y << " xb-xa=" << xb-xa << " peHeight=" << peHeight << " rectSize=" << rectRelativeSize << std::endl;
    QRect curRect = QRect(xa, y, xb - xa, peHeight * rectRelativeSize);
    painter.drawRect(curRect);
    displayedItems++;
    // draw text in the center of the rectangle
    if ((xb - xa > functionNameMinSize) && (! dwt.mixed_up)) {
      painter.drawText(curRect, Qt::AlignCenter, QString::fromUtf8(txtValue.c_str()));
      displayedItems++;
    }
  }
  return displayedItems;
}

void TLGView::setFastHasRun(bool fast){
  fastHasRun = fast;
}

/*
void TLGView::paintEvent(QPaintEvent* event){
  
  const QRect r = event->rect();
  int x0 = r.x();
  int x1 = r.x() + r.width();
  uint64_t t0, t1;

  getTimeWindow(x0, x1, t0, t1);
  QPainter painter(this);

  drawMutex.lock();
  lock_data_mtx();
  auto data = get_timeline_data();

  int nlines=0;

  if (gvsocSlowMode)
    if (fastHasRun)
      nlines = data.size() + 1 + id_to_display().size() - 10;
    else
      nlines = data.size() + 1 + id_to_display().size();
  else {
    nlines = 11 + id_to_display().size() -9;
    // should be reset when we stop gvsoc. This works only when we pause
    fastHasRun=true; 
  }



  std::cout << "[-] data.size() = " << data.size() << std::endl;
  std::cout << "[-] id_to_display().size() = " << id_to_display().size() << std::endl;
  std::cout << "[-] nlines = " << nlines << std::endl;
  
  if (nlines == 0) {
    drawMutex.unlock();
    unlock_data_mtx();
    // std::cout << "[.] Nothing to draw"<<std::endl;
    return;
  }
  
  //peHeight = size().height() / nlines;
  // switch to a constant peHeight and see what happens
  peHeight = 30;
  //drawBackground(&painter, r, nlines);
  painter.setPen(pen);

  int line = 0;
  uint displayedItems = 0;
  */

  /**************************** draw function executions ***********************/
  // Display traces only in slow mode
  // std::cout << "gvsocSlowMode:" << gvsocSlowMode << std::endl;
  
  /*
  if (gvsocSlowMode) {
    for (auto& trace: data){
      for(auto iter = trace.between(t0, t1, zoomFactor); !iter.done(); ++iter){
        displayedItems += drawDwt<const char*>(*iter, x1, line * peHeight, painter);
      }
      line++;
    }
  } else {
    // draw all traces 
    for (int id : id_to_display()){
      auto l = event_timestamps(id);
      if (l != nullptr){
        for(auto item : decompress(l->between(t0, t1, zoomFactor))){
          displayedItems += drawDwt<g_decompressed_data_t>(
                                            item, x1, line * peHeight, painter);
        }
        if (line==8) {
          line++;
        }
        
      } 
      line++;
    }
  }
*/
  /*********************** draw custom signals in gvsocSlow Mode*****************************/
  /*
  if (gvsocSlowMode && (!fastHasRun)) {
    for (int id : id_to_display()){
      auto l = event_timestamps(id);
      if (l != nullptr){
        for(auto item : decompress(l->between(t0, t1, zoomFactor))){
          displayedItems += drawDwt<g_decompressed_data_t>(
                                              item, x1, line * peHeight, painter);
        }
      }
      line++;
    }
  }
  else if (gvsocSlowMode && (fastHasRun)) {
    // First 10 ones are  for signals in fast mode
    int i =1;
    for (int id : id_to_display()){
      if (i>10){
        auto l = event_timestamps(id);
        if (l != nullptr){
          for(auto item : decompress(l->between(t0, t1, zoomFactor))){
            displayedItems += drawDwt<g_decompressed_data_t>(
                                                item, x1, line * peHeight, painter);
          }
        }
        line++;
      }
      i++;
      //std::cout << "custom signals i: " << i << std::endl;
    }
  }
  maxY = line * peHeight;
  */
  /*std::cout << "[+] " << displayedItems << " items drawn" << std::endl;
  std::cout << "[+] " << line << " lines " << std::endl;
  */
 /*
  unlock_data_mtx();
*/
  /*********************** draw the vertical red line **********************/
  /*
  if (verticalLineTime){
    painter.setPen(redPen);
    painter.drawLine(time2position(verticalLineTime), 0,
                     time2position(verticalLineTime), size().height());
  }

  uint X = size().width();
  uint Y = line * peHeight;
*/

  /*********************** write the legend *******************************/
  /*
  painter.setPen(pen);
  painter.drawLine(0, line * peHeight, size().width(), line * peHeight);
  
  // calculating time interval for 10 ticsteps approximatly
  float vi= (position2time(X) - position2time(0))/10;
  int viRound = extractPower10Round(vi);
  uint64_t xtmin= (uint64_t) (position2time(0)/viRound) * viRound;
  uint64_t xtmax= (uint64_t) (position2time(X)/viRound) * viRound;
  

  QString textSave="";
  for (uint64_t xt=xtmin; xt<=xtmax; xt+= viRound){
    int x = time2position(xt);
    painter.drawLine(x, Y, x, Y + ticSize);
    QRect rect(x - legendWidth / 2, Y + ticSize + legendTextSpace, legendWidth,
        peHeight - ticSize - 1);
    if (textSave != time2stringBis(xt)) 
      painter.drawText(rect, Qt::AlignCenter, time2stringBis(xt));
    textSave= time2stringBis(xt);
  }
  */
  /*************************** Draw Ruler if needed *****************************/
  /*
  static bool wasPressed = false;
  if(m_nbMousePressed)
  {
    QPen pen1(Qt::red);
    QBrush brush1(Qt::red,Qt::Dense6Pattern);
    painter.setPen(pen1);
    painter.setBrush(brush1);
    painter.drawPixmap(0, 0, m_nPTargetPixmap);
    painter.drawRect(m_rect);
    wasPressed = true;
  }
  else if(wasPressed)
  {
    QPainter PixmapPainter(&m_nPTargetPixmap);
    QPen pen2(Qt::red);
    QBrush brush2(Qt::red,Qt::Dense4Pattern);
    PixmapPainter.setPen(pen2);
    PixmapPainter.setBrush(brush2);
    PixmapPainter.drawRect(m_rect);
    painter.drawPixmap(0, 0, m_nPTargetPixmap);
    wasPressed = false;
  }
  // updateLeftLegendSlow();
  
  drawMutex.unlock();
  
}*/

int TLGView::getSignalIndex(auto signalIdList , int signalId) {
  // search for the signalId in the vector. Returns its index in the list or -1 if not found

  std::vector<int>::iterator it = std::find(signalIdList.begin(), signalIdList.end(), signalId);
  if (it != signalIdList.end())
    // signal Id found in list
    return std::distance(signalIdList.begin(), it);
  else
    // signal Id not found in list
    return -1;
}


void TLGView::paintSignalToGview(QPainter& painter, const QString signalPath, 
                                auto data, auto signalIdList, int line, 
                                uint64_t t0, uint64_t t1, int x1) {
                                  
  QString pcSignalPath = signalPath;
  uint displayedItems = 0;
  // First Look if it's a core state signal
  if (pcSignalPath.contains("pe") & pcSignalPath.contains("state")) {
    pcSignalPath.replace("state", "pc");
  }
  else 
    pcSignalPath="";

  // Second look for the signal ID 
  int signalId = getSignalIdFromBackend(signalPath.toStdString());
  //std::cout << "[-] Get State Signal: " << signalPath.toStdString() << " Id: " << signalId << std::endl;

  // Get the signal index in list 
  int signalIdx = getSignalIndex(signalIdList, signalId);

  //std::cout << "timestamp " << t0 << "  " << t1 << std::endl;
  if ( (signalIdx != -1) & !(pcSignalPath.contains("pe") & pcSignalPath.contains("state"))) { 
    //std::cout << "[-] Paint State Signal " << signalId << " " << signalPath.toStdString()  << " line " << line << std::endl;     
    // Index exist ==> just paint the signal in the timeline view for the timeline interval
    for(auto iter = data[signalIdx].between(t0, t1, zoomFactor); !iter.done(); ++iter){

      displayedItems += drawDwt<const char*>(*iter, x1, line * peHeight, painter, signalId);
     
    }
    return;
  }
  else 
    //std::cout << " Signal ID " <<  signalId << " not found in signalIdList or not to be displayed" << std::endl;
  
  // Signal was not found in the cores traces
  // Then, search it in the basic signals 
  if (!(pcSignalPath.contains("pe") & pcSignalPath.contains("state"))) {
    if (std::find(id_to_display().begin(), id_to_display().end(), signalId) != id_to_display().end()) {
      // Signal found in the list ==> just paint it on the TLGView
      auto l = event_timestamps(signalId); 
      if (l != nullptr){
        //std::cout << "[-] Paint Other Signal " << signalId << " " << signalPath.toStdString()  << " line " << line << std::endl;
        for(auto item : decompress(l->between(t0, t1, zoomFactor))){
            displayedItems += drawDwt<g_decompressed_data_t>(
                                            item, x1, line * peHeight, painter, signalId);  
        }
      } 
    }
  } 
  
  if (pcSignalPath.compare(QString(""))!=0) {
    // painting  corresponding pc signal on the same line as the state signal
    int pcSignalId = getSignalIdFromBackend(pcSignalPath.toStdString());
    //std::cout << "[-] Get pc Signal: " << pcSignalPath.toStdString() << " Id: " << pcSignalId << std::endl;
    // Get the signal index in list 
    int pcSignalIdx = getSignalIndex(signalIdList, pcSignalId);
 
    if (pcSignalIdx != -1) {
      // We must print the corresponding pc Signal above the state signal
      //std::cout << "[-] Paint PC Signal " << pcSignalId << " " << pcSignalPath.toStdString()  << " line " << line << std::endl;     
      // Index exist ==> just paint the signal in the timeline view for the timeline interval
      for(auto iter = data[pcSignalIdx].between(t0, t1, zoomFactor); !iter.done(); ++iter){
        displayedItems += drawDwt<const char*>(*iter, x1, line * peHeight, painter, signalId);
      }
      return;
    }
  }
  show();
}

void TLGView::paintSubTree( QPainter& painter, 
                            QModelIndex parent, 
                            QModelIndex signalIdx , 
                            auto data, 
                            auto signalIdList, 
                            int* line, 
                            uint64_t t0, 
                            uint64_t t1, 
                            int x1){

    //std::cout << "[-] Paint Signals to GView: " << std::endl;
    int i = 0;
    QModelIndex idx;
    QModelIndex siblingIdx;
    // dump parent node
    if (parent != signalsView->rootIndex()){
        //std::cout << parent.data().toString().toStdString() << std::endl;
        //std::cout << signalIdx.data().toString().toStdString() << std::endl;
        (*line)++;
        //std::cout << "line = " << *line << std::endl;
        paintSignalToGview(painter, signalIdx.data().toString(), data, signalIdList,*line,
                          t0, t1, x1 );
    }
    

    //std::cout << "row count : "<<  signalsModel->rowCount(parent) << std::endl;

    if ((parent == signalsView->rootIndex()) || signalsView->isExpanded(parent)) {
        for (i=0; i< signalsModel->rowCount(parent) ; i++) {
            idx = signalsModel->index(i,0,parent);
            siblingIdx = signalsModel->index(i,1,parent);
            paintSubTree(painter, idx,siblingIdx,data,signalIdList,line, t0, t1,x1);
        }
    }
}

void TLGView::paintEvent(QPaintEvent* event){
  
  const QRect r = event->rect();
  int x0 = r.x();
  int x1 = r.x() + r.width();
  uint64_t t0, t1;

  getTimeWindow(x0, x1, t0, t1);
  QPainter painter(this);

  drawMutex.lock();
  lock_data_mtx();
  auto data = get_timeline_data();
  auto signalIdList = get_timeline_id();

  int nlines=0;

/*
  if (gvsocSlowMode)
    if (fastHasRun)
      nlines = data.size() + 1 + id_to_display().size() - 10;
    else
      nlines = data.size() + 1 + id_to_display().size();
  else {
    nlines = 11 + id_to_display().size() -9;
    // should be reset when we stop gvsoc. This works only when we pause
    fastHasRun=true; 
  }
*/

  //nlines = data.size() + id_to_display().size();
  // TODO: nlines is not well calculated anymore. 
  // Should be the size of the visualisation tree
  nlines = id_to_display().size();

  //std::cout << "[-] data.size() = " << data.size() << std::endl;
  //std::cout << "[-] id_to_display().size() = " << id_to_display().size() << std::endl;
  //std::cout << "[-] nlines = " << nlines << std::endl;
  
  if (nlines == 0) {
    drawMutex.unlock();
    unlock_data_mtx();
    // std::cout << "[.] Nothing to draw"<<std::endl;
    return;
  }
  
  //peHeight = size().height() / nlines;
  // switch to a constant peHeight and see what happens
  peHeight = 30;
  // drawBackground(&painter, r, nlines);
  painter.setPen(pen);

  int line = -1;
  //uint displayedItems = 0;

  // *****************  End paint Legend  **** ** 

  //std::cout << "-- PaintEvent : signalIdList Vector --" << std::endl;
  /*for (auto item = signalIdList.begin(); item != signalIdList.end(); ++item)
    std::cout << *item << std::endl;
  */
  // Looping through the Visual tree and painting signals in order
  paintSubTree(painter, signalsView->rootIndex(),signalsView->rootIndex(), data, signalIdList, &line,
              t0,t1,x1);
  // add extra line to match the signalsTreeView
  line++;
  /**************************** draw function executions ***********************/
  /*for (auto& trace: data){
      for(auto iter = trace.between(t0, t1, zoomFactor); !iter.done(); ++iter){
        displayedItems += drawDwt<const char*>(*iter, x1, line * peHeight, painter);
      }
      line++;
  }
  */
  /*
    for (int id : id_to_display()){
      // getting what's in "events" table filled by add_events_to_timeline
      // server->events[id]
      std::cout << "Display trace_Id: " << id << std::endl;
      auto l = event_timestamps(id); 
      if (l != nullptr){
        for(auto item : decompress(l->between(t0, t1, zoomFactor))){
          displayedItems += drawDwt<g_decompressed_data_t>(
                                            item, x1, line * peHeight, painter);
        }
      } else
      {
        std::cout << "Nothing to display for trace_Id: " << id << std::endl;
      }
      
      line++;
    }
  */
  maxY = line * peHeight;
  setMinimumSize(0,line * peHeight);
  /*std::cout << "[+] " << displayedItems << " items drawn" << std::endl;
  std::cout << "[+] " << line << " lines " << std::endl;
  */
  unlock_data_mtx();

  /*********************** draw the vertical red line **********************/
  if (verticalLineTime){
    painter.setPen(redPen);
    painter.drawLine(time2position(verticalLineTime), 0,
                     time2position(verticalLineTime), size().height());
  }

  //uint X = size().width();
  //uint Y = line * peHeight;

  /*********************** write the legend *******************************/
  // TODO: The legend must now be drawn into a separate widget that we 
  // Its class will called the HLegendWidget ...
  // Its repaint now will not need the peHeight anymore. 

  /*
  painter.setPen(pen);
  painter.drawLine(0, line * peHeight, size().width(), line * peHeight);
  
  // calculating time interval for 10 ticsteps approximatly
  float vi= (position2time(X) - position2time(0))/10;
  int viRound = extractPower10Round(vi);
  uint64_t xtmin= (uint64_t) (position2time(0)/viRound) * viRound;
  uint64_t xtmax= (uint64_t) (position2time(X)/viRound) * viRound;
  

  QString textSave="";
  for (uint64_t xt=xtmin; xt<=xtmax; xt+= viRound){
    int x = time2position(xt);
    painter.drawLine(x, Y, x, Y + ticSize);
    QRect rect(x - legendWidth / 2, Y + ticSize + legendTextSpace, legendWidth,
        peHeight - ticSize - 1);
    if (textSave != time2stringBis(xt)) 
      painter.drawText(rect, Qt::AlignCenter, time2stringBis(xt));
    textSave= time2stringBis(xt);
  }
  */
  /*************************** Draw Ruler if needed *****************************/
  static bool wasPressed = false;
  if(m_nbMousePressed)
  {
    QPen pen1(Qt::red);
    QBrush brush1(Qt::red,Qt::Dense6Pattern);
    painter.setPen(pen1);
    painter.setBrush(brush1);
    painter.drawPixmap(0, 0, m_nPTargetPixmap);
    painter.drawRect(m_rect);
    wasPressed = true;
  }
  else if(wasPressed)
  {
    QPainter PixmapPainter(&m_nPTargetPixmap);
    QPen pen2(Qt::red);
    QBrush brush2(Qt::red,Qt::Dense4Pattern);
    PixmapPainter.setPen(pen2);
    PixmapPainter.setBrush(brush2);
    PixmapPainter.drawRect(m_rect);
    painter.drawPixmap(0, 0, m_nPTargetPixmap);
    wasPressed = false;
  }
  
  drawMutex.unlock();
  
}


void TLGView::resizeEvent(__attribute__((unused)) QResizeEvent *event){
  setMaxScrollTime();
  setPageStep();
}

/* if index is negative, item is added to the end */
ProLabel* TLGView::insertLegendItem(int index, const char* str, bool addMenuAction){
  //std::cout << "Adding String to Legend: " << str <<  " ----" << std::endl;
  ProLabel* label = new ProLabel(str);
  label->setSizePolicy(QSizePolicy::Minimum,
                        QSizePolicy::Fixed);
  label->setFixedHeight(peHeight);
  // Inserting the label in the legend Layout
  legendLayout->insertWidget(index, label,0,Qt::AlignTop);

  //int realIndex = (index >= 0 ? index : legendLayout->count());
  // a stretch is also added to keep label aligned with the timeline
  //legendLayout->insertStretch(index < 0 ? index : index + 1, 2);
  //QLayoutItem* item = legendLayout->itemAt(realIndex);
  //QLayoutItem* stretch = legendLayout->itemAt(realIndex + 1);
  /*if (addMenuAction){
    label->setContextMenuPolicy(Qt::ActionsContextMenu);
    QAction* removeAction = new QAction(QString("Remove ") + str + " from timeline",
                                        this);
    connect(removeAction, &QAction::triggered, tl, [this, label, item, stretch](){
      //this->deleteLegendItem(item, stretch);
      deleteLegendItem(item)
      label->deleteLater();
    });
    
    label->addAction(removeAction);
  }*/
  addMenuAction = addMenuAction;
  legendLayout->update();
  this->update();

  return label;
  
}

static int searchChildInLayout(const QLayoutItem* item, const QLayout* layout){
  // reimplementation of the indexOf method (for Qt < 5.12 compatibility)
  for (int i = 0; i < layout->count(); i++){
    if (item == layout->itemAt(i)) return i;
  }
  return -1;
}

void TLGView::deleteLegendItem(QLayoutItem* item, QLayoutItem* stretch){
  int index = searchChildInLayout(item, legendLayout); //legendLayout->indexOf(item)
  legendLayout->removeItem(item);
  legendLayout->removeItem(stretch);
  if (index == -1) return;
  // we have to compute the index in the user-added trace list from the index in
  // the layout. There is a factor 2 because there are stretches between each
  // legend label
  int i = (index - 1 - 2 * Data_manager::TLHeight) / 2;
  if (i < 0) return;
  lock_data_mtx();
  remove_trace_from_timeline(i);
  unlock_data_mtx();
  update();
  //sArea->update();
  delete item;
  delete stretch;
}

void TLGView::clearWidgets(QLayout * layout) {
   if (! layout)
      return;
   while (auto item = layout->takeAt(0)) {
      delete item->widget();
      clearWidgets(item->layout());
   }
}

void TLGView::execMessage(const QString &text){
  msgBox.setText(text);
  msgBox.exec();
}

/*void TLGView::updateLeftLegendSlow(){
  ProLabel* fcLabel;
  // Update Left Legend
  if (legendLayout->count() == 0){ // first call
    legendLayout->addStretch(1);
    fcLabel = insertLegendItem(-1, "FC", false);
    connect(fcLabel,SIGNAL(clicked()),this, SLOT(execMessage(QString("FC clicked"))) );
    // -2 because FC and DMA are not in the loop
    std::cout << "Data_manager::TLHeight " << Data_manager::TLHeight << std::endl;
    for (int i = 0; i < Data_manager::TLHeight - 2; i++) {
      insertLegendItem(-1, ("PE " + std::to_string(i)).c_str(), false);
    }
    insertLegendItem(-1, "DMA", false);
    legendLayout->addWidget(new QLabel(" "));
    legendLayout->addStretch(1);
    legendLayout->addSpacing(((QApplication*) QApplication::instance())->style()
                              ->pixelMetric(QStyle::PM_ScrollBarExtent) * 1.9);
    //std::cout << "legendLayout->count: " << legendLayout->count() << std::endl;
    for(int i = 0; i < legendLayout->count(); i++){
      legendLayout->itemAt(i)->setAlignment(Qt::AlignCenter);
    }
  legendLayout->setSpacing(0);
  //QSpacerItem *spacer = new QSpacerItem(40, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);
  //legendLayout->addItem(spacer);
  //legendLayout->addStretch(1);
  }
  

}
*/
/*
void TLGView::updateLeftLegendFast(){
  
  // Update Left Legend in Fast  mode
  // Not used anymore ......
  if (legendLayout->count() == 0){ // first call
    legendLayout->addStretch(1);
    insertLegendItem(-1, "FC", false);
    // -2 because FC and DMA are not in the loop
    for (int i = 0; i < Data_manager::TLHeight - 2; i++) {
      insertLegendItem(-1, ("PE " + std::to_string(i)).c_str(), false);
    }
    
    insertLegendItem(-1, "DMA", false);
    
    legendLayout->addWidget(new QLabel(" "));
    legendLayout->addStretch(1);
    // keep following lines for fast mode
    legendLayout->addSpacing(((QApplication*) QAppliSlowcation::instance())->style()
                              ->pixelMetric(QStyle::PM_ScrollBarExtent) * 1.9);
    
    std::cout << "legendLayout->count: " << legendLayout->count() << std::endl;
    for(int i = 0; i < legendLayout->count(); i++){
      legendLayout->itemAt(i)->setAlignment(Qt::AlignCenter);
    }
    legendLayout->setSpacing(0);
    
  }
  
}

*/

int TLGView::extractPower10Round(int x){
  return pow(10,floor(log10(x) + 0.6));
}

void TLGView::addTraceToLegend(std::string path,bool addMenuAction){
  
  //std::cout << "##### AddTraceToLegend ##### " << path << std::endl;
  //std::cout << "path: " << path << std::endl;
  int i = path.size() - 1;
  for (; i >= 0 && path[i] != '/'; i--);
  int insertIdx = Data_manager::TLHeight * 2 + 1;
  //int insertIdx=1;
  // slow mode
  //int insertIdx=0;
  //std::cout << "size: " << (int) id_to_display().size() << std::endl;
  if (gvsocSlowMode)
    insertIdx += std::max(0, (int) id_to_display().size() - 1 ) * 2;
  else 
    insertIdx += std::max(0, (int) id_to_display().size() - 1 -10) * 2;
  //std::cout << "insertLegendItem insertIdx: " << insertIdx << std::endl;
  //insertLegendItem(insertIdx, path.c_str() + i, addMenuAction);
  insertLegendItem(insertIdx, path.c_str(), addMenuAction);
}

void TLGView::shiftAfterZoom(double coeff){
  uint64_t  t0, t1;
  getTimeWindow(t0, t1);
  int64_t delta = (int64_t) ((double)(t1 - t0) * coeff);
  timeOffset = (uint64_t) std::max((int64_t) 0, ((int64_t) timeOffset + delta));
  sbar->setValue(timeOffset * TLGView::scrollBarMaxRange / maxScrollTime);
}

void TLGView::zoomIn(){
  if (zoomFactor >= 1. / 10) return;
  zoomFactor *= 2;
  setMaxScrollTime();
  shiftAfterZoom(+1 / 2.);
  setPageStep();
  update();
  emit zoomOccured();
  //sArea->update();
}

void TLGView::zoomOut(){
  // prevent user from crashing the app by zoom out infinitly
  if (timeWidth() > 1.5 * maxTime) return;
  zoomFactor /= 2;
  setMaxScrollTime();
  shiftAfterZoom(-1 / 4.);
  setPageStep();
  update();
  emit zoomOccured();
  //sArea->update();
}

void TLGView::viewAll() {
  // Displays all the timeline in the Timeline Window
  zoomFactor = (double)(size().width() -30) / (double) maxTime;
  setMaxScrollTime();
  shiftAfterZoom(-1 / 4.);
  setPageStep();
  update();
  //sArea->update();
}



void TLGView::updateVerticalLine(QPointF q){
  verticalLineTime = position2time(q.x());
  updateVerticalLineText();
  update();
  //sArea->update();
}

void TLGView::updateVerticalLineText(){
  tl->setTimestamp(time2string(verticalLineTime));
}

void TLGView::updateTimeInterval(uint64_t position){
  uint64_t time = width2time(position);
  tl->setTimeInterval(time2string(time));
  update();
  //sArea->update();
}

int TLGView::getCoreNb(QString sigShortName){
  // get coreNb from signal short name
  int coreNb=-1;
  
  QRegularExpression re("PE\\dstate");
  QRegularExpression rd("\\d");
  int idx;
  if (sigShortName.contains(re)) {
    idx=sigShortName.indexOf(rd);
    coreNb = sigShortName[idx].digitValue();
  }
  return coreNb;
}

int TLGView::getCoreNb2(QString sigLongName){
  // get coreNb from signal short name
  int coreNb=-1;
  
  QRegularExpression re("pe\\d/state");
  QRegularExpression rd("\\d");
  int idx;
  if (sigLongName.contains(re)) {
    idx=sigLongName.indexOf(rd);
    coreNb = sigLongName[idx].digitValue();
  }
  return coreNb;
}


QString TLGView::findEltOnRow(  QModelIndex parent, 
                                QModelIndex signalIdx,
                                int rowNb,
                                int* rowIdx){
  // Finds the visible element on row rowNb in a QTreeView

  int i=0;
  // Loop on the treeView elements 
  //std::cout << "[-] FindEltOnRow " << std::endl;
  
  QModelIndex idx;
  QModelIndex siblingIdx;
  QString signalName;
  // treat parent node
  if (parent != signalsView->rootIndex()){
    (*rowIdx)++;
    if (*rowIdx==rowNb)
      return signalIdx.data().toString();
  }


  if ((parent == signalsView->rootIndex()) || signalsView->isExpanded(parent)) {
    for (i=0; i< signalsModel->rowCount(parent) ; i++) {
      idx = signalsModel->index(i,0,parent);
      siblingIdx = signalsModel->index(i,1,parent);
      signalName=findEltOnRow(idx,siblingIdx,rowNb, rowIdx);
      if (QString::compare(signalName, QString(""))!=0)
        return signalName;
    }
  }
  return QString("");
}

void TLGView::mousePressEvent(QMouseEvent* event)
{
    
    // Code for the ruler (time interval measurement)
    //std::cout << "[-] mousePressEvent " << std::endl;
    m_nbMousePressed = true;
    m_line.setP1(event->pos());
    m_line.setP2(event->pos());
    m_rect.setBottomLeft(event->pos());
    m_rect.setWidth(0);
    m_rect.setHeight(this->height());

    // Code for the timestamp 
    if (event->button() == Qt::LeftButton) {
      QPointF q(event->x(), event->y());
      // first draw a vertical line with timestamp 
      updateVerticalLine(q);
      //then select a function 
      if (peHeight == 0) return;
      // q.y() represents the coordinate of the point clicked in the ScrollArea
      // We need to find the row number of the signal in the scrollArea to calculate the core 
      // q.y()/peHeight represents the index of the signal in the signalsTreeView
      QPoint q1(10,q.y());
      //std::cout << "q.y() " << q.y() << std::endl;
      int rowNb =  1 + q.y()/peHeight;
      //std::cout << "rowNb " << rowNb << std::endl;
      int rowIdx=0;
      QString longSigName = findEltOnRow(signalsView->rootIndex(),
                                          signalsView->rootIndex(),
                                          rowNb,
                                          &rowIdx
                                          );
      //std::cout << "longSigName " << longSigName.toStdString() << std::endl;                                    
      //std::cout << "rowIdx " << rowIdx << std::endl;
      if (QString::compare(longSigName, QString("")) != 0) {
        int coreNb=getCoreNb2(longSigName);
        if (coreNb == -1) return;
        //std::cout << "coreNb " << coreNb << std::endl;
        const char* name = function_at(coreNb +1, position2time(q.x()));
        //std::cout << "function name " << name << std::endl;
        if (name == nullptr) return;

        // show functions Details Dock window if it was hidden
        functionsDock->show();
        fd->selectFunction(std::string(name));
        fd->selectRow(name);
        stallchart->construct(name);
      }
    }
}

void TLGView::mouseReleaseEvent()
{   
    // << "[-] mouseReleaseEvent " << std::endl;
    m_nbMousePressed = false;

    update(); // update your view
    //sArea->update();
}

void TLGView::mouseMoveEvent(QMouseEvent *event)
{   
    //std::cout << "[-] mouseMoveEvent " << std::endl;
    if (event->type() == QEvent::MouseMove)
    {
        // Trying to draw a horizontal line
        m_line.setP2(event->pos());
        auto diff = m_line.p2().x() - m_line.p1().x();
        uint64_t distance = abs(diff);
        if (diff>=0) {
          m_rect.setHeight(this->height());
          m_rect.setWidth(distance);
        } else {
          m_rect.setBottomLeft(event->pos());
          m_rect.setHeight(this->height());
          m_rect.setWidth(distance);
        }
        updateTimeInterval(distance);
        // Display difference of Time between P1 and P2
        
    }
    update(); // update your view
    //sArea->update();
}

void TLGView::centerOnMousePosition(uint64_t mouseTimestamp) {
// center TLGView on mouse position (X wise)
  //std::cout << "[-] Center on Mouse Position" << std::endl;
  
  uint64_t t0, t1;
  getTimeWindow(t0, t1);
  uint64_t midTimestamp= timeOffset + (t1-t0)/2 ;
  //std::cout << "[-] t0: " << t0 << std::endl;
  //std::cout << "[-] t1: " << t1 << std::endl;
  //std::cout << "[-] timeoffset before: " << timeOffset << std::endl;
  
  if (mouseTimestamp <= midTimestamp) {
    if (timeOffset >= (midTimestamp - mouseTimestamp))
      timeOffset = timeOffset - (midTimestamp - mouseTimestamp);
  } 
  else {
    timeOffset = timeOffset + (mouseTimestamp - midTimestamp);
  }
    
  //std::cout << "[-] : midTimestamp " << midTimestamp << std::endl;
  //std::cout << "[-] : mouseTimestamp " << mouseTimestamp << std::endl;
  //std::cout << "[-] : (midTimestamp - mouseTimestamp): " << midTimestamp - mouseTimestamp << std::endl;
  //std::cout << "[-] timeoffset after: " << timeOffset << std::endl;
  
  // readjust sbar->value to be consistent with timeOffset
  sbar->setValue(timeOffset * TLGView::scrollBarMaxRange / maxScrollTime);
  update();
  //sArea->update();
}

void TLGView::placeMouseTimestampOnPoint(uint64_t mouseTimestamp, QPointF mousePosition) {
// place the mouseTimestamp on  mouse position (X wise)
  
  uint64_t t0, t1;
  getTimeWindow(t0, t1);
  //uint64_t midTimestamp= timeOffset + (t1-t0)/2 ;
  //QPointF mousePosition(event->x(), event->y());
  uint64_t targetTimestamp = position2time(mousePosition.rx());
  //std::cout << "[-] t0: " << t0 << std::endl;
  //std::cout << "[-] t1: " << t1 << std::endl;
  //std::cout << "[-] timeoffset before: " << timeOffset << std::endl;
  
  if (mouseTimestamp <= targetTimestamp) {
    if (timeOffset >= (targetTimestamp - mouseTimestamp))
      timeOffset = timeOffset - (targetTimestamp - mouseTimestamp);
  } 
  else {
    timeOffset = timeOffset + (mouseTimestamp - targetTimestamp);
  }
    
  //std::cout << "[-] : midTimestamp " << midTimestamp << std::endl;timeline
  //sArea->update();
}

void TLGView::addToList(uint64_t mouseTimestamp){
  //viewLayout->addStretch(0);
  mouseTimestamps.push_back(mouseTimestamp);
  wheelEventCount++;
}

void TLGView::zoomThread(){

  // the access to this function is mutually exclusive
  mouseMutex.lock();
  //std::cout << "[-] ZoomThread started" << std::endl;
  // wait arbitrarily 2s till user is done with mouse scroll
  sleep(0.5);
  
  // clear mouseTimestamp list & wheelEventCount
  mouseTimestamps.clear();
  //std::cout << "[-] ZoomThread started 5" << std::endl;
  
  mouseMutex.unlock();

  wheelEventCount=0;
  //std::cout << "[-] ZoomThread ended" << std::endl;
}



void TLGView::wheelEvent(QWheelEvent *event)
{ 
  std::cout << "[-] WheelEvent" << std::endl;
  // Get Mouse Position and store in the list
  QPointF mousePosition(event->x(), event->y());
  uint64_t mouseTimestamp = position2time(mousePosition.rx());
  addToList(mouseTimestamp);

  if (wheelEventCount == 1) {

    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta() / 8;

    if (!numPixels.isNull()) {
        scrollWithPixels(numPixels);
    } else if (!numDegrees.isNull()) {
        QPoint numSteps = numDegrees / 15;
        scrollWithDegrees(numSteps);
    }

    // Get first mouseTimestamp 
    mouseTimestamp=mouseTimestamps.front();
    // center window on this timestamp if possible
    //centerOnMousePosition(mouseTimestamp);
    placeMouseTimestampOnPoint(mouseTimestamp, mousePosition);

    // Launch thread
    th1=std::thread(&TLGView::zoomThread,this);
    th1.detach();
  }

  event->accept();
  update();
  //sArea->update();
  // std::cout << "[-] WheelEvent Ended" << std::endl;
}

void TLGView::scrollWithPixels(const QPoint &pixel)
{
    
    int s = pixel.y();
    //double z = std::pow(1.01, s);
    std::cout << "Type of mouse wheel not yet implemented" << s << std::endl;
}

void TLGView::scrollWithDegrees(const QPoint &step)
{
    int s = step.y();
    if (s==1)
      zoomIn();
    else 
      zoomOut();

}

void TLGView::gotoTimestamp(int64_t t){
  updateVerticalLine(QPointF(time2position(t), 0));
  //centerOn(t, sceneRect().height() / 2);
}

void TLGView::sliderMoved(int x){
  timeOffset = x * maxScrollTime / scrollBarMaxRange;
  update();
  //sArea->update();
}

void TLGView::radioB1Clicked(){
  currentMode=TIME_MODE;
  // Need to act on the timestamp as it changed
  updateVerticalLineText();
  fd->switch2TimeMode();
  update();
  //sArea->update();
}
void TLGView::radioB2Clicked(){
  currentMode=FC_CYCLE_MODE;
  updateVerticalLineText();
  fd->switch2FCCycleMode();
  update();
  //sArea->update();
}

void TLGView::radioB3Clicked(){
  currentMode=CLUSTER_CYCLE_MODE;
  updateVerticalLineText();
  fd->switch2ClusterCycleMode();
  update();
  //sArea->update();
}

void TLGView::addSignalToGview(const QString signalPath) {
   
  if (add_trace_to_timeline(signalPath.toStdString()))
    std::cout << "[-] Add Signal to GView: " << signalPath.toStdString() << std::endl;
}

void TLGView::addSignalsToGview(QModelIndex parent, 
                                QModelIndex signalIdx){
    //std::cout << "[-] Add Signals to GView: " << std::endl;
    int i =0;
    QModelIndex idx;
    QModelIndex siblingIdx;
    // dump parent node
    if (parent != signalsView->rootIndex()){
        //std::cout << parent.data().toString().toStdString() << std::endl;
        //std::cout << signalIdx.data().toString().toStdString() << std::endl;
        addSignalToGview(signalIdx.data().toString());
    }
    else
         std::cout << "Root Index : "  << std::endl;

    //std::cout << "row count : "<<  signalsModel->rowCount(parent) << std::endl;

    if ((parent == signalsView->rootIndex()) || signalsView->isExpanded(parent)) {
        for (i=0; i< signalsModel->rowCount(parent) ; i++) {
            idx = signalsModel->index(i,0,parent);
            siblingIdx = signalsModel->index(i,1,parent);
            addSignalsToGview(idx,siblingIdx);
        }
    }
}

void TLGView::handleSignalNodeExpanded(){
    std::cout<< "[-] Adding Signals to Gview" << std::endl;
    /*std::cout << "Index expanded : " << view->isExpanded(index) << std::endl;
    std::cout << index.data(0).toString().toStdString() << std::endl;
    std::cout << index.sibling(0,1).data(0).toString().toStdString() << std::endl;
    */
    // dump the TreeView
    addSignalsToGview(signalsView->rootIndex(),signalsView->rootIndex()); 
    //What is needed is to call a repaint ... 
    this->repaint();
    this->update();
}

SignalTree::SignalTree( TLGView* gview, QWidget* parent) {
  this->setParent(parent);
  this->gview = gview;
  std::string txt;
  setModel(new TreeModel(get_trace_txt_list().c_str()));
  setContextMenuPolicy(Qt::ActionsContextMenu);
  addTraceAction = new QAction(QString("Add to timeline"), this);
  connect(addTraceAction, &QAction::triggered, this, &SignalTree::contextMenuHandler);
  addAction(addTraceAction);
  horizontalScrollBar()->setEnabled(true);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  resizeColumnToContents(0);
  setAutoScroll(false);
  connect(this, SIGNAL(expanded(QModelIndex)), this, SLOT(updateScrollArea(QModelIndex)));
  connect(this, SIGNAL(collapsed(QModelIndex)), this, SLOT(updateScrollArea(QModelIndex)));
}

void SignalTree::updateScrollArea(const QModelIndex& i)
{
  resizeColumnToContents(i.column());
}


void SignalTree::contextMenuHandler(){
  QModelIndexList indexes = selectionModel()->selectedRows();
  if (indexes.size() > 0) {
    QModelIndex selectedIndex = indexes.at(0);
    std::string path = "";
    std::vector<std::string> v;
    do {
      v.push_back(selectedIndex.data().toString().toStdString());
      selectedIndex = selectedIndex.parent();
    } while (selectedIndex != QModelIndex());
    for (auto c = v.rbegin(); c != v.rend(); c++){
      path += "/" + *c;
    }
    if (add_trace_to_timeline(path)) {
      //std::cout << "===== AddTraceToLegend " << path << std::endl;
      gview->addTraceToLegend(path);
      gview->update();
    }
    else {
      QMessageBox::warning(this, "Error",
        ("Impossible to add trace " + path + "\nFormat not supported yet").c_str());
    }
  }
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

void Timeline::addTraceToLegend(std::string path, bool addMenuAction){
  //std::cout << "====== 2 addTraceToLegend " << path << std::endl;
  gview->addTraceToLegend(path,addMenuAction);
  gview->update();
  //scrollArea->update();
}

void Timeline::completeToolBar(){

  const QIcon zoomInIcon = QIcon::fromTheme("zoom-in", QIcon(":/images/Apps-Zoom-In-icon.png"));
  const QIcon zoomOutIcon = QIcon::fromTheme("zoom-out", QIcon(":/images/Apps-Zoom-Out-icon.png"));
  const QIcon viewAllIcon = QIcon::fromTheme("zoom-fit", QIcon(":/images/Apps-Zoom-Fit-icon.png"));

  QAction *zoomInAct = new QAction(zoomInIcon, tr("Zoom In"), this);
  QAction *zoomOutAct = new QAction(zoomOutIcon, tr("Zoom Out"), this);
  QAction *viewAllAct = new QAction(viewAllIcon, tr("Zoom Fit"), this);

  //std::cout << "connects mainwindow line 271" << std::endl;
  connect(zoomInAct, &QAction::triggered, gview, &TLGView::zoomIn);
  connect(zoomOutAct, &QAction::triggered, gview, &TLGView::zoomOut);
  connect(viewAllAct, &QAction::triggered, gview, &TLGView::viewAll);

  connect(radioB1, &QRadioButton::toggled, gview, &TLGView::radioB1Clicked);
  connect(radioB2, &QRadioButton::toggled, gview, &TLGView::radioB2Clicked);
  timestamp = new QLineEdit("0 µs");
  timeInterval = new QLineEdit("0");
  timestamp->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  timeInterval->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  connect(timestamp, &QLineEdit::returnPressed, this, &Timeline::gotoGivenTimestamp);
  progressBar = new QProgressBar();
  progressBar->setMinimum(0);
  progressBar->setMaximum(0);
  progressBar->setFixedWidth(100);

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
  //toolBar->addSeparator();
  toolBar->addWidget(radioB1);
  toolBar->addWidget(radioB2);
  //toolBar->addWidget(radioB3);
  
  toBeRemoveAfterExec.append(
    toolBar->addWidget(new QLabel("   your code is being profiled...   ")));
  toBeRemoveAfterExec.append(toolBar->addWidget(progressBar));
  
  update();
}


void Timeline::foo(int x){
  std::cout << "foo called " << x << std::endl;
}

void Timeline::createSignalsTree(){
    QFile file(":/images/signalstree.txt");
    std::cout << "[-] Open Signals Hierarchy File" << std::endl;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
      qDebug("File signalstree.txt didn't open  ");
    std::cout << "[-] Create Signals Tree Model" << std::endl;
    //TreeModel model(file.readAll(),QString("path"),nullptr);
    model = new TreeModel(file.readAll());
    file.close();

    std::cout << "[-] Create Signals Tree View" << std::endl;
    signalsTreeView = new QTreeView();
    signalsTreeView->setModel(model);
    signalsTreeView->setWindowTitle(QObject::tr("Signals"));
    signalsTreeView->setStyleSheet("alternate-background-color:yellow;");
    // hide the column containing the full path of the signal
    signalsTreeView->hideColumn(1);
    signalsTreeView->setColumnWidth(200, 800);
    std::cout << "[-] Signals Tree View created" << std::endl;

}

void Timeline::setGviewVScrollValue(int value1) {
  // syncs the Scrollarea vertical scroll bar value according to the 
  // value of the signalsTreeView scrollbar
  //std::cout<< "[-] setGViewScrollValue " << value1 << std::endl;
  // First block the scrollArea vertical bar signals
  scrollArea->verticalScrollBar()->blockSignals(true);

  // First calculate the percentage of move on the signalsTreeView
  float min1 = (float) signalsTreeView->verticalScrollBar()->minimum(); 
  float max1 = (float) signalsTreeView->verticalScrollBar()->maximum();
  float percentage = 100.0/(max1-min1) * (float)value1;
  // Then get min and max of the   Vartical bar
  float min2 = (float) scrollArea->verticalScrollBar()->minimum(); 
  float max2 = (float) scrollArea->verticalScrollBar()->maximum();
  float value3 = round(percentage * (max2-min2)/100.0);
  int value2 = value3;
  // set the scrollArea scroll bar value
  scrollArea->verticalScrollBar()->setValue(value2);

  // update widget position
  QPoint topLeft = scrollArea->viewport()->rect().topLeft();
  scrollArea->widget()->move(0, topLeft.y() - value2);

  // Finally unblock scrollarea signals
  scrollArea->verticalScrollBar()->blockSignals(false);

}

Timeline::Timeline( QMainWindow* mw, 
                    QToolBar* toolBar,
                    FunctionDetails* fd,
                    QDockWidget* functionsDock,
                    QPlainTextEdit* sourceCode,
                    QPlainTextEdit* asmCode,
                    StallChart* stallchart):
                  mw(mw),
                  toolBar(toolBar),
                  functionsDock(functionsDock),
                  sourceCode(sourceCode),
                  asmCode(asmCode),
                  stallchart(stallchart)
{

  // Add a group button to the toolBar
  buttonGroup = new QButtonGroup(this); 
  radioB1 = new QRadioButton("Time Mode");
  radioB2 = new QRadioButton("Cluster Cycle Mode");
  //radioB3 = new QRadioButton(" FC Cycle Mode");
  buttonGroup->addButton(radioB1); 
  buttonGroup->addButton(radioB2);
  //buttonGroup->addButton(radioB3);
  radioB1->setChecked(true);

  
  // construct hierarchy

  //Define Timeline layout
  //tlLayout = new QHBoxLayout();
  tlLayout = new QGridLayout();
  this->setLayout(tlLayout);

  // Create SignalsTree View
  std::cout << "Create Signal Tree" << std::endl;
  createSignalsTree();
  signalsTreeView->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
  signalsTreeView->setHeaderHidden(true);
  //signalsTreeView->header()->setMaximumSectionSize(32);
  //signalsTreeView->header()->setMaximumSectionSize(32);
  //signalsTreeView->setStyleSheet("border: 2px solid orange");
  //signalsTreeView->verticalScrollBar()->hide();
  signalsTreeView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  signalsTreeView->update();
  signalsTreeView->show();

  treeLabel = new QLabel();
  treeLabel->setText("Signals");
  treeLabel->setFixedHeight(30);
  verTreeLayout = new QVBoxLayout();
  //verTreeLayout->addWidget(treeLabel);
  //verTreeLayout->addWidget(signalsTreeView);

  tlLayout->addWidget(signalsTreeView,1,0);

  std::cout << "Create Horizontal ScrollBar" << std::endl;

  scrollbar = new QScrollBar(Qt::Horizontal);
  //scrollbar->setStyleSheet("border: 2px solid red");
  scrollbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  
  
  std::cout << "Create gview" << std::endl;
  // Add the Timeline Graphic View
  gview = new TLGView(fd, functionsDock, this, scrollbar, legendLayout, 
                      sourceCode, asmCode,stallchart, scrollArea,
                      model, signalsTreeView);
  gview->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  completeToolBar();
  //signalsTreeView->header()->setFixedHeight(32);
  //gview->setStyleSheet("border: 2px solid brown");

  verViewW = new QWidget();
  verViewLayout = new QVBoxLayout();
  verViewW->setLayout(verViewLayout);
  
  //create scroll area containing 
  std::cout << "Create ScrollArea" << std::endl;
  scrollArea = new AdjustingScrollArea(signalsTreeView,this);
  scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  //scrollArea->horizontalScrollBar()->hide();
	scrollArea->setWidgetResizable(true);
  //scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  scrollArea->setWidget(gview); 
  scrollArea->ensureWidgetVisible(gview);
  //scrollArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
  //scrollArea->setStyleSheet("border: 2px solid green");
  /*std::cout << "sig pagestep" << signalsTreeView->verticalScrollBar()->pageStep() << std::endl;
  std::cout << "sig min" << signalsTreeView->verticalScrollBar()->minimum() << std::endl;
  std::cout << "sig max" << signalsTreeView->verticalScrollBar()->maximum() << std::endl;
  std::cout << "sArea pagestep" << scrollArea->verticalScrollBar()->pageStep() << std::endl;
  std::cout << "sArea min" << scrollArea->verticalScrollBar()->minimum() << std::endl;
  std::cout << "sArea max" << scrollArea->verticalScrollBar()->maximum() << std::endl;
  */
  scrollArea->verticalScrollBar()->setPageStep(signalsTreeView->verticalScrollBar()->pageStep());
  scrollArea->verticalScrollBar()->setMinimum(signalsTreeView->verticalScrollBar()->minimum());
  scrollArea->verticalScrollBar()->setMaximum(signalsTreeView->verticalScrollBar()->maximum());
  scrollArea->verticalScrollBar()->setSingleStep(signalsTreeView->verticalScrollBar()->singleStep());
  //scrollArea->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
  //std::cout << "sArea pagestep" << scrollArea->verticalScrollBar()->pageStep() << std::endl;

  hLegendW = new HLegendWidget(gview);
  //hLegendW->setVerticalPolicy(QSizePolicy::Fixed);
  hLegendW->setStyleSheet("border: 2px solid red");
  connect(gview, SIGNAL(zoomOccured()), hLegendW, SLOT(update()));
  connect(scrollbar, SIGNAL(valueChanged(int)), hLegendW, SLOT(update()));
  
  //verViewLayout->addWidget(hLegendW,0);
  verViewLayout->addWidget(scrollArea);
  verViewLayout->addWidget(scrollbar);
  verViewLayout->setMargin(0);
  verViewLayout->setSpacing(0);
  //verViewLayout->setAlignment(hLegendW,Qt::AlignTop);
  verViewLayout->setAlignment(scrollbar,Qt::AlignBottom);

  tlLayout->addWidget(treeLabel,0,0);
  tlLayout->addWidget(verViewW,1,1);
  tlLayout->addWidget(hLegendW,0,1);
  tlLayout->setRowMinimumHeight(0,30);
  tlLayout->setVerticalSpacing(0);

  QObject::connect(signalsTreeView, SIGNAL(expanded(QModelIndex)),
                     gview, SLOT(handleSignalNodeExpanded()));
  QObject::connect(signalsTreeView, SIGNAL(collapsed(QModelIndex)),
                     gview, SLOT(handleSignalNodeExpanded()));
  
  
  /*QObject::connect(signalsTreeView->verticalScrollBar(), SIGNAL(valueChanged(int)), 
                  scrollArea->verticalScrollBar(), SLOT(setValue(int)));*/
  QObject::connect(signalsTreeView->verticalScrollBar(), SIGNAL(valueChanged(int)), 
                  this, SLOT(setGviewVScrollValue(int)));
  QObject::connect(scrollArea->verticalScrollBar(), SIGNAL(valueChanged(int)), 
                  signalsTreeView->verticalScrollBar(), SLOT(setValue(int)));
  
  connect(scrollbar, &QScrollBar::valueChanged, gview, &TLGView::sliderMoved);
  
  std::cout << "[-] Timeline window  created" << std::endl;
  
  scrollbar->update();
  hLegendW->update();
  signalsTreeView->update();
  gview->update();
  gview->raise();
  verViewW->update();
  scrollArea->update();
  this->update();

 
  scrollbar->show();
  hLegendW->show();
  gview->show();
  verViewW->show();
  scrollArea->show();
  signalsTreeView->show();
  this->show();
  //viewW->show();
  

}

Timeline::~Timeline(){
  // need to delete the buttonGroup
  delete buttonGroup;
  delete timestamp;
  delete timeInterval;
  delete zoomInAct;
  delete zoomOutAct;
  delete viewAllAct;
  delete progressBar;
  delete timeLabel;
  delete intervalLabel;
  delete radioB1;
  delete radioB2;
  // delete radioB3;
  clearCurrentExecToolBar();
}

void Timeline::resizeEvent(__attribute__((unused)) QResizeEvent *event){

}

void Timeline::clearCurrentExecToolBar(){
  for (auto c: toBeRemoveAfterExec){
    toolBar->removeAction(c);
    }
  toBeRemoveAfterExec.clear();
}
