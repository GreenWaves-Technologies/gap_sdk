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

#ifndef TLGVIEW_HPP
#define TLGVIEW_HPP

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
#include <unistd.h>

#include "functiondetails.hpp"
#include "treemodel.hpp"
#include "stallchart.hpp"
#include "prolabel.hpp"
#include "signaltree.hpp"

//#include "tldata.hpp"
//#include "statmodel.hpp"
#include <QTableView>

#include "profiler_backend_user.hpp"
#include "profiler_backend.hpp"

class Timeline;
class StatModel;

class Display {
public:
    Display();
    Display(QString displayType, QString message);
    ~Display();
    QString displayType;
    QString message;
};

class SigValData {
public:
    int jStart; // pixel coordinate of where value must be written
    int jEnd; // pixel of last pixel where value can be written
    double value;
    int getSpace();
    QString getHexaStr(int sigWidth);
};

class SigFuncData {
public:
    int jStart; // pixel coordinate of where value must be written
    int jEnd; // pixel of last pixel where value can be written
    QString value;
    int getSpace();
};

class Stick
{
public:
    Stick(int xr, int y1r, int y2r, QString textr, bool mainr);
    int x; // abcisse of the stick on the legend
    int y1,y2; // Stick drawn from y1 to y2
    bool main; // true for a main stick
    QString text;
};

/**
 @brief TLGView stands for TimeLine Graphic View. This class is responsible for the
 display of all the signals in a QWidget
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

    TLGView(Timeline* tl,
            QScrollBar* sbar,
            QVBoxLayout* legendLayout,
            SignalTree* signalsView,
            Profiler_backend_user* backendUserRef,
            js::config *json);
    /*
   * TLGView(FunctionDetails* fd,
          QDockWidget* functionsDock,
          Timeline* tl,
          QScrollBar* sbar,
          QVBoxLayout* legendLayout,
          QPlainTextEdit* sourceCode,
          QPlainTextEdit* asmCode,
          StallChart* stallchart,
          SignalTree* signalsView,
          StatModel* statModelRef,
          QTableView*  statTableViewRef,
          Profiler_backend_user* backendUserRef);
          */

public:

    // methods for managing signals
    int getSignalIndex(std::vector<int> signalIdList , int signalId);
    void addSignalToGview(QString signalPath);
    void addSignalsToGview (QModelIndex parent,QModelIndex signalIdx );
    void updateMe(Profiler_view_results *results);
    // pushes the x coordinate of a hLegend main tic
    void pushGridStick(Stick sti);
    // Clears the grid tics stored
    void clearGridSticks();

    // Returns the nb of signals drawn that have a dedicated place in gView
    int getSignalsNb(){return traceId.size();}

    // For the selection range being displayed
    uint64_t t0Select=0;
    uint64_t t1Select=0;
    // For saving the selection range already displayed
    uint64_t t0SelectDisplayed=0;
    uint64_t t1SelectDisplayed=0;
    // For saving the last  time range required with getData
    void setReloadData(bool state) {reloadData=state;}
    void setHighlightRow(int row) {highlightRow=row;}
    void printGridStick();

public slots:

    // Zoom slots
    void displayView();
    void zoomIn(bool shiftAfter=true);
    void zoomOut(bool shiftAfter=true);
    void viewAll();
    void verticalLineZoomIn();
    void verticalLineZoomOut();

    // updates timestamp line position
    void gotoTimestamp(int64_t t);

    // Dealing with the gview H scroll bar moving event
    void sliderMoved(int x);

    //Methods to manage gview traces depending on SignalTree events
    //void addTraceToLegend(std::string path, bool addMenuAction=true);
    //void deleteLegendItem(QLayoutItem* item, QLayoutItem* stretch);
    void handleSignalNodeCollapsed();
    void handleSignalNodeExpanded();
    void setHScrollbarParams();
    void handleSBarChange(int value);
    void handleSliderAction(int action);
    void highlightSignal (const QModelIndex &index);
    void createGridSticks();


signals:
    void zoomOccured();
    void dataIsReady();

protected:
    void paintEvent(QPaintEvent* event) override;
    void drawBackground(QPainter *painter, const QRect &exposed, int nlines);
    void highlightFunction(QPointF q);
    QString findEltOnRow( QModelIndex parent,
                          QModelIndex signalIdx,
                          int rowNb,
                          int* rowIdx);
    // Mouse events Management
    void wheelEvent(QWheelEvent*) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent* event);

    // Scrolling methods
    void scrollWithPixels(const QPoint &pixel);
    void scrollWithDegrees(const QPoint &step);

    // Utils
    uint64_t extractPower10Round(uint64_t x);


