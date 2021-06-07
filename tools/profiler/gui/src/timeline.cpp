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
    qDebug() << "[-] eventFilter";
    // Before sync, we should repaint the graphic view unless it's not
    // right
    tlgView->addSignalsToGview(signalsTreeView->rootIndex(),signalsTreeView->rootIndex());
    tlgView->update();
    syncWithTreeView();
    tlgView->update();
    tlgView->show();
    this->update();
    this->show();
  }

  //std::cout << "[-] end eventFilter" << std::endl;
  return QScrollArea::eventFilter(obj, ev);
}

void AdjustingScrollArea::syncWithTreeView() {
     qDebug() << "[-] syncWithTreeView" ;
    // Problem is when we enter this function, sometimes, the SignalsTreeView scrollbar
    // range is not yet updated.
    // So first, update signalsTreeView
    signalsTreeView->update();
    signalsTreeView->verticalScrollBar()->update();
    syncMutex.lock();
    // First block the scrollArea vertical bar signals
    this->verticalScrollBar()->blockSignals(true);

    // First calculate the percentage of move on the signalsTreeView
    double  min1 = (double) signalsTreeView->verticalScrollBar()->minimum();
    double  max1 = (double) signalsTreeView->verticalScrollBar()->maximum();
    if ((max1 - min1) <= 0) {
      qDebug() << "Error no sync possible: max1 -min1 <=0";
      syncMutex.unlock();
      return;
    } else {
      // Then get min and max of the   Vartical bar
      double min2 = (double) verticalScrollBar()->minimum();
      double max2 = (double) verticalScrollBar()->maximum();
      double factor = (max2 - min2) / (max1-min1) ;
      //qDebug() << "min-max " << min1 << " " << max1 << " " << min2 << " " << max2;
      //qDebug() << "factor " << factor;
      double value4= factor * (double) signalsTreeView->verticalScrollBar()->value();
      double value3= round(value4);
      int value2 = value3;
      //qDebug() << "signals slider value" << signalsTreeView->verticalScrollBar()->value();
      //qDebug() << "scrollarea slider value (double) " << value4;
      //qDebug() << "round'scrollarea slider value) (double) " << value3;
      //qDebug() << "scrollarea slider value (int)" << value2;
      // set the scrollArea scroll bar value
      this->verticalScrollBar()->setValue(value2);

      // update widget position
      QPoint topLeft = viewport()->rect().topLeft();
      this->widget()->move(0, topLeft.y() - value2);

      // Finally unblock scrollarea signals
      this->verticalScrollBar()->blockSignals(false);
      this->update();
      this->show();
    }
    syncMutex.unlock();
}

void AdjustingScrollArea::updateWidgetPosition(){
  qDebug() << "[-] updateWidgetPosition" ;
  int vvalue = verticalScrollBar()->value();
  QPoint topLeft = viewport()->rect().topLeft();

  widget()->move(0, topLeft.y() - vvalue);
}

