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
#include <QProgressBar>

#include <thread>
#include <mutex>
#include <QMutex>
#include <unistd.h>

#include "functiondetails.hpp"
#include "treemodel.hpp"
#include "stallchart.hpp"
#include "prolabel.hpp"

#include "tldata.hpp"
#include "treemodel.hpp"
#include "statmodel.hpp"
#include <QTableView>

class Timeline;
class TLGView;
class StatModel;

class AdjustingScrollArea : public QScrollArea {
  Q_OBJECT
  bool eventFilter(QObject * obj, QEvent * ev);
public:
  AdjustingScrollArea(QTreeView* signalsView,
                      TLGView* gview,
                       QWidget * parent = 0);
  void setWidget(QWidget *w);
  void updateWidgetPosition();
  QTreeView* signalsTreeView;
  TLGView* tlgView;
public slots:
  void syncWithTreeView();
private:
    QMutex syncMutex;
};

class HLegendWidget : public QWidget
{
  Q_OBJECT

public:

    HLegendWidget(TLGView* gview);

  protected:
    void paintEvent(QPaintEvent* event) override;

  private:
    TLGView* gview;
    QPen pen;

};

/**
 @brief TLGView stands for TimeLine Graphic View. This class is responsible for the
 display in a QWidget
 */
class TLGView : public QWidget
{
  Q_OBJECT

public:
  const static double rectRelativeSize;
  const static double rectRelativeOffset;
  const static int ticStep;        /**< time axis tic step, in pixels */
  const static int legendWidth;   /**< width of labels of the time axis */
  const static int legendTextSpace;/**< space between two labels */
  const static int ticSize;     /**< tic length on the time axis */
  const static int functionNameMinSize; /**< minimum width of a rect for the function name to be displayed */
  const static int autoRedrawPeriod; /**< number of ms between 2 updates of the view */
  const static int scrollBarMaxRange; /**< range of the scrollBar */
  const static double scrollCoeff; /**< defines how fast user can scroll */

  TLGView(FunctionDetails* fd,
          QDockWidget* functionsDock,
          Timeline* tl,
          QScrollBar* sbar,
          QVBoxLayout* legendLayout,
          QPlainTextEdit* sourceCode,
          QPlainTextEdit* asmCode,
          StallChart* stallchart,
          TreeModel* model,
          QTreeView* signalsView,
          StatModel* statModelRef,
          QTableView*  statTableViewRef);

public:
  int getSignalIndex(std::vector<int> signalIdList , int signalId);
  void addSignalToGview(QString signalPath);
  void addSignalsToGview (QModelIndex parent,QModelIndex signalIdx );

  // For the selection range being displayed
  uint64_t t0Select=0;
  uint64_t t1Select=0;
  // For saving the selection range already displayed
  uint64_t t0SelectDisplayed=0;
  uint64_t t1SelectDisplayed=0;

public slots:
  //void show();
  void zoomIn();
  void zoomOut();
  void viewAll();

  void gotoTimestamp(int64_t t);
  /**
    changes time unit. The mode can be seconds, or cluster cycle, ...
    The cycle mode (cluster or soc) it is based on the last registered cycle
    period, so it might be wrong it cycle period has changed
  */

  //void toggleMode();
  void radioB1Clicked();
  void radioB2Clicked();
  void radioB3Clicked();
  void sliderMoved(int x);

  uint getLineHeight() const { return peHeight; }