private:
    int paintNb=0;
    QWidget* parentWidget;

    js::config *json;
    Profiler_backend_user* backendUser;
    Profiler_view_results *requested_results=NULL; // Indicate the latest request sent to backend.
        // This is used to check if the results that we are receiving corresponds to the latest
        // request sent
    Profiler_view_results *pending_results=NULL; // Indicate the current results being displayed
    bool waiting_results = false;
      // abd used to know wheter the latest reply received corresponds to the latest
      // request sent
    // Store data received in updateMe from the backend
    //bool firstData=true; // true when first data received has not been processed yet
    bool dataReceived =false; // true when first data has been received
    int64_t start_ts=0; // first window point timestamp
    int64_t end_ts=0; // last window point timestamp
    int64_t max_ts=0; // max simulation timestamp
    int nbPixels=0; // length of the trace in pixels
    std::vector<std::vector<Profiler_backend_slot_info>> traces;
    std::vector<int> traceId; // stores the traceIds of the traces that have been received from the backend
    std::vector<int> traceWidth; // stores the trace widths of the traces

    // methods for the ruler & selection area on gview
    QLine m_line; // line drawn by mouse move event
    QRect m_rect; // rectangle drawn by mouse move event
    bool selectionRect= false; // true if a selection rect is displayed on gview
    bool movingLeftEdge = false; // true if user is currently adjusting left edge of select rect
    bool movingRightEdge = false; // true if user is currently adjusting right edge of select rect
    QPixmap m_nPTargetPixmap; // pixmap for displaying the time range selection area
    bool m_nbMousePressed; // nb of mouse pressed events accumulated
    bool drawRect = false; // true if a selection rectangle has to be painted
    bool drawLine = false; // true if a timestamp line has to be painted
    bool mouseMoving = false; // true if the user was currently moving the mouse
    bool leftClick=false; // true if user jusr left clicked with the mouse
    bool rightClick=false;// true if user jusr right clicked with the mouse
    QPointF functionPoint; // point last clicked on gview ..used to dearch corresponding function
    int sbarValue=0;

    std::vector<Stick> gridStick;
    // Analog Signals patterns constants. The Vertical size of a signal place is 19 pixels
    int highlightRow = -1; // row that needs to be highlighted
    bool firstPainting = true; // true if first gview painting didn't occur yet
    const int spMax = 16; // Analog Signal bottom line pixel nb
    const int spMin = 3;  // Analog Signal top line pixel nb
    const int sg1Min = 15; // Analog signal end/begin pattern first segment pixel min
    const int sg1Max = 16; // Analog signal end/begin pattern first segment
    const int sg2Min = 12; // Analog signal end/begin pattern 2nd segment
    const int sg2Max= 14;  // Analog signal end/begin pattern 2nd segment
    const int sg3Min = 8; // Analog Signal end/begin pattern 3rd segment
    const int sg3Max = 11; // Analog Signal end/begin pattern 3rd segment
    const int sg4Min = 5; // Analog Signal end/begin pattern 4th segment
    const int sg4Max = 7; // Analog Signal end/begin pattern 4thsegment
    const int sg5Min = 3; // Analog Signal end/begin pattern 5th segment
    const int sg5Max = 4; // Analog Signal end/begin pattern 5th segment

    void updateVerticalLine(QPointF q); // updates vertical timestamp line after H Scroll
    void updateVerticalLineText(); // updates timestamp text
    void updateTimeInterval(uint64_t position);
    //void resizeEvent(QResizeEvent *event) override; // manages resize event

    /** gives the time range currently displayed to the user */
    void getTimeWindow(uint64_t& t0, uint64_t& t1) const;
    /** gives the time range that corresponds to the pixels between x0 and x1 */
    void getTimeWindow(int x0, int x1, uint64_t& t0, uint64_t& t1) const;
    uint64_t timeWidth() const;

    void setMaxScrollTime();
    void shiftAfterZoom(double coeff);
    void setPageStep();
    // checks if a signal is in the viewPort of the signals Tree
    bool signalInViewPort(QModelIndex signalIdx);
    // Paints a signal to the gview at the right line between t0 & t1
    void paintSignalToGview(QPainter& painter, const QString signalPath,
                            //std::vector<TLData<const char*>> data,
                            std::vector<int> signalIdList,
                            int line,
                            uint64_t t0,
                            uint64_t t1,
                            int x1);

    // paints all signals visible in the signals tree, recursively
    void paintSubTree(QPainter& painter,
                      QModelIndex parent,
                      QModelIndex signalIdx,
                      //std::vector<TLData<const char*>> data,
                      std::vector<int> signalIdList,
                      int* line,
                      uint64_t t0,
                      uint64_t t1,
                      int x1);
    bool buildSignalsList( QModelIndex parent,
                           QModelIndex signalIdx,
                           QMap<QString,int> traceMap,
                           QMap<QString,int> sigWidthMap);
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
    void centerGviewOnVerticalLine();

    std::list<uint64_t> mouseTimestamps; /** list to store the different mousetimestamps during a wheelEvent **/

    std::mutex mouseMutex; /** mutex used to help manage the mouse centered zoom **/
    uint16_t wheelEventCount =0; /** nb of wheelEvents during one mouse scroll zoom commande **/
    std::thread th1;

    QColor getColor(std::string fname);
    int getCoreNb(QString sigShortName);
    int getCoreNb2(QString sigLongName);

    // Deletes all subwidgets of the TLGView
    void clearWidgets(QLayout * layout);


    /**
    draws a Data_with_time element
    this function is not trivial to implement because all supported types T must
    be handled correctly
  */
    /*template <typename T> int drawDwt(const Data_with_time<T>& dwt, int x1, int y,
                                    QPainter& painter, int signalId);
   */
    //QString getLegendModeText();

    ProLabel* insertLegendItem(int index, const char* str, bool addMenuAction);


    double zoomFactor = 1. / 10000000;   /**< number of pixels per time unit */
    uint64_t timeOffset;  /**< timestamp of the leftmost displayed pixel */
    uint64_t maxTime;     /**< timestamp of the last event */
    uint64_t maxScrollTime; /**< user cannot scroll beyond this timestamp */
    uint64_t verticalLineTime;  /**< abscissa of the red vertical line */
    uint64_t timeIntervalValue;// Value of the time interval displayed by the mouse move
    uint maxY;
    uint peHeight=19;  /**< height in pixels of the timeline for 1 core */
    uint nCore;
    Timeline* tl;
    FunctionDetails* fd;


    // Widgets we need to access from the main window
    QDockWidget* functionsDock;
    StallChart* stallchart;
    QScrollArea* sArea;

    SignalTree* signalsView;
    StatModel* statModel;
    QTableView* statTableView;


    QTabWidget* tabw;
    QMessageBox msgBox;

    //int functionsIdx;
    QScrollBar* sbar;
    QVBoxLayout* legendLayout;
    QPen pen, greenPen, redPen;
    QTimer timer;
    bool reloadData = false; // set to true when need of reloading Data in Paint
    QMutex* drawMutex;
    LegendMode currentMode=TIME_MODE;

    std::unordered_map<std::string, QColor> functionColors;
    int hueStep = 90;
    int currentHue = 0;

    friend class HLegendWidget;
    void getAllData(int pixelsNb);

    // Vector to store the signal values to be displayed
    std::vector<SigValData *> sigValues;
    std::vector<SigFuncData *> funcValues;

    // Makes the image for a digital signal
    QImage* makeDigSigImage(std::vector<Profiler_backend_slot_info> &trace_slot,
                            int nb_slots,
                            int pixelMax,
                            bool highlight);
    // Makes the image for a digital signal in display mode "logic-box"
    QImage* makeDigBoxSigImage(std::vector<Profiler_backend_slot_info> &trace_slot,
                               int nb_slots,
                               int pixelMax,
                               bool highlight);
    // Makes an image for an analog signal
    QImage* makeAnaSigImage(std::vector<Profiler_backend_slot_info> &trace_slot,
                            int traceWidth,
                            int nb_plots,
                            int pixelMax,
                            bool highlight);
    QImage* makeFuncSigImage(std::vector<Profiler_backend_slot_info> &trace_slot,
                                     int traceWidth,
                                     int nb_slots,
                                     int pixelMax,
                                     bool highlight);
    QImage* makeFuncBoxSigImage(std::vector<Profiler_backend_slot_info> &trace_slot,
                                     int traceWidth,
                                     int nb_slots,
                                     int pixelMax,
                                     bool highlight);
    void drawTimeStamps(QImage* image, int j);
    // Adds the signal values within the boxes for analog signals
    void drawVerticalGrid (QImage* image);
    // Adds the signal values within the boxes for analog signals
    void addSignalValues(QPainter& painter, QFont font, int lWidth, int i);
    // Adds the signal message within all the boxes of the signal
    void addSignalMessage(QPainter& painter, QFont font, QString message, int lWidth, int i);
    // Adds function names within all the boxes of the signal
    void addFunctionValues(QPainter& painter, QFont font, int lWidth, int i);
    // Vector to store the QImages created -- garbage collection
    std::vector<QImage*> images;
    void deleteImages();

    // Map to store the display of the signals.
    // The key of the Map corresponds to the path of the signal
    // its value stores the diplay structure of the signal
    QMap<QString,Display*> signalsDisplayMap;
    void createSignalsDisplayMap(js::config *json);
    void resizeEvent(QResizeEvent*);

private slots:
    /** checks whether new data has arrived from gvsoc. If yes, view is updated */
    //void autoReconstruct();
    void execMessage(const QString &text);
    void getData(uint64_t tMin,uint64_t tMax, int pixelsNb, bool all=false);


};


#endif // TLGVIEW_HPP