AdjustingScrollArea::AdjustingScrollArea(QTreeView* signalsView,
                                         TLGView* gview,
                                         QWidget * parent) : QScrollArea{parent} {
  signalsTreeView = signalsView;
  tlgView=gview;
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

  // calculating time interval for 5 ticsteps approximatly
  float vi= (gview->position2time(X) - gview->position2time(0))/10;
  int viRound = gview->extractPower10Round(vi);
  uint64_t xtmin= (uint64_t) (gview->position2time(0)/viRound) * viRound;
  uint64_t xtmax= (uint64_t) (gview->position2time(X)/viRound) * viRound;


  QString textSave="";
  if ((xtmax-xtmin)/viRound > 12)
    viRound = viRound *2;
  if ((xtmax-xtmin)/viRound < 5)
    viRound = viRound/2;
  int firstText=true;
  for (uint64_t xt=xtmin; xt<=xtmax; xt+= viRound){
    int x = gview->time2position(xt);
    // paint vertical stick of the legend for the given timestamp
    painter.drawLine(x, Y, x, Y - gview->ticSize);
    QRect rect( x - gview->legendWidth / 2,
                Y - gview->peHeight,
                gview->legendWidth,
                gview->peHeight - gview->ticSize - 1);
    // Don't write first time number on legend as it would be half written
    if (!firstText) {
      painter.drawText(rect, Qt::AlignCenter, gview->time2stringBis(xt));
    } else {
      firstText=false;
    }
  }


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
                  TreeModel* signalsModel,
                  QTreeView* signalsView,
                  StatModel* statModelRef,
                  QTableView*  statTableViewRef
                  ):
                  functionsDock(functionsDock),
                  sourceCode(sourceCode),
                  asmCode(asmCode),
                  stallchart(stallchart),
                  signalsModel(signalsModel),
                  signalsView(signalsView),
                  statModel(statModelRef),
                  statTableView(statTableViewRef)
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
  //qDebug() << "this->sbar: " << this->sbar ;
  this->sbar->setRange(0, TLGView::scrollBarMaxRange);
  setPageStep();

  m_nbMousePressed = false;

  connect(&timer, SIGNAL(timeout()), this, SLOT(autoReconstruct()));
  timer.start(TLGView::autoRedrawPeriod);
  qDebug() << "End create Gview: " ;
}

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
  assert(maxScrollTime!= 0);
  assert(zoomFactor!=0);
  this->sbar->setPageStep(TLGView::scrollBarMaxRange * (t1 - t0) / maxScrollTime);
  this->sbar->setSingleStep(std::max(1, (int) (TLGView::scrollCoeff / zoomFactor)));
}

void TLGView::setMaxScrollTime(){
  // << "[-]setMaxScrollTime " << std::endl;
  uint64_t w = timeWidth();
  // add a 30 pixel margin after last event (no margin looks weird)
  //std::cout << "maxTime " << maxTime << std::endl;
  //std::cout << "w " << w << std::endl;
  //std::cout << "zoomFactor" << zoomFactor << std::endl;
  assert(zoomFactor!=0);
  if (maxTime > w)
    maxScrollTime = maxTime - w + 30 / zoomFactor;
  else
    maxScrollTime = 1;

  //std::cout << "maxScrollTime " << maxScrollTime << std::endl;
  // readjust sbar->value to be consistent with timeOffset
  assert(maxScrollTime!= 0);
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
  assert(zoomFactor!= 0);
  t1 = timeOffset + (uint64_t) ((x1 - x0) / zoomFactor);
}

int TLGView::time2position(uint64_t t) const {
  if (t < timeOffset) return -1;   // not in display range
  return round((double) (t - timeOffset) * zoomFactor);
}

uint64_t TLGView::position2time(int x) const {
  assert(zoomFactor!= 0);
  return timeOffset + (uint64_t) ((double) x / zoomFactor);
}

uint64_t TLGView::width2time(int x) const {
  assert(zoomFactor!= 0);
  return (uint64_t) ((double) x / zoomFactor);
}