  //void updateLeftLegendFast();
  //void updateLeftLegendSlow();
  void addTraceToLegend(std::string path, bool addMenuAction=true);
  void deleteLegendItem(QLayoutItem* item, QLayoutItem* stretch);
  void handleSignalNodeCollapsed();
  void handleSignalNodeExpanded();

signals:
  void zoomOccured();

protected:
  void paintEvent(QPaintEvent* event) override;
  void wheelEvent(QWheelEvent*) override;
  QString findEltOnRow( QModelIndex parent,
                              QModelIndex signalIdx,
                              int rowNb,
                              int* rowIdx);
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event);
  void highlightFunction(QPointF q);
  void mouseDoubleClickEvent(QMouseEvent* event);
  void drawBackground(QPainter *painter, const QRect &exposed, int nlines);
  void scrollWithPixels(const QPoint &pixel);
  void scrollWithDegrees(const QPoint &step);
  int extractPower10Round(int x);

  private:
  // GvsocMode
  bool gvsocSlowMode=false;

  // for the ruler & selection area on gview-- added by Corine
  QLine m_line; // line drawn by mouse move event
  QRect m_rect; // rectangle drawn by mouse move event
  bool selectionRect= false; // true if a selection rect is displayed on gview
  bool movingLeftEdge = false; // true if user is currently adjusting left edge of select rect
  bool movingRightEdge = false; // true if user is currently adjusting right edge of select rect
  QPixmap m_nPTargetPixmap; // pixmap for displaying the time range selection area
  bool m_nbMousePressed;
  bool drawRect = false;
  bool drawLine = false;
  bool mouseMoving = false;
  bool leftClick=false;
  bool rightClick=false;
  QPointF functionPoint;
  QWidget* parentWidget;


  void updateVerticalLine(QPointF q);
  void updateVerticalLineText();
  void updateTimeInterval(uint64_t position);
  void resizeEvent(QResizeEvent *event) override;

  /** gives the time range currently displayed to the user */
  void getTimeWindow(uint64_t& t0, uint64_t& t1) const;
  /** gives the time range that corresponds to the pixels between x0 and x1 */
  void getTimeWindow(int x0, int x1, uint64_t& t0, uint64_t& t1) const;
  uint64_t timeWidth() const;

  void setMaxScrollTime();
  void shiftAfterZoom(double coeff);
  void setPageStep();


  void paintSignalToGview(QPainter& painter, const QString signalPath,
                                std::vector<TLData<const char*>> data, std::vector<int> signalIdList, int line,
                                uint64_t t0, uint64_t t1, int x1);


  void paintSubTree(QPainter& painter,
                    QModelIndex parent,
                    QModelIndex signalIdx,
                    std::vector<TLData<const char*>> data,
                    std::vector<int> signalIdList,
                    int* line,
                    uint64_t t0,
                    uint64_t t1,
                    int x1);

  /** converts a timestamp to a position in pixel in the widget */
  int time2position(uint64_t t) const;
  /** converts a position in pixel in the widget to a timestamp */
  uint64_t position2time(int x) const;
  /** converts a width in pixel in the widget to a time interval */
  uint64_t width2time(int x) const;


  /** @return a string description of the given position
              (e.g. 123 pixels -> "432 ns")
  */
  QString position2string(int x) const;
  QString time2string(uint64_t t) const;
  QString time2stringBis(uint64_t t) const;
  QString formatTimeStamp(QString ts) const;

  void centerOnMousePosition(uint64_t mouseTimestamp);
  void placeMouseTimestampOnPoint(uint64_t mouseTimestamp, QPointF mousePosition);
  void zoomThread();
  void addToList(uint64_t mouseTimestamp);

  std::list<uint64_t> mouseTimestamps; /** list to store the different mousetimestamps during a wheelEvent **/
  std::mutex mouseMutex; /** mutex used to help manage the mouse centered zoom **/
  uint16_t wheelEventCount =0; /** nb of wheelEvents during one mouse scroll zoom commande **/
  std::thread th1;

  QColor getColor(std::string fname);
  void drawCustomTraces();
  void clearWidgets(QLayout * layout);


  int getCoreNb(QString sigShortName);
  int getCoreNb2(QString sigLongName);


  /**
    draws a Data_with_time element
    this function is not trivial to implement because all supported types T must
    be handled correctly
  */
  template <typename T> int drawDwt(const Data_with_time<T>& dwt, int x1, int y,
                                    QPainter& painter);
  //QString getLegendModeText();

  ProLabel* insertLegendItem(int index, const char* str, bool addMenuAction);


  double zoomFactor;    /**< number of pixels per time unit */
  uint64_t timeOffset;  /**< timestamp of the leftmost displayed pixel */
  uint64_t maxTime;     /**< timestamp of the last event */
  uint64_t maxScrollTime; /**< user cannot scroll beyond this timestamp */
  uint64_t verticalLineTime;  /**< abscissa of the red vertical line */
  uint64_t timeIntervalValue;// Value of the time interval displayed by the mouse move
  uint maxY;
  uint peHeight=30;  /**< height in pixels of the timeline for 1 core */
  uint nCore;
  Timeline* tl;
  FunctionDetails* fd;


  // Widgets we need to communicate with
  QDockWidget* functionsDock;
  QPlainTextEdit* sourceCode;
  QPlainTextEdit* asmCode;
  StallChart* stallchart;
  QScrollArea* sArea;

  TreeModel* signalsModel;
  QTreeView* signalsView;
  StatModel* statModel;
  QTableView* statTableView;


  QTabWidget* tabw;
  QMessageBox msgBox;

  //int functionsIdx;
  QScrollBar* sbar;
  QVBoxLayout* legendLayout;
  QPen pen, greenPen, redPen;
  QTimer timer;
  QMutex drawMutex;
  LegendMode currentMode=TIME_MODE;

  std::unordered_map<std::string, QColor> functionColors;
  int hueStep = 90;
  int currentHue = 0;

  friend class HLegendWidget;

  private slots:
  /** checks whether new data has arrived from gvsoc. If yes, view is updated */
  void autoReconstruct();
  void execMessage(const QString &text);

};