uint64_t TLGView::timeWidth() const{
  assert(zoomFactor!= 0);
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
      // format in us unit
      return QString::number(double(t) / double(1000000),'f',2 );
      //return formatTimeStamp(QString::number(double(t) / double(1000000),'g',2 ));
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
                        QPainter& painter){

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
  txtValue = std::string("1");
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
    //qDebug() << "QRect: xa=" << xa << " y=" << y << " xb-xa=" << xb-xa << " peHeight=" << peHeight << " rectSize=" << rectRelativeSize << std::endl;
    //qDebug() << "y0=" << y - rectRelativeOffset * peHeight;
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

int TLGView::getSignalIndex(std::vector<int> signalIdList , int signalId) {
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
                                std::vector<TLData<const char*>> data, std::vector<int> signalIdList, int line,
                                uint64_t t0, uint64_t t1, int x1) {

  QString pcSignalPath = signalPath;
  uint displayedItems = 0;
  // First Look if it's a core or fc & state signal
  if (pcSignalPath.contains("pe") & pcSignalPath.contains("state")) {
    pcSignalPath.replace("state", "pc");
  } else if (pcSignalPath.contains("fc") & pcSignalPath.contains("state")) {
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
    //qDebug() << "[-] Paint State Signal " << signalId << " " << signalPath  << " line " << line ;
    // Index exist ==> just paint the signal in the timeline view for the timeline interval
    for(auto iter = data[signalIdx].between(t0, t1, zoomFactor); !iter.done(); ++iter){
      displayedItems += drawDwt<const char*>(*iter, x1, line * peHeight, painter);
    }
    return;
  }
  //else
    //std::cout << " Signal ID " <<  signalId << " not found in signalIdList or not to be displayed" << std::endl;

  // Signal was not found in the cores traces
  // Then, search it in the basic signals
  if (!(pcSignalPath.contains("pe") & pcSignalPath.contains("state"))) {
    if (std::find(id_to_display().begin(), id_to_display().end(), signalId) != id_to_display().end()) {
      // Signal found in the list ==> just paint it on the TLGView
      auto l = event_timestamps(signalId);
      if (l != nullptr){
        //qDebug() << "[-] Paint Other Signal " << signalId << " " << signalPath  << " line " << line;
        for(auto item : decompress(l->between(t0, t1, zoomFactor))){
            displayedItems += drawDwt<g_decompressed_data_t>(
                                            item, x1, line * peHeight, painter);
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
      //qDebug() << "[-] Paint PC Signal " << pcSignalId << " " << pcSignalPath  << " line " << line ;
      // Index exist ==> just paint the signal in the timeline view for the timeline interval
      for(auto iter = data[pcSignalIdx].between(t0, t1, zoomFactor); !iter.done(); ++iter){
        displayedItems += drawDwt<const char*>(*iter, x1, line * peHeight, painter);
      }
      return;
    }
  }
  show();
}

void TLGView::paintSubTree( QPainter& painter,
                            QModelIndex parent,
                            QModelIndex signalIdx ,
                            std::vector<TLData<const char*>> data,
                            std::vector<int> signalIdList,
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
  // [x0, x1] correxponds to the TLGView X coordinates
  int x0 = r.x(); // first abcisse of the event rectangle
  int x1 = r.x() + r.width(); // second abcisse of the event rectangle
  uint64_t t0, t1;

  // calculate corresponding Time Range values
  getTimeWindow(x0, x1, t0, t1);
  QPainter painter(this);

  drawMutex.lock();
  lock_data_mtx();
  auto data = get_timeline_data();
  auto signalIdList = get_timeline_id();

  //qDebug() << "signalsView rows nb :" << signalsView->height()/30;

  // switch to a constant peHeight and see what happens
  peHeight = 30;
  // drawBackground(&painter, r, nlines);
  painter.setPen(pen);

  int line = -1;
  paintSubTree(painter, signalsView->rootIndex(),signalsView->rootIndex(), data, signalIdList, &line,
              t0,t1,x1);
  // add extra line to match the signalsTreeView ?? what's this? might be wrong
  line++;
  maxY = line * peHeight;
  setMinimumSize(0,line * peHeight);
  unlock_data_mtx();

  /*********************** draw the vertical red line **********************/
  if (drawLine){
    painter.setPen(redPen);
    painter.drawLine(time2position(verticalLineTime), 0,
                     time2position(verticalLineTime), size().height());
  }

  /*************************** Draw Ruler if needed *****************************/
  //static bool wasPressed = false;
  QPen pen1(Qt::red);
  QBrush brush1(Qt::red,Qt::Dense6Pattern);
  painter.setPen(pen1);
  painter.setBrush(brush1);

  if(drawRect)
  {
    // update selected area rectangle and draw it
    if (!mouseMoving) {
      m_rect.setBottomLeft(QPoint(time2position(t0Select),0));
      m_rect.setWidth(time2position(t1Select) - time2position(t0Select));
    }
    m_rect.setHeight(this->height());
    painter.drawRect(m_rect);
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

int TLGView::extractPower10Round(int x){
  return pow(10,floor(log10(x) + 0.6));
}

void TLGView::addTraceToLegend(std::string path,bool addMenuAction){

  //qDebug() << "##### AddTraceToLegend ##### " << path;

  int i = path.size() - 1;
  for (; i >= 0 && path[i] != '/'; i--);
  int insertIdx = Data_manager::TLHeight * 2 + 1;

  if (gvsocSlowMode)
    insertIdx += std::max(0, (int) id_to_display().size() - 1 ) * 2;
  else
    insertIdx += std::max(0, (int) id_to_display().size() - 1 -10) * 2;

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

}

void TLGView::viewAll() {
  // Displays all the timeline in the Timeline Window
  zoomFactor = (double)(size().width() -30) / (double) maxTime;
  setMaxScrollTime();
  shiftAfterZoom(-1 / 4.);
  setPageStep();
  update();

}



void TLGView::updateVerticalLine(QPointF q){
  verticalLineTime = position2time(q.x());
  updateVerticalLineText();
  update();

}

void TLGView::updateVerticalLineText(){
  tl->setTimestamp(time2string(verticalLineTime));
}

void TLGView::updateTimeInterval(uint64_t position){
  uint64_t time = width2time(position);
  tl->setTimeInterval(time2string(time));
  update();

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
    // Dealing with mouse press event
    // set usefull variables for this event
    m_line.setP1(event->pos());
    m_line.setP2(event->pos());
    if (!selectionRect) {
      // If no selection rectangle is already displayed
      m_rect.setBottomLeft(event->pos());
      m_rect.setWidth(0);
      m_rect.setHeight(this->height());
    }

    // used to pinpoint functions
    functionPoint.setX(event->x());
    functionPoint.setY(event->y());

  if (event->button() == Qt::LeftButton) {
    leftClick = true;
  } else if (event->button() == Qt::RightButton) {
    rightClick=true;
  }
}


void TLGView::highlightFunction(QPointF q) {
    // This function highlights the function on which the user clicked
    // in the functions table.
    if (peHeight == 0) return;

    // q.y() represents the coordinate of the point clicked in the ScrollArea
    // We need to find the row number of the signal in the scrollArea
    // qy()/peHeight represents the index of the signal in the signalsTreeView
    int rowNb =  1 + q.y()/peHeight;
    int rowIdx=0;
    QString longSigName = findEltOnRow(signalsView->rootIndex(),
                                        signalsView->rootIndex(),
                                        rowNb,
                                        &rowIdx
                                        );
    if (QString::compare(longSigName, QString("")) != 0) {
      int coreNb=getCoreNb2(longSigName);
      if (coreNb == -1) return;
      const char* name = function_at(coreNb +1, position2time(q.x()));
      if (name == nullptr) return;
      // show functions Details Dock window if it was hidden
      functionsDock->show();
      fd->selectFunction(std::string(name));
      fd->selectRow(name);
      stallchart->construct(name);
    }
}

void TLGView::mouseReleaseEvent(QMouseEvent *event) {

  if (mouseMoving & rightClick) {

    // set variables values for the selected area
    m_line.setP2(event->pos());

    if (!selectionRect) {
      // We need to draw the selection rectangle and calculate the time interval
      t0Select = this->position2time(m_line.p1().x());
      t1Select = this->position2time(m_line.p2().x());
    } else if (movingLeftEdge || movingRightEdge) {
    // A selection rectangle has already been displayed
    // We need to just adjust the rectangle edges
      if (movingLeftEdge) {
        t0Select = this->position2time(m_line.p2().x());
        t1Select = t1SelectDisplayed;
      } else  {
        t0Select = t0SelectDisplayed;
        t1Select = this->position2time(m_line.p2().x());
      }
    }

    if (t1Select < t0Select) {
      // inverse the values
      uint64_t t0Save = t0Select;
      t0Select = t1Select;
      t1Select = t0Save;
    }
    tl->updateSignalsStatView(signalsModel, signalsView,t0Select,t1Select);
    // update m_rect
    m_rect.setBottomLeft(QPoint(time2position(t0Select),0));
    m_rect.setWidth(time2position(t1Select) - time2position(t0Select));
    // update the time interval display field
    auto diff = m_rect.right() - m_rect.left();
    uint64_t distance = abs(diff);
    updateTimeInterval(distance);

    drawRect = true;
    selectionRect = true;
    // save the selection area abcisses that is displayed
    t0SelectDisplayed = t0Select;
    t1SelectDisplayed = t1Select;
    selectionRect=true;
  }
  else if (leftClick){
    updateVerticalLine(functionPoint);// updates the timestamp value
    highlightFunction(functionPoint); // highlights the function wher the click occured
    drawLine = true; // used when repainting the gview to draw a vertical line
  }
  else if (rightClick) {
    // Nothing to draw : nor timestamp line, nore selected area
    drawLine = false;
    drawRect = false;
    selectionRect = false;
    t0Select=0;
    t1Select=0;
  }

   // reset all flags
   rightClick = false;
   leftClick  = false;
   mouseMoving = false;
   movingRightEdge=false;
   movingLeftEdge=false;

   update();
}

void TLGView::mouseMoveEvent(QMouseEvent *event) {

  if ((event->type() == QEvent::MouseMove) & rightClick) {   // & click inside TLGView
    if (!selectionRect) {
      // No Selection rectangle has been displayed yet
      m_line.setP2(event->pos());  // ??
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

      // Display difference of Time between P1 and P2
      updateTimeInterval(distance);

      drawRect = true;
      mouseMoving = true;
    }
    else {
      // A selection rectangle has already been displayed
      // check if right click close to one of the edges of the rectangle
      m_line.setP2(event->pos());
      if (abs(m_line.p1().x() - time2position(t1SelectDisplayed)) <= 5) {
        m_rect.setRight(m_line.p2().x());
        movingLeftEdge = false;
        movingRightEdge=true;
      } else if (abs(m_line.p1().x() - time2position(t0SelectDisplayed)) <= 5) {
        m_rect.setLeft(m_line.p2().x());
        movingLeftEdge = true;
        movingRightEdge=false;
      }
      auto diff = m_rect.bottomRight().x() - m_rect.bottomLeft().x();
      uint64_t distance = abs(diff);
      // Display difference of Time between P1 and P2
      updateTimeInterval(distance);
      drawRect = true;
      mouseMoving = true;
    }
  }
  update();
}


void TLGView::centerOnMousePosition(uint64_t mouseTimestamp) {
// center TLGView on mouse position (X wise)

  uint64_t t0, t1;
  getTimeWindow(t0, t1);
  uint64_t midTimestamp= timeOffset + (t1-t0)/2 ;

  if (mouseTimestamp <= midTimestamp) {
    if (timeOffset >= (midTimestamp - mouseTimestamp))
      timeOffset = timeOffset - (midTimestamp - mouseTimestamp);
  }
  else {
    timeOffset = timeOffset + (mouseTimestamp - midTimestamp);
  }

  // readjust sbar->value to be consistent with timeOffset
  sbar->setValue(timeOffset * TLGView::scrollBarMaxRange / maxScrollTime);
  update();

}

void TLGView::placeMouseTimestampOnPoint(uint64_t mouseTimestamp, QPointF mousePosition) {
// place the mouseTimestamp on  mouse position (X wise)

  uint64_t t0, t1;
  getTimeWindow(t0, t1);

  uint64_t targetTimestamp = position2time(mousePosition.rx());

  if (mouseTimestamp <= targetTimestamp) {
    if (timeOffset >= (targetTimestamp - mouseTimestamp))
      timeOffset = timeOffset - (targetTimestamp - mouseTimestamp);
  }
  else {
    timeOffset = timeOffset + (mouseTimestamp - targetTimestamp);
  }

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
   //qDebug<< "[-] WheelEvent";
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

  // std::cout << "[-] WheelEvent Ended" << std::endl;
}

void TLGView::mouseDoubleClickEvent(QMouseEvent* event) {
  QPointF mousePosition(event->x(), event->y());
  uint64_t mouseTimestamp = position2time(mousePosition.rx());

  zoomIn();
  placeMouseTimestampOnPoint(mouseTimestamp, mousePosition);

  update();
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

}

void TLGView::radioB1Clicked(){
  currentMode=TIME_MODE;
  // Need to act on the timestamp as it changed
  updateVerticalLineText();
  fd->switch2TimeMode();
  update();

}
void TLGView::radioB2Clicked(){
  currentMode=FC_CYCLE_MODE;
  updateVerticalLineText();
  fd->switch2FCCycleMode();
  update();

}

void TLGView::radioB3Clicked(){
  currentMode=CLUSTER_CYCLE_MODE;
  updateVerticalLineText();
  fd->switch2ClusterCycleMode();
  update();

}

void TLGView::addSignalToGview(const QString signalPath) {

  add_trace_to_timeline(signalPath.toStdString());
}

void TLGView::addSignalsToGview(QModelIndex parent,
                                QModelIndex signalIdx){
    //qDebug() << "[-] Add Signals to GView" ;
    int i =0;
    QModelIndex idx;
    QModelIndex siblingIdx;
    // dump parent node
    if (parent != signalsView->rootIndex()){
        //std::cout << parent.data().toString().toStdString() << std::endl;
        //std::cout << signalIdx.data().toString().toStdString() << std::endl;
        addSignalToGview(signalIdx.data().toString());
    }

    //std::cout << "row count : "<<  signalsModel->rowCount(parent) << std::endl;

    if ((parent == signalsView->rootIndex()) || signalsView->isExpanded(parent)) {
        for (i=0; i< signalsModel->rowCount(parent) ; i++) {
            idx = signalsModel->index(i,0,parent);
            siblingIdx = signalsModel->index(i,1,parent);
            addSignalsToGview(idx,siblingIdx);
        }
    }
}


void TLGView::handleSignalNodeCollapsed() {
  handleSignalNodeExpanded();
   qDebug() << "[-] handleSignalNodeCollapsed" ;
}

// Handles the sync with the timeline view whenever a node in the signals
// Tree View is expanded or collapsed. As both operations need to browse the
// signal tree and repaint the timeline window accordingly.
void TLGView::handleSignalNodeExpanded(){

    qDebug() << "[-] handleSignalNodeExpanded: Adding/removing  Signals to Gview" ;
    // First add signals to the timeline
    addSignalsToGview(signalsView->rootIndex(),signalsView->rootIndex());
    // Update stats Models & view if already created
    // We must check here if gvsoc has already been run. If not , this
    // call to updateSignalsStatView ends in a core dump because .....
    // gvsocRunFlag is useless as it is set before gvsoc is really started
    // qDebug() << "[-] gvsocRunFlag: " << tl->gvsocRunFlag;
    if (tl->gvsocRunFlag) {
      tl->updateSignalsStatView(signalsModel, signalsView);
    }

    // Then, sync the two views so signals are displayed in front of their name
    this->repaint();
    tl->setGviewVScrollValue();
    tl->update();
    this->update();
    this->show();
    tl->show();
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

  //connect(radioB1, &QRadioButton::toggled, gview, &TLGView::radioB1Clicked);
  //connect(radioB2, &QRadioButton::toggled, gview, &TLGView::radioB2Clicked);
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
  //toolBar->addWidget(radioB1);
  //toolBar->addWidget(radioB2);
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
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
      qDebug("File signalstree.txt didn't open  ");
    // need to cjeck if model exists ---
    model = new TreeModel(file.readAll());
    file.close();
    signalsTreeView = new QTreeView();
    signalsTreeView->setModel(model);
    signalsTreeView->setWindowTitle(QObject::tr("Signals"));
    signalsTreeView->setStyleSheet("alternate-background-color:yellow;");
    // hide the column containing the full path of the signal
    signalsTreeView->hideColumn(1);
    signalsTreeView->setColumnWidth(200, 800);
    signalsTreeView->expandToDepth(0);
}

void Timeline::setGviewVScrollValue() {

  qDebug() << "[-] setGViewVScrollValue () " ;
  int value = signalsTreeView->verticalScrollBar()->value();
  signalsTreeView->verticalScrollBar()->setValue(signalsTreeView->verticalScrollBar()->minimum());
  signalsTreeView->verticalScrollBar()->setValue(value);
  //setGviewVScrollValue(signalsTreeView->verticalScrollBar()->value());
  scrollArea->syncWithTreeView();
  gview->update();
  gview->show();
  scrollArea->update();
  scrollArea->show();
}

void Timeline::setGviewVScrollValue(int value1) {
  // syncs the Scrollarea vertical scroll bar value according to the
  // value of the signalsTreeView scrollbar
  qDebug() << "[-] setGViewVScrollValue (int value1) " ;
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


void Timeline::updateSignalsStatView( TreeModel* sigModel,
                                      QTreeView* signalsTreeView,
                                      uint64_t t0,
                                      uint64_t t1) {
  // Display signals statistics between x1 and x2
  //displaySignalsStats(auto m_line.p1().x(),m_line.p2().x());

  if ((gview!= NULL) && (statModel != NULL) && (statTableView != NULL)){
    delete statModel;
    statModel= new StatModel(this);
    statModel->updateTreeParameters(sigModel, signalsTreeView);
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

void Timeline::updateSignalsStatView( TreeModel* sigModel,
                                      QTreeView* signalsTreeView) {
  //std::cout << "[-] Timeline::updateSignalsStatView()  " << std::endl;
  // Display signals statistics between previously set range [t0Select, t1Select]
  if ((gview!= NULL) && (statModel != NULL) && (statTableView != NULL)) {
    delete statModel;
    statModel= new StatModel(this);
    statModel->updateTreeParameters(sigModel, signalsTreeView);
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

  // Assertions
  assert(mw != NULL);
  assert(toolBar != NULL);
  assert(fd != NULL);
  assert(functionsDock != NULL);
  assert(sourceCode != NULL);
  assert(asmCode != NULL);
  assert(stallchart != NULL);

  // Add a group button to the toolBar
  //qDebug() << "[-] Create Button Group";
  //buttonGroup = new QButtonGroup(this);
  //radioB1 = new QRadioButton("Time Mode");
  //radioB2 = new QRadioButton("Cluster Cycle Mode");
  //buttonGroup->addButton(radioB1);
  //buttonGroup->addButton(radioB2);
  //radioB1->setChecked(true);

  // construct hierarchy

  //Define Timeline layout as a Grid Layout
  tlLayout = new QGridLayout();
  this->setLayout(tlLayout);

  // Create SignalsTree View
  qDebug() << "[-] Create Signals Tree";
  createSignalsTree();
  signalsTreeView->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
  signalsTreeView->setHeaderHidden(true);
  signalsTreeView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  signalsTreeView->verticalScrollBar()->setSingleStep(30);
  signalsTreeView->verticalScrollBar()->setPageStep(120);
  signalsTreeView->update();
  treeLabel = new QLabel();
  treeLabel->setText("Time (us)");
  treeLabel->setFixedHeight(30);
  statLabel = new QLabel();
  statLabel->setText("Signal Time Up (%)");
  statLabel->setFixedHeight(30);
  tlLayout->addWidget(signalsTreeView,1,0);

  // Create the stats Model
  statModel= new StatModel(this);
  // Create the TableView for signals statistics
  // Connect Model to the TableView
  statTableView = new QTableView();
  //statTableView->setModel(statModel);

  // Make table header unvisible
  statTableView->horizontalHeader()->setVisible(false);

  //statTableView->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  statTableView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  statTableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  statTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  statTableView->verticalScrollBar()->setPageStep(signalsTreeView->verticalScrollBar()->pageStep());
  statTableView->verticalScrollBar()->setMinimum(signalsTreeView->verticalScrollBar()->minimum());
  statTableView->verticalScrollBar()->setMaximum(signalsTreeView->verticalScrollBar()->maximum());
  statTableView->verticalScrollBar()->setSingleStep(signalsTreeView->verticalScrollBar()->singleStep());
  // statTableView->setColumnWidth(0,100); should be done after setting the model
  statTableView->setAlternatingRowColors(true);
  //statTableView->horizontalHeader()->setSectionResizeMode(QtWidgets.QHeaderView.Stretch);
  //statTableView->setSizeAdjustPolicy(QtWidgets.QAbstractScrollArea.AdjustToContents);

  // Create TL horizontal scroll bar
  qDebug() << "[-] Create Horizontal ScrollBar";
  scrollbar = new QScrollBar(Qt::Horizontal);
  scrollbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  // Create the Timeline Graphic View
  qDebug() << "[-] Create gview" ;
  gview = new TLGView(fd, functionsDock, this, scrollbar, legendLayout,
                      sourceCode, asmCode, stallchart, model, signalsTreeView,
                      statModel, statTableView);
  completeToolBar();
  qDebug() << "[-] Tool Bar Completed" ;

  verViewW = new QWidget();
  verViewLayout = new QVBoxLayout();

  verViewW->setLayout(verViewLayout);

  //create scroll area containing
  qDebug() << "[-] Create ScrollArea";
  scrollArea = new AdjustingScrollArea(signalsTreeView,gview,this);
  scrollArea->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
  scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	scrollArea->setWidgetResizable(true);
  scrollArea->setWidget(gview);
  scrollArea->ensureWidgetVisible(gview);
  scrollArea->verticalScrollBar()->setPageStep(signalsTreeView->verticalScrollBar()->pageStep());
  scrollArea->verticalScrollBar()->setMinimum(signalsTreeView->verticalScrollBar()->minimum());
  scrollArea->verticalScrollBar()->setMaximum(signalsTreeView->verticalScrollBar()->maximum());
  scrollArea->verticalScrollBar()->setSingleStep(signalsTreeView->verticalScrollBar()->singleStep());
  scrollArea->setWidgetResizable(true);
  scrollArea->adjustSize();

  // Create horizontal Legend for the gview
  qDebug() << "[-] hLegendWidget";
  hLegendW = new HLegendWidget(gview);
  hLegendW->setStyleSheet("border: 2px solid red");
  connect(gview, SIGNAL(zoomOccured()), hLegendW, SLOT(update()));
  connect(scrollbar, SIGNAL(valueChanged(int)), hLegendW, SLOT(update()));

  qDebug() << "[-] Create scrollArea + scrollBar container widget";
  verViewLayout->addWidget(scrollArea);
  verViewLayout->addWidget(scrollbar);
  verViewLayout->setMargin(0);
  verViewLayout->setSpacing(0);
  verViewLayout->setAlignment(scrollbar,Qt::AlignBottom);
  verViewW->adjustSize();


  qDebug() << "[-] Fill up  Grid Layout";
  tlLayout->addWidget(treeLabel,0,0);
  tlLayout->addWidget(verViewW,1,1);
  tlLayout->addWidget(hLegendW,0,1);
  tlLayout->addWidget(statTableView,1,2, Qt::AlignRight);
  tlLayout->addWidget(statLabel,0,2, Qt::AlignRight);
  //statLabel->setVisible(false);
  //statTableView->setVisible(false);
  tlLayout->setColumnStretch(2,0);
  tlLayout->setColumnStretch(0,0);
  tlLayout->setColumnStretch(1,800);
  tlLayout->setColumnMinimumWidth(2,50);

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

  QObject::connect(signalsTreeView->verticalScrollBar(),SIGNAL(rangeChanged(int,int)),
                  statTableView->verticalScrollBar(), SLOT(setRange(int,int)),Qt::QueuedConnection);
  QObject::connect(signalsTreeView->verticalScrollBar(), SIGNAL(valueChanged(int)),
                  statTableView->verticalScrollBar(), SLOT(setValue(int)),Qt::QueuedConnection);

  QObject::connect(this, SIGNAL(timeUpValueChanged()),
                   this, SLOT(refreshTimelineWidgets()),
                   Qt::QueuedConnection);
  connect(scrollbar, &QScrollBar::valueChanged, gview, &TLGView::sliderMoved);

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

  qDebug() << "[-] Timeline window  displayed";
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
  //delete radioB1;
  //delete radioB2;
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

void Timeline::refreshTimelineWidgets() {
    statTableView->repaint();
    gview->repaint();
    scrollArea->repaint();
    this->repaint();
    mw->repaint();
    statTableView->show();
    gview->show();
    scrollArea->show();
    this->show();
    mw->show();
}