/**
  @brief class to let the user browse and choose a trace to add to the timeline
*/
class SignalTree : public QTreeView
{
  Q_OBJECT
public:
  SignalTree(TLGView* gview, QWidget* parent);

private:
  QAction* addTraceAction;
  TLGView* gview;

private slots:
  void updateScrollArea(const QModelIndex& i);
  void contextMenuHandler();

};


/**
  @brief class to let the user browse and choose a group of traces to add to the timeline
*/
/*class SignalsCommand : public
{
  Q_OBJECT
public:
  SignalTree(TLGView* gview, QWidget* parent);

private:
  QAction* addTraceAction;
  TLGView* gview;

private slots:
  void updateScrollArea(const QModelIndex& i);
  void contextMenuHandler();

};
*/

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
            QToolBar* toolBar,
            FunctionDetails* fd,
            QDockWidget* functionsDock,
            QPlainTextEdit* sourceCode,
            QPlainTextEdit* asmCode,
            StallChart* stallchart, 
            QString signalsTreeFileName);
  ~Timeline();
  void setTimestamp(const QString& s) {timestamp->setText(s); }
  void setTimeInterval(const QString& s) {timeInterval->setText(s); }
  void resizeEvent(QResizeEvent *event) override;
  void setModeButtonText(QString s);
  void foo(int x);
  void clearCurrentExecToolBar();
  void addTraceToLegend(std::string path, bool addMenuAction=true);
  TLGView* getTLGView(){return gview;};
  //void setGviewVScrollValue();
  void updateSignalsStatView( TreeModel* sigModel,
                              QTreeView* signalsTreeView,
                              uint64_t t0,
                              uint64_t t1);
  void updateSignalsStatView( TreeModel* sigModel,
                              QTreeView* signalsTreeView);
  void changeSignalsStatVisibility(bool visible);
  void updateFlags(bool gvsocRun) {gvsocRunFlag = gvsocRun;};
  // Flags
  // gvsocRunFlag is set to True when gvsoc has started running
  // gvsocRunFlag is set to False when a new gvsoc process is opened
  bool gvsocRunFlag = false;
  // Timeline Scroll Area
  //QScrollArea* scrollArea;
  AdjustingScrollArea* scrollArea;

private:
  void makeLegend();
  void completeToolBar();
  void gotoGivenTimestamp();
  void createSignalsTree(QString signalsTreeFileName);

  QMainWindow* mw;
  QWidget *viewW; // widget for the signalsTreeView + verViewV
  QVBoxLayout* mainLayout;
  QToolBar* toolBar;
  QVBoxLayout* legendLayout;
  QHBoxLayout* viewLayout;
  QGridLayout* tlLayout;
  QTreeView* signalsTreeView;
  QWidget* hLegendW ;
  QWidget* bottomW; //bottom timeline view
  QWidget* signalsW; // headerW + signalsTreeView
  QVBoxLayout* bottomLayout;

  // widget for the gview + scrollbar
  QWidget* verViewW;
  QVBoxLayout* verViewLayout;
  TreeModel* model;
  QLabel *treeLabel;
  QLabel *statLabel;
  QVBoxLayout* verTreeLayout;



  // Timeline Graphic View
  TLGView *gview = NULL;

  // Statistics Table
  StatModel *statModel = NULL;
  QTableView *statTableView = NULL;
  // Dock windows we need to communicate with
  QDockWidget* functionsDock;
  QPlainTextEdit* sourceCode;
  QPlainTextEdit* asmCode;
  StallChart* stallchart;

  // For the tool Bar
  QPushButton* zoomIn;
  QPushButton* zoomOut;
  QPushButton* viewAll;
  QButtonGroup* buttonGroup;
  //QRadioButton* radioB1;
  //QRadioButton* radioB2;
  QRadioButton* radioB3;
  QLineEdit* timestamp;
  QLineEdit* timeInterval;

  QAction* zoomInAct;
  QAction* zoomOutAct;
  QAction* viewAllAct;

  QLabel* timeLabel;
  QLabel* intervalLabel;

  QScrollBar* scrollbar;
  QProgressBar* progressBar;

  QList<QAction*> toBeRemoveAfterExec;

signals:
  void timeUpValueChanged();

public  slots:

  //void setGviewVScrollValue(int value1);
  void setGviewVScrollValue();
  void setGviewVScrollValue(int value1);
  void refreshTimelineWidgets();


private slots:

};

#endif // TIMELINE_HPP
