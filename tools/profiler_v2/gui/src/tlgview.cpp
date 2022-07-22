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
#include <sstream>
#include <QPalette>
#include <QResizeEvent>

#include "math.h"

#include "timeline.hpp"

#ifndef _COLORS_
#define _COLORS_

/* FOREGROUND */
#define RST  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#define FRED(x) KRED x RST
#define FGRN(x) KGRN x RST
#define FYEL(x) KYEL x RST
#define FBLU(x) KBLU x RST
#define FMAG(x) KMAG x RST
#define FCYN(x) KCYN x RST
#define FWHT(x) KWHT x RST

#define BOLD(x) "\x1B[1m" x RST
#define UNDL(x) "\x1B[4m" x RST

#endif  /* _COLORS_ */

//#define PROFILE 1

//#include "backend_interface.hpp"

class Timeline;

Stick::Stick(int xr, int y1r, int y2r, QString textr, bool mainr){
    x=xr;
    y1=y1r;
    y2=y2r;
    main=mainr;
    text=textr;
}

Display::Display(QString displayType, QString message){
    this->displayType=displayType;
    this->message=message;
}


int SigValData::getSpace() {

    int space = (jEnd-jStart);

    return space;
}

int SigFuncData::getSpace() {

    int space = (jEnd-jStart);

    return space;
}

QString SigValData::getHexaStr(int sigWidth){

    //char buffer[25] = { 0 };
    //std::snprintf(buffer, 25, "%A", value);
    //return QString::fromStdString(buffer);
    std::stringstream sstream;
    sstream << std::hex << int(value);
    std::string result = sstream.str();
    return QString::fromStdString(result);

}


TLGView::TLGView(Timeline* tl,
                 QScrollBar* sbar,
                 QVBoxLayout* legendLayout,
                 SignalTree* signalsView,
                 Profiler_backend_user* backendUserRef,
                 js::config *json):
    signalsView(signalsView),
    json(json)

{
    qDebug() << "[-] Create gview" ;
    this->tl = tl;
    this->sbar =sbar;
    this->legendLayout = legendLayout;
    this->backendUser = backendUserRef;
    currentMode = TIME_MODE;
    pen = QPen(Qt::black);
    greenPen = QPen(Qt::darkGreen);
    redPen = QPen(Qt::red);
    pen.setWidthF(0);
    greenPen.setWidthF(0);
    redPen.setWidthF(0);
    zoomFactor = 1. / 10000000;
    timeOffset = 0;
    maxTime = 1;
    maxScrollTime = 1;
    verticalLineTime = 0;
    timeIntervalValue = 0;
    m_nbMousePressed = false;
    sigValues.clear();
    signalsDisplayMap.clear();
    drawMutex= new QMutex(QMutex::Recursive);
    this->setAutoFillBackground(true);
    this->setPalette(QPalette(Qt::black));

    createSignalsDisplayMap(json->get("**/views/timeline/signals"));
    connect(this,SIGNAL(dataIsReady()),this, SLOT(displayView()),Qt::QueuedConnection );
    connect(&timer, SIGNAL(timeout()), this, SLOT(update()), Qt::QueuedConnection);
    timer.start(2000000);
    drawMutex->lock();
    getData(0,-1,this->width());
    drawMutex->unlock();
    qDebug() << "End create Gview: " ;
}

void TLGView::displayView() {
    update();
    show();
}

void TLGView::resizeEvent(QResizeEvent* event){
    QWidget::resizeEvent(event);
    drawMutex->lock();
    int width = event->size().width();
    uint64_t t0, t1;
    // calculate corresponding Time Range values according to current zoomFactor
    getTimeWindow(0, width, t0, t1);
    getData(t0,t1,width);
    drawMutex->unlock();
}

bool TLGView::signalInViewPort(QModelIndex idx) {
    // checks if a signal is in the viewPort of the signals Tree
    if (signalsView->visualRect(idx).isValid()) {
        if (signalsView->visualRect(idx).intersects(signalsView->viewport()->rect()))
            return true;
    } else
        std::cout << "Rect of Signal is not valid " << std::endl;
    return false;
}

bool TLGView::buildSignalsList( QModelIndex parent,
                                QModelIndex signalIdx,
                                QMap<QString,int> traceMap,
                                QMap<QString,int> sigWidthMap) {

    // This procedure builds a Map of the signals we need received from the backend.
    // We need to check that the group to which the signal belongs is selected, unless the signal
    // doesn't have to figure in the list
    // It also builds a map of the width of the signals.
    //std::cout << "[-] Build Signals List " << std::endl;

    int i = 0;
    QModelIndex idx;
    QModelIndex siblingIdx;
    bool success=true;

    if (parent != signalsView->rootIndex()){
        if (signalInViewPort(parent)) {
        // check if this signal has already been received from the backend (is in the list)
            if (traceMap.contains(signalIdx.data().toString())) {
                // We now need to know if the signal's row is visible in the Signals Tree Viewport
                traceId.push_back(traceMap[signalIdx.data().toString()]);
                if (sigWidthMap.contains(signalIdx.data().toString()))
                    traceWidth.push_back(sigWidthMap[signalIdx.data().toString()]);
                else
                    std::cout << "[-] Error: width not found for trace "  << signalIdx.data().toString().toStdString() << std::endl;

            }
            else if (QString::compare(signalIdx.data().toString(), "null") !=0) {
                // if the signal name is not "null"
                //std::cout << "[-] Warning - Signal not found: " << signalIdx.data().toString().toStdString() << std::endl;
                traceId.push_back(-1);
                traceWidth.push_back(1);
            } else {
                // set the trace ID to -1 in case signal name == null
                //std::cout << "[-] Warning - Signal name =null " << signalIdx.data().toString().toStdString() << std::endl;
                traceId.push_back(-1);
                traceWidth.push_back(1);
            }
        }
    }

    //std::cout << "row count : "<<  signalsModel->rowCount(parent) << std::endl;

    if ((parent == signalsView->rootIndex()) || signalsView->isExpanded(parent)) {
        for (i=0; i< signalsView->model->rowCount(parent) ; i++) {
            idx = signalsView->model->index(i,0,parent);
            siblingIdx = signalsView->model->index(i,1,parent);
            success = success && buildSignalsList(idx,siblingIdx,traceMap,sigWidthMap);
        }
    }
    return success;
}

void TLGView::getData(uint64_t tMin,uint64_t tMax, int pixelsNb, bool all ) {

    //std::cout << "getData" << std::endl;
    // We compare the list of signals received, stored in the Map
    // We check  the  ones we need in the SignalsTreeView .
    // If one signal is missing, we wait....
    // We must loop on the signalsTreeView

    // First , clear the list of Ids
    traceId.clear();
    traceWidth.clear();
    // Build the list of signals Ids we need to get from backend
    bool success= buildSignalsList(signalsView->rootIndex(),
                                   signalsView->rootIndex(),
                                   backendUser->traceMap,
                                   backendUser->sigWidthMap);
    
    if (!success){
        std::cout << "[-] Error: GetData aborted - Probably because some Signals not delivered by backend " << std::endl;
        return;
    }


    Profiler_view_results *results;

    if ((all) || (tMin > tMax))
    {
        tMin = 0;
        tMax = -1;
    }

    results = new Profiler_view_results(this, traceId, tMin, tMax, pixelsNb);

    this->requested_results = results;
    this->waiting_results = true;

    backendUser->backend->get_view(results);
}

void TLGView::updateMe(Profiler_view_results *results) {

    qDebug() << "------ updateMe";

    drawMutex->lock();

    // Due to multi-threading, we may received results which do not correspond to our latest
    // request, in this case, just drop it
    if (requested_results != results)
    {
        delete results;
        drawMutex->unlock();
        return;
    }

    this->waiting_results = false;

    // If some results are still there, we know now we won't need them anymore
    if (this->pending_results)
    {
        delete this->pending_results;
    }

    // Store the results so they can be used to handle the display until other results are received.
    pending_results = results;

    start_ts=results->start_timestamp;
    end_ts=results->end_timestamp;
    maxTime=std::max(results->end_timestamp,results->max_timestamp);
    max_ts=results->max_timestamp;
    nbPixels = results->nb_slots;
    traces=results->trace_slots; // stores a reference to the trace_slots vector
    // need to check the traces here
    if (!dataReceived)
        dataReceived=true;
    // Call update & show to repaint the TimeLine window.
    //std::cout << "  updateMe before emit " << std::endl;
    drawMutex->unlock();
    emit dataIsReady();
    qDebug() << " END updateMe";
}

// pushes the x coordinate of a hLegend main tic
void TLGView::pushGridStick(Stick sti){
    gridStick.push_back(sti);
}
// Clears the grid tics stored
void TLGView::clearGridSticks(){
    gridStick.clear();
}

void TLGView::highlightSignal(const QModelIndex &index) {
    //At this point, user has clicked in the signals Tree View on a signal
    // and we get the QModelIndex of the signal
    // Get the sibling of this index in column 1 and then the value containing the path of
    // the signal
    drawMutex->lock();
    QModelIndex sibling = index.sibling(index.row(),1);
    // Now, get the path of the signal
    QString signalPath = sibling.data().toString();
    // Now, get the Id of the signal from the traceMap
    int signalId = backendUser->traceMap[signalPath];

    //Now get the index of the signal in the traceId list
    std::pair<bool, int > result;
    // Find given element in vector
    auto it = std::find(traceId.begin(), traceId.end(), signalId);
    if (it != traceId.end())
    {
        result.second = distance(traceId.begin(), it);
        result.first = true;
    }
    else
    {
        result.first = false;
        result.second = -1;
    }

    if (highlightRow == result.second)
        // De-select the highlighted signal
        highlightRow = -1;
    else
        // store the row of the signal to highlight
        highlightRow = result.second;

    // Now force a repaint event
    getData(start_ts,end_ts,nbPixels);
    drawMutex->unlock();
}

void TLGView::deleteImages() {
    for(unsigned long i = 0; i < images.size(); i++)
    {
        delete images[i];
    }
    images.clear();
}

void TLGView::drawVerticalGrid (QImage* image){
    for(auto it = std::begin(gridStick); it != std::end(gridStick); ++it) {
        if(it->main)
            for (int z=0;z<signalHeight;z++)
                image->setPixel(it->x,z, qRgb(95,91,218));
    }
}

QImage* TLGView::makeDigSigImage(std::vector<Profiler_backend_slot_info> &trace_slot,
                                 int nb_slots,
                                 int pixelMax,
                                 bool highlight=false) {
    // This procedure transforms the trace vector in a QImage nb_slots x signalHeight
    // For a  "1" value, we fill top column pixel
    // For a  "0" values, we fill bottom column pixel
    // For a "-1" value, we fill all the column
    // with green color
    // Image background is white
    qDebug() << " makeDigSigImage";
    QRgb signalColor;
    QRgb backgroundColor;

    // set colors depending on the highlight flag

    if (highlight) {
        signalColor = QColor(Qt::darkGreen).rgb();
        backgroundColor = QColor(Qt::gray).rgb();
    } else {
        signalColor = qRgb(43,193,20);
        backgroundColor = QColor(Qt::black).rgb();
    }

    //std::cout << "makeImage" << std::endl;
    QImage* sigImage= new QImage(nb_slots, signalHeight, QImage::Format_ARGB32_Premultiplied);
    images.push_back(sigImage);
    sigImage->fill(backgroundColor);
    if (pixelMax + 1 > nb_slots) {
        std::cout << "[-] Error: pixelMax out of range:  " << pixelMax << " Should be < " << nb_slots -1 << std::endl;
        return sigImage;
    }

    //for (int j = 0; j < trace_slot.size(); j++)
    for (int j = 0; j <= pixelMax ; j++)
    {
        // draw grid horizontal blue line on line 0
        sigImage->setPixel(j,18, qRgb(95,91,218));

        // The following is only for signals of size 1
        if (trace_slot[j].value == 1.0) {
            sigImage->setPixel(j,spMin, signalColor);
        }
        else if (trace_slot[j].value == 0.0) {
            sigImage->setPixel(j,spMax, signalColor);
        }
        else if (trace_slot[j].value == -1.0)
            for (int z=spMin;z<=spMax; z++)
                sigImage->setPixel(j,z,signalColor);

        drawTimeStamps(sigImage,j);
    }
    drawVerticalGrid(sigImage);

    return sigImage;
}

void TLGView::createSignalsDisplayMap(js::config *json){

    // Creates a Map for the display of the signals from the Json file.
    // The key is the path of the signals. The value is a structure representing the
    // display type and the message.

    qDebug() << "CreateSignalsDisplayMap json: "  << json;
    for (js::config *child_json: json->get_elems())
    {
        QString child_name = QString::fromStdString(child_json->get("name")->get_str());
        //qDebug() << "child_name" << child_name;
        js::config *path_json = child_json->get("path");
        //if (path_json!=NULL)
            //qDebug() << "Path " << QString::fromStdString(child_json->get("path")->get_str());
        js::config *display_json = child_json->get("display");
        if (display_json != NULL) {
            js::config *display_type = display_json->get("type");
            js::config *display_message = display_json->get("message");
            QString message;
            if (display_message == NULL)
                message.append(QString(" "));
            else
                message.append(QString::fromStdString(display_message->get_str()));
            if (display_type!= NULL)
            {
                Display* childDisplay = new Display(QString::fromStdString(display_type->get_str()),
                                                    message);
                if (path_json!=NULL) {
                    qDebug() << "Path " << QString::fromStdString(child_json->get("path")->get_str());
                    qDebug() << "display_type " << QString::fromStdString(display_type->get_str());
                    qDebug() << "display_message" << message;
                    signalsDisplayMap[QString::fromStdString(path_json->get_str())] = childDisplay;
                }
            }
        }

        js::config *child_signals = child_json->get("signals");

        if (child_signals != NULL)
        {
            this->createSignalsDisplayMap(child_signals);
        }
    }
    qDebug() << "END CreateSignalsDisplayMap";
}


QImage* TLGView::makeDigBoxSigImage(std::vector<Profiler_backend_slot_info> &trace_slot,
                                    int nb_slots,
                                    int pixelMax,
                                    bool highlight=false) {
    // This procedure transforms the trace vector in a QImage nb_slots x signalHeight
    // For a  value !=0 , we fill top & bottom column pixels
    // Except for the first 14 comums of the signal, fo rwhich we fill in ascending & descending
    // diagonals, starting from the center pixels if signal length >= 54 pixels
    // we calculate the interval between 2 points of the diagonal
    // in the case sigLength <=
    // For a "0" value, we fill 2 pixels in the middle of the comumn (pixels 14&15)
    // with green color
    // Image background is white
    qDebug() << "MakeDigBoxSigImage" ;

    QRgb signalColor;
    QRgb backgroundColor;
    QRgb zeroColor;
    QRgb boxColor;
    QRgb patternColor; // color of the begin/end pattern of an analog signal element
    SigValData * sigValue; // to sotre a signal value and where to print it
    bool eltStarted = false; // true if an element has started
    // First clear sigValues vector
    for(unsigned long i = 0; i < sigValues.size(); i++)
    {
        delete sigValues[i];
    }
    sigValues.clear();


    // set colors depending on the highlight flag

    if (highlight) {
        signalColor = QColor(Qt::darkGreen).rgb();
        backgroundColor = QColor(Qt::gray).rgb();
    } else {
        signalColor = qRgb(43,193,20);
        backgroundColor = QColor(Qt::black).rgb();
        patternColor= QColor(Qt::darkGreen).rgb();
    }
    zeroColor= qRgb(245,245,162);
    boxColor= qRgb(155,186,178);

    QImage* sigImage= new QImage(nb_slots, signalHeight, QImage::Format_ARGB32_Premultiplied);
    images.push_back(sigImage);
    sigImage->fill(backgroundColor);
    if (pixelMax + 1 > nb_slots) {
        std::cout << "[-] Error: pixelMax out of range:  " << pixelMax << " Should be < " << nb_slots -1 << std::endl;
        return sigImage;
    }


    int previousValue = 0;
    int borderDelay=0;
    for (int j = 0; j < pixelMax + 1; j++)
    {
        // draw grid horizontal blue line on 18
        sigImage->setPixel(j,18, qRgb(95,91,218));
        if (trace_slot[j].value >0) {
            if (borderDelay==0) {
                sigImage->setPixel(j,spMax , signalColor);
                sigImage->setPixel(j,spMin , signalColor);
                for (int z=spMin+1;z<spMax; z++)
                    sigImage->setPixel(j,z,boxColor);
            } else
                borderDelay--;

            previousValue = trace_slot[j].value;
        }else if (trace_slot[j].value == -1.0) {
            // Draw transition pattern: 2 pixels in the middle
            for (int z=sg3Min;z<=sg3Max; z++)
                sigImage->setPixel(j,z,signalColor);
            borderDelay=0;
            bool patternDrawn = false;
            if (previousValue>0) {
                patternDrawn=true;
                // Design end Signal Pattern
                // 2nd and 4th segments
                if (j-1 > 0) {
                    for (int z=sg2Min;z<=sg2Max; z++)
                        sigImage->setPixel(j-1,z,signalColor);
                    for (int z=sg4Min;z<=sg4Max; z++)
                        sigImage->setPixel(j-1,z,signalColor);
                    // draw box inner pixels in boxColor
                    for (int z=sg4Max+1;z<sg2Min; z++)
                        sigImage->setPixel(j-1,z,boxColor);
                    if (j-2<0) {
                        // Superpose 1st and 5th segments
                        for (int z=sg1Min;z<=sg1Max; z++)
                            sigImage->setPixel(j-1,z,signalColor);
                        for (int z=sg5Min;z<=sg5Max; z++)
                            sigImage->setPixel(j-1,z,signalColor);
                    }
                    // set border pixels back to black
                    sigImage->setPixel(j-1,spMax , backgroundColor);
                    sigImage->setPixel(j-1,spMin , backgroundColor);
                    sigImage->setPixel(j-1,spMax -1 , backgroundColor);
                    sigImage->setPixel(j-1,spMin +1, backgroundColor);
                }
                // 1st and 5th segments
                if (j-2>0){
                    for (int z=sg1Min;z<=sg1Max; z++)
                        sigImage->setPixel(j-2,z,signalColor);
                    for (int z=sg5Min;z<=sg5Max; z++)
                        sigImage->setPixel(j-2,z,signalColor);
                    // draw box inner pixels in boxColor
                    for (int z=sg5Max+1;z<sg1Min; z++)
                        sigImage->setPixel(j-2,z,boxColor);
                }
                // This is the end of an element -- finish to store it
                if ((sigValues.size() >0) && eltStarted) {
                    sigValues.back()->jEnd=j-2;
                }
                eltStarted = false;
            }

            if ((j+1<=pixelMax) && (trace_slot[j+1].value >0)) {
                patternDrawn=true;
                // store the beginning of the element
                if (((previousValue == 0) || (previousValue == -1)) && !eltStarted) {
                    // create a new sigValue
                    sigValue = new SigValData();
                    sigValue->jStart=j+2;
                    sigValue->jEnd = pixelMax;
                    sigValue->value=trace_slot[j].value;
                    sigValues.push_back(sigValue);
                }
                // Next value >0 ==> Design Begin Signal Pattern
                // 2nd and 4th segments
                if (j+1<=pixelMax){
                    for (int z=sg2Min;z<=sg2Max; z++)
                        sigImage->setPixel(j+1,z,signalColor);
                    for (int z=sg4Min;z<=sg4Max;z++)
                        sigImage->setPixel(j+1,z,signalColor);
                    // draw box inner pixels in boxColor
                    for (int z=sg4Max+1;z<sg2Min; z++)
                        sigImage->setPixel(j+1,z,boxColor);
                    if (j+2>pixelMax) {
                        // Superpose 1st and 5th segments
                        for (int z=sg1Min;z<=sg1Max; z++)
                            sigImage->setPixel(j+1,z,signalColor);
                        for (int z=sg5Min;z<=sg5Max; z++)
                            sigImage->setPixel(j+1,z,signalColor);
                    }
                    borderDelay++;
                }
                // 1st and 5th segments
                if (j+2<=pixelMax){
                    for (int z=sg1Min;z<=sg1Max; z++)
                        sigImage->setPixel(j+2,z,signalColor);
                    for (int z=sg5Min;z<=sg5Max; z++)
                        sigImage->setPixel(j+2,z,signalColor);
                    // draw box inner pixels in boxColor
                    for (int z=sg5Max+1;z<sg1Min; z++)
                        sigImage->setPixel(j+2,z,boxColor);
                    borderDelay++;
                }
                eltStarted=true;
            }
            if (!patternDrawn){ // Just draw a bar
                for (int z=spMin;z<=spMax; z++)
                    sigImage->setPixel(j,z,signalColor);
                patternDrawn=false;
                eltStarted=false;
            }
            previousValue=trace_slot[j].value;

        } else { // Zero value
            // Just draw a two pixels in the middle (pixels 9 & 10)
            sigImage->setPixel(j,9,zeroColor);
            sigImage->setPixel(j,10,zeroColor);
            previousValue=0;
        }

        drawTimeStamps(sigImage, j);

    }
    drawVerticalGrid(sigImage);
    //std::cout << " @@@@@@@ END MakeDigBoxSigImage"<<  std::endl;
    drawVerticalGrid(sigImage);
    return sigImage;
}

void TLGView::drawTimeStamps(QImage* image, int j){
    // This function draws the timestamps & time interval graphic
    // representations

    int redLineX=-1; // coordinate of the red line for user timestamp . If -1, no need to draw any
    int areaMinX;
    int areaMaxX;

    // Preparing Data
    if (drawLine){
        // we need to find which pixel nb corresponds to the vertica line
        // and draw it.
        redLineX = time2position(verticalLineTime);
    }
    else
        redLineX=-1;

    if(drawRect)
    {
        // update selected area rectangle and draw it
        if (!mouseMoving) {
            m_rect.setBottomLeft(QPoint(time2position(t0Select),0));
            m_rect.setWidth(time2position(t1Select) - time2position(t0Select));
        }
        m_rect.setHeight(this->height());
        areaMinX = m_rect.left();
        areaMaxX = areaMinX + m_rect.width();
    }

    // draw timestamp red line if needed
    if (redLineX >=0)
        if(j==redLineX)
            for (int z=0;z<signalHeight; z++)
                image->setPixel(j,z,qRgb(255,0,0));


    // draw time interval vertical  red interval if needed
    if  (drawRect) {
        if (( j==areaMinX ) || (j==areaMaxX))
            for (int z=0;z<signalHeight; z++)
                image->setPixel(j,z,qRgb(255,0,0));
        int num = j % 4;
        if ((j>areaMinX) && (j<areaMaxX) && (num==0)) {
            for (int z=0;z<signalHeight; z++){
                int num1 = z % 4;
                if (num1==0)
                    image->setPixel(j,z,qRgb(252,197,197));
            }
        }
    }

}

QImage* TLGView::makeAnaSigImage(std::vector<Profiler_backend_slot_info> &trace_slot,
                                 int traceWidth,
                                 int nb_slots,
                                 int pixelMax,
                                 bool highlight=false) {
    // This procedure transforms the trace vector in a QImage nb_slots x signalHeight
    // For a  value !=0 , we fill top & bottom column pixels
    // Except for the first 14 comums of the signal, fo rwhich we fill in ascending & descending
    // diagonals, starting from the center pixels if signal length >= 54 pixels
    // we calculate the interval between 2 points of the diagonal
    // in the case sigLength <=
    // For a "-1" value, we fill 2 pixels in the middle of the comumn (pixels 14&15)
    // with green color
    // Image background is white
    qDebug() << "MakeAnaSigImage" ;

    QRgb signalColor;
    QRgb backgroundColor;
    QRgb patternColor; // color of the begin/end pattern of an analog signal element
    SigValData * sigValue; // to sotre a signal value and where to print it
    bool eltStarted = false; // true if an element has started
    // First clear sigValues vector
    for(unsigned long i = 0; i < sigValues.size(); i++)
    {
        delete sigValues[i];
    }
    sigValues.clear();


    // set colors depending on the highlight flag

    if (highlight) {
        signalColor = QColor(Qt::darkGreen).rgb();
        backgroundColor = QColor(Qt::gray).rgb();
    } else {
        signalColor = qRgb(43,193,20);
        backgroundColor = QColor(Qt::black).rgb();
        patternColor= QColor(Qt::darkGreen).rgb();
    }

    //std::cout << "makeImage" << std::endl;
    QImage* sigImage= new QImage(nb_slots, signalHeight,QImage::Format_ARGB32_Premultiplied);
    images.push_back(sigImage);
    sigImage->fill(backgroundColor);
    if (pixelMax + 1 > nb_slots) {
        std::cout << "[-] Error: pixelMax out of range:  " << pixelMax << " Should be < " << nb_slots -1 << std::endl;
        return sigImage;
    }



    int previousValue = 0;
    int dontDrawNextBorderPixel=false;
    for (int j = 0; j < pixelMax + 1; j++)
    {
        // draw grid horizontal blue line on 18
        sigImage->setPixel(j,18, qRgb(95,91,218));

        if (trace_slot[j].value >=0) {
            if (!dontDrawNextBorderPixel) {
                sigImage->setPixel(j,spMax , signalColor);
                sigImage->setPixel(j,spMin , signalColor);
            }
            else
                dontDrawNextBorderPixel = false;


            // store the beginning of the element
            if (((previousValue == 0) || (previousValue == -1)) && !eltStarted) {
                // create a new sigValue
                sigValue = new SigValData();
                sigValue->jStart=j+2;
                sigValue->jEnd = pixelMax;
                sigValue->value=trace_slot[j].value;
                sigValues.push_back(sigValue);
                eltStarted = true;
            }
            previousValue = trace_slot[j].value;

        } else if ((trace_slot[j].value == -1.0) && (previousValue!=-1)) {
            // Design end Signal Pattern
            // Third middle segment
            for (int z=sg3Min;z<=sg3Max; z++)
                sigImage->setPixel(j,z,signalColor);

            // 2nd and 4th segments
            if (j-1 >= 0) {
                for (int z=sg2Min;z<=sg2Max; z++)
                    sigImage->setPixel(j-1,z,signalColor);
                for (int z=sg4Min;z<=sg4Max; z++)
                    sigImage->setPixel(j-1,z,signalColor);
                if (j-2<0) {
                    // Superpose 1st and 5th segments
                    for (int z=sg1Min;z<=sg1Max; z++)
                        sigImage->setPixel(j-1,z,signalColor);
                    for (int z=sg5Min;z<=sg5Max; z++)
                        sigImage->setPixel(j-1,z,signalColor);
                }
                // set border pixels back to black
                sigImage->setPixel(j-1,spMax , backgroundColor);
                sigImage->setPixel(j-1,spMin , backgroundColor);
            }
            // 1st and 5th segments
            if (j-2>=0){
                for (int z=sg1Min;z<=sg1Max; z++)
                    sigImage->setPixel(j-2,z,signalColor);
                for (int z=sg5Min;z<=sg5Max; z++)
                    sigImage->setPixel(j-2,z,signalColor);
            }

            // Design Begin Signal Pattern
            // 2nd and 4th segments
            if (j+1<=pixelMax){
                for (int z=sg2Min;z<=sg2Max; z++)
                    sigImage->setPixel(j+1,z,signalColor);
                for (int z=sg4Min;z<=sg4Max;z++)
                    sigImage->setPixel(j+1,z,signalColor);
                if (j+2>pixelMax) {
                    // Superpose 1st and 5th segments
                    for (int z=sg1Min;z<=sg1Max; z++)
                        sigImage->setPixel(j+1,z,signalColor);
                    for (int z=sg5Min;z<=sg5Max; z++)
                        sigImage->setPixel(j+1,z,signalColor);
                }
                dontDrawNextBorderPixel=true;
            }
            // 1st and 5th segments
            if (j+2<=pixelMax){
                for (int z=sg1Min;z<=sg1Max; z++)
                    sigImage->setPixel(j+2,z,signalColor);
                for (int z=sg5Min;z<=sg5Max; z++)
                    sigImage->setPixel(j+2,z,signalColor);
            }
            // This is the end of an element -- finish to store it
            if ((sigValues.size() >0) && eltStarted) {
                sigValues.back()->jEnd=j-2;
                eltStarted = false;
            }
            previousValue=-1;

        } else if ((trace_slot[j].value == -1.0) && (previousValue==-1)) {
            // Set all the column pixels
            for (int z=spMin;z<=spMax; z++)
                sigImage->setPixel(j,z,signalColor);
            previousValue=-1;
        }

        drawTimeStamps(sigImage, j);
    }
    drawVerticalGrid(sigImage);
    return sigImage;
}



QImage* TLGView::makeFuncSigImage(std::vector<Profiler_backend_slot_info> &trace_slot,
                                 int traceWidth,
                                 int nb_slots,
                                 int pixelMax,
                                 bool highlight=false) {
    // This procedure transforms the trace vector in a QImage nb_slots x signalHeight
    // For a  value !=0 , we fill top & bottom column pixels
    // Except for the first 14 comums of the signal, fo rwhich we fill in ascending & descending
    // diagonals, starting from the center pixels if signal length >= 54 pixels
    // we calculate the interval between 2 points of the diagonal
    // in the case sigLength <=
    // For a "-1" value, we fill 2 pixels in the middle of the comumn (pixels 14&15)
    // with green color
    // Image background is white
    qDebug() << "MakeFuncSigImage" ;

    QRgb zeroColor;
    QRgb boxColor;
    QRgb signalColor;
    QRgb backgroundColor;
    QRgb patternColor; // color of the begin/end pattern of an analog signal element
    SigFuncData * sigValue; // to sotre a function name and where to print it
    bool eltStarted = false; // true if an element has started
    // First clear funcValues vector
    for(unsigned long i = 0; i < funcValues.size(); i++)
    {
        delete funcValues[i];
    }
    funcValues.clear();

    // set colors depending on the highlight flag

    if (highlight) {
        signalColor = QColor(Qt::darkGreen).rgb();
        backgroundColor = QColor(Qt::gray).rgb();
    } else {
        signalColor = qRgb(43,193,20);
        backgroundColor = QColor(Qt::black).rgb();
        patternColor= QColor(Qt::darkGreen).rgb();
    }
    zeroColor= qRgb(245,245,162);
    boxColor= qRgb(155,186,178);

    //std::cout << "makeImage" << std::endl;
    QImage* sigImage= new QImage(nb_slots, signalHeight,QImage::Format_ARGB32_Premultiplied);
    images.push_back(sigImage);
    sigImage->fill(backgroundColor);
    if (pixelMax + 1 > nb_slots) {
        std::cout << "[-] Error: pixelMax out of range:  " << pixelMax << " Should be < " << nb_slots -1 << std::endl;
        return sigImage;
    }


    void *previousValue = 0;
    // This delay is used to tell the signal drawer not to
    // paint the signal border as a begin signal pattern has just been painted
    int borderDelay=0;

    for (int j = 0; j < pixelMax + 1; j++)
    {
        // draw grid horizontal blue line on 18
        sigImage->setPixel(j,18, qRgb(95,91,218));
        if ((trace_slot[j].value_p != NULL) ) {
            if (trace_slot[j].value_p !=(char*)1){
                if (borderDelay==0) {
                    //std::cout << "Design Middle Box Signal Pattern" << std::endl;
                    // Draw border of the signal box
                    sigImage->setPixel(j,spMax , signalColor);
                    sigImage->setPixel(j,spMin , signalColor);
                }
                else
                    borderDelay--;
            }
            else { // Zero value
                // Just draw a two pixels in the middle (pixels 9 & 10)
                //std::cout << "Design Zero Signal Pattern" << std::endl;
                sigImage->setPixel(j,9,zeroColor);
                sigImage->setPixel(j,10,zeroColor);
            }
            previousValue = trace_slot[j].value_p;
        } else {
            // Transition detected : draw End and then Begin Pattern if enough place
            // Draw transition pattern: 2 pixels in the middle
            for (int z=sg3Min;z<=sg3Max; z++)
                sigImage->setPixel(j,z,signalColor);
            borderDelay=0;
            bool patternDrawn = false;
            if ((previousValue != NULL) && (previousValue !=(char*)1)) {
                // Design end Signal Pattern
                patternDrawn=true;
                // 2nd and 4th segments
                if (j-1 > 0) {
                    for (int z=sg2Min;z<=sg2Max; z++)
                        sigImage->setPixel(j-1,z,signalColor);
                    for (int z=sg4Min;z<=sg4Max; z++)
                        sigImage->setPixel(j-1,z,signalColor);
                    if (j-2<0) {
                        // Superpose 1st and 5th segments
                        for (int z=sg1Min;z<=sg1Max; z++)
                            sigImage->setPixel(j-1,z,signalColor);
                        for (int z=sg5Min;z<=sg5Max; z++)
                            sigImage->setPixel(j-1,z,signalColor);
                    }
                    // set border pixels back to black
                    sigImage->setPixel(j-1,spMax , backgroundColor);
                    sigImage->setPixel(j-1,spMin , backgroundColor);
                    sigImage->setPixel(j-1,spMax -1 , backgroundColor);
                    sigImage->setPixel(j-1,spMin +1, backgroundColor);
                }
                // 1st and 5th segments
                if (j-2>0){
                    for (int z=sg1Min;z<=sg1Max; z++)
                        sigImage->setPixel(j-2,z,signalColor);
                    for (int z=sg5Min;z<=sg5Max; z++)
                        sigImage->setPixel(j-2,z,signalColor);
                }
                // This is the end of an element -- finish to store it
                if ((funcValues.size() >0) && eltStarted) {
                    funcValues.back()->jEnd=j-2;
                }

                eltStarted = false;
            }
            if ((j+1<=pixelMax) && (((trace_slot[j+1].value_p != NULL) && (trace_slot[j+1].value_p !=(char*)1)))) {
                patternDrawn=true;
                // store the beginning of the element
                if (!eltStarted) {
                    // create a new sigValue for storing function names
                    sigValue = new SigFuncData();
                    sigValue->jStart=j+3;
                    sigValue->jEnd = pixelMax;
                    sigValue->value=QString((char *) (trace_slot[j+1].value_p));
                    funcValues.push_back(sigValue);
                }
                eltStarted = true;
                // Next value >0 ==> Design Begin Signal Pattern
                // 2nd and 4th segments
                if (j+1<=pixelMax){
                    for (int z=sg2Min;z<=sg2Max; z++)
                        sigImage->setPixel(j+1,z,signalColor);
                    for (int z=sg4Min;z<=sg4Max;z++)
                        sigImage->setPixel(j+1,z,signalColor);
                    if (j+2>pixelMax) {
                        // Superpose 1st and 5th segments
                        for (int z=sg1Min;z<=sg1Max; z++)
                            sigImage->setPixel(j+1,z,signalColor);
                        for (int z=sg5Min;z<=sg5Max; z++)
                            sigImage->setPixel(j+1,z,signalColor);
                    }
                    borderDelay++;
                }
                // 1st and 5th segments
                if (j+2<=pixelMax){
                    for (int z=sg1Min;z<=sg1Max; z++)
                        sigImage->setPixel(j+2,z,signalColor);
                    for (int z=sg5Min;z<=sg5Max; z++)
                        sigImage->setPixel(j+2,z,signalColor);
                    borderDelay++;
                }
                eltStarted=true;
            }

            if (!patternDrawn){ // Just draw a bar
                for (int z=spMin;z<=spMax; z++)
                    sigImage->setPixel(j,z,signalColor);
                patternDrawn=false;
                eltStarted=false;
            }
            previousValue=trace_slot[j].value_p;

        }

        drawTimeStamps(sigImage, j);
    }
    drawVerticalGrid(sigImage);
    return sigImage;
}

QImage* TLGView::makeFuncBoxSigImage(std::vector<Profiler_backend_slot_info> &trace_slot,
                                 int traceWidth,
                                 int nb_slots,
                                 int pixelMax,
                                 bool highlight=false) {
    // This procedure transforms the trace vector in a QImage nb_slots x signalHeight
    // For a  value !=0 , we fill top & bottom column pixels
    // Except for the first 14 comums of the signal, fo rwhich we fill in ascending & descending
    // diagonals, starting from the center pixels if signal length >= 54 pixels
    // we calculate the interval between 2 points of the diagonal
    // in the case sigLength <=
    // For a "-1" value, we fill 2 pixels in the middle of the comumn (pixels 14&15)
    // with green color
    // Image background is white
    qDebug() << "MakeFuncSigImage" ;

    QRgb zeroColor;
    QRgb boxColor;
    QRgb signalColor;
    QRgb backgroundColor;
    QRgb patternColor; // color of the begin/end pattern of an analog signal element
    SigFuncData * sigValue; // to sotre a function name and where to print it
    bool eltStarted = false; // true if an element has started
    // First clear funcValues vector
    for(unsigned long i = 0; i < funcValues.size(); i++)
    {
        delete funcValues[i];
    }
    funcValues.clear();

    // set colors depending on the highlight flag

    if (highlight) {
        signalColor = QColor(Qt::darkGreen).rgb();
        backgroundColor = QColor(Qt::gray).rgb();
    } else {
        signalColor = qRgb(43,193,20);
        backgroundColor = QColor(Qt::black).rgb();
        patternColor= QColor(Qt::darkGreen).rgb();
    }
    zeroColor= qRgb(245,245,162);
    boxColor= qRgb(155,186,178);

    //std::cout << "makeImage" << std::endl;
    QImage* sigImage= new QImage(nb_slots, signalHeight,QImage::Format_ARGB32_Premultiplied);
    images.push_back(sigImage);
    sigImage->fill(backgroundColor);
    if (pixelMax + 1 > nb_slots) {
        std::cout << "[-] Error: pixelMax out of range:  " << pixelMax << " Should be < " << nb_slots -1 << std::endl;
        return sigImage;
    }

    void *previousValue = 0;
    // This delay is used to tell the signal drawer not to
    // paint the signal border as a begin signal pattern has just been painted
    int borderDelay=0;

    for (int j = 0; j < pixelMax + 1; j++)
    {
        // draw grid horizontal blue line on 18
        sigImage->setPixel(j,18, qRgb(95,91,218));
        if ((trace_slot[j].value_p != NULL) ) {
            if (trace_slot[j].value_p !=(char*)1){
                if (borderDelay==0) {
                    //std::cout << "Design Middle Box Signal Pattern" << std::endl;
                    // Draw border of the signal box
                    sigImage->setPixel(j,spMax , signalColor);
                    sigImage->setPixel(j,spMin , signalColor);
                    // colorize the interior of the signal box
                    for (int z=spMin+1;z<spMax; z++)
                        sigImage->setPixel(j,z,boxColor);
                }
                else
                    borderDelay--;
            }
            else { // Zero value
                // Just draw a two pixels in the middle (pixels 9 & 10)
                //std::cout << "Design Zero Signal Pattern" << std::endl;
                sigImage->setPixel(j,9,zeroColor);
                sigImage->setPixel(j,10,zeroColor);
            }
            previousValue = trace_slot[j].value_p;
        } else {
            // Transition detected : draw End and then Begin Pattern if enough place
            // Draw transition pattern: 2 pixels in the middle
            for (int z=sg3Min;z<=sg3Max; z++)
                sigImage->setPixel(j,z,signalColor);
            borderDelay=0;
            bool patternDrawn = false;
            if ((previousValue != NULL) && (previousValue !=(char*)1)) {
                // Design end Signal Pattern
                patternDrawn=true;
                // 2nd and 4th segments
                if (j-1 > 0) {
                    for (int z=sg2Min;z<=sg2Max; z++)
                        sigImage->setPixel(j-1,z,signalColor);
                    for (int z=sg4Min;z<=sg4Max; z++)
                        sigImage->setPixel(j-1,z,signalColor);
                    // draw box inner pixels in boxColor
                    for (int z=sg4Max+1;z<sg2Min; z++)
                        sigImage->setPixel(j-1,z,boxColor);
                    if (j-2<0) {
                        // Superpose 1st and 5th segments
                        for (int z=sg1Min;z<=sg1Max; z++)
                            sigImage->setPixel(j-1,z,signalColor);
                        for (int z=sg5Min;z<=sg5Max; z++)
                            sigImage->setPixel(j-1,z,signalColor);
                    }
                    // set border pixels back to black
                    sigImage->setPixel(j-1,spMax , backgroundColor);
                    sigImage->setPixel(j-1,spMin , backgroundColor);
                    sigImage->setPixel(j-1,spMax -1 , backgroundColor);
                    sigImage->setPixel(j-1,spMin +1, backgroundColor);
                }
                // 1st and 5th segments
                if (j-2>0){
                    for (int z=sg1Min;z<=sg1Max; z++)
                        sigImage->setPixel(j-2,z,signalColor);
                    for (int z=sg5Min;z<=sg5Max; z++)
                        sigImage->setPixel(j-2,z,signalColor);
                    // draw box inner pixels in boxColor
                    for (int z=sg5Max+1;z<sg1Min; z++)
                        sigImage->setPixel(j-2,z,boxColor);
                }
                // This is the end of an element -- finish to store it
                if ((funcValues.size() >0) && eltStarted) {
                    funcValues.back()->jEnd=j-2;
                }

                eltStarted = false;
            }
            if ((j+1<=pixelMax) && (((trace_slot[j+1].value_p != NULL) && (trace_slot[j+1].value_p !=(char*)1)))) {
                patternDrawn=true;
                // store the beginning of the element
                if (!eltStarted) {
                    // create a new sigValue for storing function names
                    sigValue = new SigFuncData();
                    sigValue->jStart=j+3;
                    sigValue->jEnd = pixelMax;
                    sigValue->value=QString((char *) (trace_slot[j+1].value_p));
                    funcValues.push_back(sigValue);
                }
                eltStarted = true;
                // Next value >0 ==> Design Begin Signal Pattern
                // 2nd and 4th segments
                if (j+1<=pixelMax){
                    for (int z=sg2Min;z<=sg2Max; z++)
                        sigImage->setPixel(j+1,z,signalColor);
                    for (int z=sg4Min;z<=sg4Max;z++)
                        sigImage->setPixel(j+1,z,signalColor);
                    // draw box inner pixels in boxColor
                    for (int z=sg4Max+1;z<sg2Min; z++)
                        sigImage->setPixel(j+1,z,boxColor);
                    if (j+2>pixelMax) {
                        // Superpose 1st and 5th segments
                        for (int z=sg1Min;z<=sg1Max; z++)
                            sigImage->setPixel(j+1,z,signalColor);
                        for (int z=sg5Min;z<=sg5Max; z++)
                            sigImage->setPixel(j+1,z,signalColor);
                    }
                    borderDelay++;
                }
                // 1st and 5th segments
                if (j+2<=pixelMax){
                    for (int z=sg1Min;z<=sg1Max; z++)
                        sigImage->setPixel(j+2,z,signalColor);
                    for (int z=sg5Min;z<=sg5Max; z++)
                        sigImage->setPixel(j+2,z,signalColor);
                    // draw box inner pixels in boxColor
                    for (int z=sg5Max+1;z<sg1Min; z++)
                        sigImage->setPixel(j+2,z,boxColor);
                    borderDelay++;
                }
                eltStarted=true;
            }

            if (!patternDrawn){ // Just draw a bar
                for (int z=spMin;z<=spMax; z++)
                    sigImage->setPixel(j,z,signalColor);
                patternDrawn=false;
                eltStarted=false;
            }
            previousValue=trace_slot[j].value_p;

        }

        drawTimeStamps(sigImage, j);
    }
    drawVerticalGrid(sigImage);
    return sigImage;
}


void TLGView::addSignalValues(QPainter& painter, QFont font, int lWidth, int i){
    // Adds the signal values within the boxes for analog signals
    painter.setFont(font);
    painter.setPen(Qt::white);
    for(unsigned long k = 0; k < sigValues.size(); k++){
        QString text = sigValues[k]->getHexaStr(traceWidth[i]);
        QFontMetrics fm(font);
        int txtWidth = fm.width(text);
        if (txtWidth <= sigValues[k]->getSpace()){
            painter.drawText(sigValues[k]->jStart,i*signalHeight + 14 , text);
        } else if (sigValues[k]->getSpace()>=2 * lWidth)
            painter.drawText(sigValues[k]->jStart,i*signalHeight + 14 , text.mid(0,sigValues[k]->getSpace()/lWidth -1) + "+");
        else if (sigValues[k]->getSpace() >= lWidth)
            painter.drawText(sigValues[k]->jStart,i*signalHeight + 14 , QString("+"));
    }
}

void TLGView::addFunctionValues(QPainter& painter, QFont font, int lWidth, int i){
    // Adds the function values within the boxes for signals of type "function"
    font.setCapitalization(QFont::MixedCase);
    painter.setFont(font);
    painter.setPen(Qt::white);
    for(unsigned long k = 0; k < funcValues.size(); k++){
        QString text = funcValues[k]->value;
        QFontMetrics fm(font);
        int txtWidth = fm.width(text);
        if (txtWidth <= funcValues[k]->getSpace()){
            painter.drawText(funcValues[k]->jStart,i*signalHeight + 12 , text);
        } else if (funcValues[k]->getSpace()>=2 * lWidth)
            painter.drawText(funcValues[k]->jStart,i*signalHeight + 12 , text.mid(0,funcValues[k]->getSpace()/lWidth -1) + "+");
        else if (funcValues[k]->getSpace() >= lWidth)
            painter.drawText(funcValues[k]->jStart,i*signalHeight + 12 , QString("+"));
    }
}


void TLGView::addSignalMessage(QPainter& painter, QFont font, QString message,int lWidth, int i){
    // Adds the signal message within all the boxes of the dignal
    painter.setFont(font);
    painter.setPen(Qt::white);
    for(unsigned long k = 0; k < sigValues.size(); k++){
        QFontMetrics fm(font);
        int txtWidth = fm.width(message);
        if (txtWidth <= sigValues[k]->getSpace()){
            // Center the message within the box.
            int x=sigValues[k]->jStart + (sigValues[k]->jEnd - sigValues[k]->jStart - txtWidth)/2;
            painter.drawText(x,i*signalHeight + 14 , message);
        } else if (sigValues[k]->getSpace()>=2 * lWidth)
            painter.drawText(sigValues[k]->jStart,i*signalHeight + 14 , message.mid(0,sigValues[k]->getSpace()/lWidth -1) + "+");
        else if (sigValues[k]->getSpace() >= lWidth)
            painter.drawText(sigValues[k]->jStart,i*signalHeight + 14 , QString("+"));
    }

}

void TLGView::printGridStick() {
    for(std::vector<Stick>::iterator it = std::begin(gridStick); it != std::end(gridStick); ++it) {
        std::cout << it->x << "\n";
    }
}

void TLGView::createGridSticks(){
    //std::cout << "CreateGridSticks ZoomFactor " << zoomFactor << std::endl;
    clearGridSticks();
    int X = size().width();
    int Y = peHeight;
    int xGridMin;
    int xGridMax;

    // calculating time interval for 10 main ticsteps approximatly

    uint64_t ti= round((position2time(X) - position2time(0))/10);
    uint64_t tiRound = extractPower10Round(ti);
    uint64_t xtmin= position2time(0);
    uint64_t xtmax= position2time(X);
    if ((xtmax-xtmin)/tiRound > 12)
        tiRound = tiRound *2;
    if ((xtmax-xtmin)/tiRound < 5)
        tiRound = tiRound/2;
    bool firstInterval = true;
    int xPlus= round((double)(time2position(xtmin + tiRound) - time2position(xtmin))/ (double) 10);
    // Find first main grid stick
    int f=-1;
    do {
        f++;
    } while (f * tiRound < xtmin);
    uint64_t xtfirst = f * tiRound;
    // Loop through all the intervals starting from f*tiRound
    // might be partially off the gview depending of the value of xt.
    for (uint64_t xt=xtfirst; xt<=xtmax; xt+= tiRound){
        int x = time2position(xt);
        xGridMin = time2position(xt);
        xGridMax=  time2position(xt+tiRound);
        if (firstInterval == true) {
            // Process first interval that is truncated
            for (int z=1; z<10 ; z++) {
                if ((x -xPlus *10) +z * xPlus >=0)
                    pushGridStick(Stick((x -xPlus *10)+z * xPlus,Y,Y-ticSize, QString(""),false));
            }
            firstInterval = false;
        }
        // Store main stick
        pushGridStick(Stick(x,Y,0,time2stringBis(xt),true));
        // Store intermediate sticks
        for (int z=1; z<10; z++){
            pushGridStick(Stick(x+z * xPlus,Y,Y-ticSize,QString(""),false));
        }
    }
}

void TLGView::paintEvent(QPaintEvent *event)
{
    // This procedure creates an image for each last graphical udata
    // that has been stored in the traces vector. Then, the image
    // is displayed in the TimeLine window.

#ifdef PROFILE
    clock_t begin = clock();
#endif

    //std::cout << " paintEvent" << std::endl;

    uint64_t hot_start_ts=0; // first window point timestamp for hotspots update
    uint64_t hot_end_ts=0; // last window point timestamp for hotspots update

    double updatedZoomF; // updated zoom factor

    drawMutex->lock();

    if (waiting_results)
    {
        drawMutex->unlock();
        return;
    }

    const QRect r = event->rect();
    // [x0, x1] correxponds to the TLGView X coordinates
    int x0 = r.x(); // first abcisse of the event rectangle
    int x1 = r.x() + r.width(); // second abcisse of the event rectangle
    uint64_t t0, t1;

    // calculate corresponding Time Range values according to current zoomFactor
    getTimeWindow(x0, x1, t0, t1);
    paintNb +=1;
    // ZoomFactor corresponding to the data received
    updatedZoomF=  double(nbPixels) / double(end_ts - start_ts);
    //update zoomFactor to the one received ..
    zoomFactor = updatedZoomF;

    QImage* signalImage;// image representing one signal
    QImage* resultImage;// resulting image corresponding to the vertical juxtaposition of signal images
    QPainter painter;
    QFont font("Times",9,QFont::Bold); // font of the text
    font.setCapitalization(QFont::AllUppercase);
    QFontMetrics fm(font);
    int lWidth = fm.width(QString("+"));

    bool logicBox=false;
    QString message;

    if ((!dataReceived) || (max_ts <=0) || reloadData) {
        getData(t0,t1,x1-x0);
        timeOffset =t0;
        drawMutex->unlock();
        reloadData=false;
        return;
    }


    if ((nbPixels != x1-x0) || (start_ts==end_ts)) {
        // if data stored doesn't correspond to the one we need
        // then, get new corresponding data
        timeOffset=t0;
        getData(t0,t1,x1-x0);
    }
    else {
        resultImage = new QImage(nbPixels, traces.size() * signalHeight, QImage::Format_ARGB32_Premultiplied);
        resultImage->fill(Qt::transparent);
        painter.begin(resultImage);
        deleteImages();
        // paint singnals on the window from traces data vectors
        if (firstPainting) {
            signalsView->clearSelection();
            firstPainting = false;
        }

        int pixelMax;
        if (max_ts <= end_ts)
            pixelMax= int ((max_ts - start_ts) * zoomFactor) -1;
        else
            pixelMax= nbPixels -1;

        this->setMinimumHeight(traces.size() * signalHeight);
        this->setMaximumHeight(traces.size() * signalHeight);

        createGridSticks();
        //printGridStick();

        for (unsigned long i = 0; i < traces.size(); i++) {
            QString sigPath= backendUser->idMap[traceId[i]];
            //std::cout << FRED( "Displaying trace #") << traceId[i] << " " << sigPath.toStdString() <<" Width " << traceWidth[i] << std::endl;
            Display* display = signalsDisplayMap[sigPath];
            if (display==NULL) {
                // Make images for all signals without display
                if (traceWidth[i]<=1)
                    // This is a simple digital signal
                    if (i==highlightRow)
                        signalImage = makeDigSigImage(traces[i], nbPixels, pixelMax,true);
                    else
                        signalImage = makeDigSigImage(traces[i], nbPixels, pixelMax,false);
                else
                    // This is an analog signal
                    // traceWidth>1 This is an analog signal or function signal
                    if (i==highlightRow)
                        signalImage = makeAnaSigImage(traces[i], traceWidth[i], nbPixels, pixelMax,true);
                    else
                        signalImage = makeAnaSigImage(traces[i], traceWidth[i], nbPixels, pixelMax,false);
            } else {
                // Make images for all signals with a display type
                if (display->displayType == "logic_box") {
                    logicBox=true;
                    message=display->message;
                    if (i==highlightRow)
                        signalImage = makeDigBoxSigImage(traces[i], nbPixels, pixelMax,true);
                    else
                        signalImage = makeDigBoxSigImage(traces[i], nbPixels, pixelMax,false);
                } else if (display->displayType == "string") {
                    if (i==highlightRow)
                        signalImage = makeFuncSigImage(traces[i], traceWidth[i], nbPixels, pixelMax,true);
                    else
                        signalImage = makeFuncSigImage(traces[i], traceWidth[i], nbPixels, pixelMax,false);
                }  else if (display->displayType == "string_box") {
                    if (i==highlightRow)
                        signalImage = makeFuncBoxSigImage(traces[i], traceWidth[i], nbPixels, pixelMax,true);
                    else
                        signalImage = makeFuncBoxSigImage(traces[i], traceWidth[i], nbPixels, pixelMax,false);
                }
            }
            // signal image must be painted on the Timeline widget
            if (signalImage != NULL)
                painter.drawImage( 0,i*signalHeight, *signalImage,0,0);

            if (logicBox){
                addSignalMessage(painter,font,message,lWidth,i);
                logicBox = false;
            }
            if (traceWidth[i] > 1)
                if (display == NULL)
                    addSignalValues(painter,font, lWidth,i);
                else if ((display->displayType == "string") || (display->displayType == "string_box"))
                    addFunctionValues(painter,font, lWidth,i);
        }
        painter.end();
        // now draw this resultImage on the widget
        QPainter tlgviewPainter;
        tlgviewPainter.begin(this);
        tlgviewPainter.drawPixmap(QPoint(0,0),QPixmap::fromImage(*resultImage));
        tlgviewPainter.end();
        delete resultImage;
        // update timeOffset
        timeOffset=start_ts;
        hot_start_ts=start_ts; // first window point timestamp
        hot_end_ts=end_ts; // last window point timestamp
    }

    // Make new Legend widget
    tl->hLegendW->setWindowInterval(start_ts,end_ts);
    setHScrollbarParams();
    tl->hLegendW->update();
    tl->hLegendW->show();
    // in case no time interval has been selected, update the hotspots widget
    if (!drawRect)
        tl->updateHotspotsWidget(hot_start_ts,hot_end_ts);
    drawMutex->unlock();
    //std::cout << "End paintEvent" << std::endl;

#ifdef PROFILE
    clock_t end = clock();

    printf("Processed request time: %f\n", (double)(end - begin) / CLOCKS_PER_SEC);
#endif

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

void TLGView::handleSliderAction(int action){
    drawMutex->lock();
    qDebug() << "handleSliderAction";
    int sign;
    if (action==QAbstractSlider::SliderPageStepAdd)
        sign=1;
    else if (action==QAbstractSlider::SliderPageStepSub)
        sign = -1;
    uint64_t t0,t1;
    getTimeWindow(t0,t1);
    // calculate a deltaT which is time window width /20
    int deltaT= (sbar->pageStep()*maxTime) / TLGView::scrollBarMaxRange ;
    if ((sign == - 1) && (deltaT < t0))
        getData(t0 - deltaT,t1 - deltaT ,int((t1-t0) *zoomFactor));
    else if (sign==1)
        getData(t0 + deltaT,t1 + deltaT ,int((t1-t0) *zoomFactor));

    drawMutex->unlock();
}

void TLGView::handleSBarChange(int value){
    // User moved the cursor
    // cursor has a new value. This is directly a time value.
    // This must correspond to a change in the graphic view.
    return;
    //std::cout << "@@@@@ handleSBarChange" << std::endl;

    drawMutex->lock();

    //std::cout << "handleSBarChange" << std::endl;
    //std::cout << sbarValue << std::endl;
    //std::cout << value << std::endl;

    int delta = value - sbarValue;
    // need of getting the sign of delta
    int sign=1;
    if (delta >=0)
        sign = 1;
    else
        sign = -1;
    //std::cout << delta << std::endl;
    uint64_t deltaT = sign * delta * maxTime / TLGView::scrollBarMaxRange;
    //std::cout << deltaT <<  std::endl;
    sbarValue=value;
    if (sign>=0)
        getData(start_ts + deltaT, end_ts - deltaT, nbPixels);
    else if (start_ts >= deltaT)
        getData(start_ts - deltaT, end_ts - deltaT, nbPixels);
    else
        getData(0, end_ts-start_ts,nbPixels);

    drawMutex->unlock();
}


void TLGView::setHScrollbarParams(){
    //std::cout << "setHScrollbarParams " << std::endl;
    int pageStep = TLGView::scrollBarMaxRange * (end_ts - start_ts)/ maxTime;
    this->sbar->setSingleStep(std::max(1, (int) (TLGView::scrollCoeff / zoomFactor)));
    this->sbar->setPageStep(pageStep);
    this->sbar->setMinimum(0);
    this->sbar->setMaximum(TLGView::scrollBarMaxRange - pageStep);

    this->sbar->setValue(start_ts * TLGView::scrollBarMaxRange /maxTime);
    sbarValue=start_ts * TLGView::scrollBarMaxRange /maxTime;
    /*std::cout << "start_ts" << start_ts << std::endl;
    std::cout << "end_ts" << end_ts << std::endl;
    std::cout << "max_ts" << max_ts << std::endl;
    std::cout << "maxTime" << maxTime << std::endl;
    std::cout << "pageStep " << pageStep << std::endl;
    //std::cout << "value" <<  value << std::endl;
    std::cout << "maxTime - pageStep "  << maxTime - pageStep << std::endl;
  */
}

/*void TLGView::setPageStep(){
  uint64_t t0, t1;
  getTimeWindow(t0, t1);
  assert(maxScrollTime!= 0);
  assert(zoomFactor!=0);
  this->sbar->setPageStep(TLGView::scrollBarMaxRange * (t1 - t0) / maxScrollTime);
  this->sbar->setSingleStep(std::max(1, (int) (TLGView::scrollCoeff / zoomFactor)));
}
*/

/*
void TLGView::setMaxScrollTime(){
  // << "[-]setMaxScrollTime " << std::endl;
  uint64_t w = timeWidth();
  // add a signalHeight pixel margin after last event (no margin looks weird)
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
*/

/*void TLGView::autoReconstruct(){
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
*/

void TLGView::getTimeWindow(uint64_t& t0, uint64_t& t1) const{
    getTimeWindow(0, size().width(), t0, t1);
}

void TLGView::getTimeWindow(int x0, int x1, uint64_t& t0, uint64_t& t1) const{
    //std::cout << "getTimeWindow " << "timeOffset " << timeOffset << "zoomFactor " << zoomFactor << std::endl;
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
    //std::cout << "ZoomFactor " << zoomFactor << "timeOffset "<<  timeOffset << std::endl;
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
        return QString::number(double(t) / double(1000000),'e',5) + QString("us");
        //return formatTimeStamp(QString::number(double(t) / double(1000))) + " ns";
    case FC_CYCLE_MODE:
        return "";
        /*T = get_fc_period();
      if (T == 0) {
        std::cout << "[-] Error: cluster period = 0" << std::endl;
        exit(1);
      }
      return formatTimeStamp(QString::number(t / T));
      */
    case CLUSTER_CYCLE_MODE:
        return "";
        /*
      T = get_cluster_period();
      if (T == 0) {
        std::cout << "[-] Error: cluster period = 0" << std::endl;
        exit(1);
      }
      return formatTimeStamp(QString::number(t / T));
      */
    }
}


QString TLGView::time2stringBis(uint64_t t) const {
    //int T;
    QString text;
    switch(currentMode){
    default:
    case TIME_MODE:
        // format in us unit
        text.setNum((double)(t) / (double)(1000000),'e',5 );
        return text;
        //return QString::number(double(t) / double(1000000),'f',4 );
        //return formatTimeStamp(QString::number(double(t) / double(1000000),'g',2 ));
    case FC_CYCLE_MODE:
        return "";
        /*T = get_fc_period();
      if (T == 0) {
        std::cout << "[-] Error: cluster period = 0" << std::endl;
        exit(1);
      }*/

    case CLUSTER_CYCLE_MODE:
        return "";
        /*T = get_cluster_period();
      if (T == 0) {
        std::cout << "[-] Error: cluster period = 0" << std::endl;
        exit(1);
      }
      return formatTimeStamp(QString::number(t / T));
      */
    }
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

/*static int searchChildInLayout(const QLayoutItem* item, const QLayout* layout){
  // reimplementation of the indexOf method (for Qt < 5.12 compatibility)
  for (int i = 0; i < layout->count(); i++){
    if (item == layout->itemAt(i)) return i;
  }
  return -1;
}
*/

/*
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

*/

/*
void TLGView::clearWidgets(QLayout * layout) {
   if (! layout)
      return;
   while (auto item = layout->takeAt(0)) {
      delete item->widget();
      clearWidgets(item->layout());
   }
}
*/
void TLGView::execMessage(const QString &text){
    msgBox.setText(text);
    msgBox.exec();
}

uint64_t TLGView::extractPower10Round(uint64_t x){
    return pow(10,floor(log10(x) + 0.6));
}


/*void TLGView::addTraceToLegend(std::string path,bool addMenuAction){

  //qDebug() << "##### AddTraceToLegend ##### " << path;

  int i = path.size() - 1;
  for (; i >= 0 && path[i] != '/'; i--);
  //int insertIdx = Data_manager::TLHeight * 2 + 1;
  int insertIdx = 11 * 2 + 1;
  insertIdx += std::max(0, (int) id_to_display().size() - 1 -10) * 2;

  insertLegendItem(insertIdx, path.c_str(), addMenuAction);
}
*/

// All Zooming Functions

void TLGView::shiftAfterZoom(double coeff){
    //std::cout << "@@@@ shift after zoom" << std::endl;
    uint64_t  t0, t1;
    getTimeWindow(t0, t1);
    int64_t delta = (int64_t) ((double)(t1 - t0) * coeff);
    timeOffset = (uint64_t) std::max((int64_t) 0, ((int64_t)  + delta));
    sbar->setValue(timeOffset * TLGView::scrollBarMaxRange / maxScrollTime);
    getData(timeOffset, timeOffset + nbPixels/zoomFactor, nbPixels);
}

void TLGView::zoomIn(bool shiftAfter){
    if (zoomFactor >= 1. / 10) return;
    //std::cout << "@@@@ zoomIn zoomFactor " << zoomFactor << std::endl;
    zoomFactor *= 2;
    //setMaxScrollTime();
    if (shiftAfter)
        shiftAfterZoom(+1 / 2.);
    //setPageStep();
    //update();
    emit zoomOccured();

}

void TLGView::zoomOut(bool shiftAfter){
    // prevent user from crashing the app by zoom out infinitly
    //std::cout << "timeWidth() " << timeWidth() << "  " << maxTime << "  " << 1.5 * maxTime << std::endl;
    //std::cout << "@@@@ old zoomOut zoomFactor  " << zoomFactor << std::endl;
    // As of now, this is always the case ... this means that maxTime is not updated. And it should
    //if (timeWidth() > 1.5 * maxTime) return;
    zoomFactor /= 2;
    //std::cout << "@@@@ new zoomOut zoomFactor  " << zoomFactor << std::endl;
    if (timeWidth()>pow(2,64) -1) {
        //std::cout << "revert to previous zoom factor and do nothing" << std::endl;
        zoomFactor *=2;
        return;
    }
    //setMaxScrollTime();
    if (shiftAfter)
        shiftAfterZoom(-1 / 4.);
    //setPageStep();
    //update();
    emit zoomOccured();

}

void TLGView::viewAll() {
    drawMutex->lock();
    // Displays all the available timeline in the gview
    // We just need to make a call to getView
    getData(0,-1, this->width(), true);
    drawMutex->unlock();
}

// End all Zooming functions

void TLGView::updateVerticalLine(QPointF q){
    std::cout << "update Vertical Line" << std::endl;
    std::cout << "q.x()" << q.x() << std::endl;
    verticalLineTime = position2time(q.x());
    std::cout << "verticalLineTime " << verticalLineTime << std::endl;
    std::cout << "zoomFactor " << zoomFactor << std::endl;
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
        for (i=0; i< signalsView->model->rowCount(parent) ; i++) {
            idx = signalsView->model->index(i,0,parent);
            siblingIdx = signalsView->model->index(i,1,parent);
            signalName=findEltOnRow(idx,siblingIdx,rowNb, rowIdx);
            if (QString::compare(signalName, QString(""))!=0)
                return signalName;
        }
    }
    return QString("");
}



void TLGView::mousePressEvent(QMouseEvent* event)
{
    drawMutex->lock();
    // Dealing with mouse press event
    // set usefull variables for this event
    qDebug() << "mousePressEvent";
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
    drawMutex->unlock();
}


void TLGView::highlightFunction(QPointF q) {
    // This function highlights the function on which the user clicked
    // in the functions table.
    //assert(stallChart);
    return;
    /*
    assert(functionsDock);
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
    }*/
}

void TLGView::mouseReleaseEvent(QMouseEvent *event) {
    drawMutex->lock();

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
        tl->updateHotspotsWidget(t0Select,t1Select);
        //tl->updateSignalsStatView(signalsView,t0Select,t1Select);
        // update m_rect
        m_rect.setBottomLeft(QPoint(time2position(t0Select),0));
        m_rect.setWidth(time2position(t1Select) - time2position(t0Select));
        // update the time interval display field
        auto diff = m_rect.right() - m_rect.left();
        uint64_t distance = abs(diff);
        updateTimeInterval(distance);
        updateVerticalLine(functionPoint);// updates the timestamp value

        drawRect = true;
        selectionRect = true;
        // save the selection area abcisses that is displayed
        t0SelectDisplayed = t0Select;
        t1SelectDisplayed = t1Select;
        selectionRect=true;
    }
    else if (leftClick){
        updateVerticalLine(functionPoint);// updates the timestamp value
        highlightFunction(functionPoint); // highlights the function where the click occured
        drawLine = true; // used when repainting the gview to draw a vertical line
        //std::cout << "@@@@@ drawline " << drawLine << std::endl;
    }
    else if (rightClick) {
        // Nothing to draw : nor timestamp line, nore selected area
        drawLine = false;
        //std::cout << "@@@@@ drawline " << drawLine << std::endl;
        drawRect = false;
        selectionRect = false;
        t0Select=0;
        t1Select=0;
        updateTimeInterval(0);
        tl->setTimestamp(time2string(0));
        // update the hotspot table to reflect all the hotspots
        tl->updateHotspotsWidget(start_ts,end_ts);
    }

    // reset all flags
    rightClick = false;
    leftClick  = false;
    mouseMoving = false;
    movingRightEdge=false;
    movingLeftEdge=false;
    update();
    drawMutex->unlock();
}

void TLGView::mouseMoveEvent(QMouseEvent *event) {

    drawMutex->lock();
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
    drawMutex->unlock();
}


void TLGView::placeMouseTimestampOnPoint(uint64_t mouseTimestamp, QPointF mousePosition) {
    // place the mouseTimestamp on  mouse position (X wise)
    //std::cout << "@@@@@@@ placeMouseTimestampOnPoint" << std::endl;
    uint64_t t0, t1;
    getTimeWindow(t0, t1);
    uint64_t interval = t1 - t0;
    uint64_t targetTimestamp = position2time(mousePosition.rx());

    if (mouseTimestamp <= targetTimestamp) {
        if (timeOffset >= (targetTimestamp - mouseTimestamp))
            timeOffset = timeOffset - (targetTimestamp - mouseTimestamp);
    }
    else {
        timeOffset = timeOffset + (mouseTimestamp - targetTimestamp);
    }
    getData(timeOffset,timeOffset + interval,nbPixels);
}

void TLGView::centerGviewOnVerticalLine() {
    // This function centers the graphical view on the current vertical line time
    // This is not what to do
    //std::cout << "@@@@@@ CenterGViewOnVerticalLine " << verticalLineTime << std::endl;
    uint64_t t0, t1;
    getTimeWindow(t0, t1);
    //std::cout << "t0,t1 " << t0 << ", " << t1 << std::endl;
    uint64_t targetTimestamp = t0 + (t1-t0)/2;
    //std::cout << "targetTimeStamp " << targetTimestamp << std::endl;
    // put targetTimeStamp on center of GView (mouseTimeStamp)
    uint64_t mouseTimestamp = verticalLineTime;
    if (mouseTimestamp <= targetTimestamp) {
        if (timeOffset >= (targetTimestamp - mouseTimestamp))
            timeOffset = timeOffset - (targetTimestamp - mouseTimestamp);
        else {
            // In this case, it's not possible to put the verticalLine on the center of the
            // Gview. Maximum timeOffset possible is zero.
            timeOffset = 0;
        }
    }
    else {
        // mouseTimestamp > targetTimeStamp ==> negative translation
        timeOffset = timeOffset + (mouseTimestamp - targetTimestamp);
    }

    getData(timeOffset, timeOffset + (t1-t0), int((t1-t0) *zoomFactor));
}


void TLGView::wheelEvent(QWheelEvent *event)
{
    drawMutex->lock();
    //std::cout <<"[-] @@@@@@ WheelEvent" << std::endl;
    // Get Mouse Position and store in the list
    if (event->modifiers().testFlag(Qt::ControlModifier)) {
        // Zoom with ctrl + wheel event
        //std::cout << "Zoom from wheelEvent" << std::endl;
        QPointF mousePosition(event->x(), event->y());
        uint64_t mouseTimestamp = position2time(mousePosition.rx());

        QPoint numPixels = event->pixelDelta();
        QPoint numDegrees = event->angleDelta() / 8;

        if (!numPixels.isNull()) {
            scrollWithPixels(numPixels);
        } else if (!numDegrees.isNull()) {

            double steps = (double)numDegrees.y() / 15;

            if (steps > 0)
            {
                zoomFactor *= pow(1.1, steps);
            }
            else
            {
                zoomFactor /= pow(1.1, -steps);
            }
        }

        placeMouseTimestampOnPoint(mouseTimestamp, mousePosition);

        event->accept();
    } else if ((wheelEventCount == 1 ) || (wheelEventCount % 10 == 0 )) {
        // horizontal scrollbar move with mouse wheel
        // first need to know if delta is + or -
        int delta = event->angleDelta().y();
        event->accept();
        int sign;
        if (delta >=0)
            sign = 1;
        else
            sign = -1;

        uint64_t t0,t1;
        getTimeWindow(t0,t1);
        // calculate a deltaT which is time window width /20
        uint64_t deltaT= (t1-t0)/20;
        if ((sign == 1) && (deltaT < t0))
            getData(t0 - deltaT,t1 - deltaT ,int((t1-t0) *zoomFactor));
        else if (sign==-1)
            getData(t0 + deltaT,t1 + deltaT ,int((t1-t0) *zoomFactor));

    }
    // std::cout << "[-] WheelEvent Ended" << std::endl;
    //tl->hLegendW->update();
    //update();
    drawMutex->unlock();

}

void TLGView::verticalLineZoomIn() {
    // zoomIn()
    // center view on red vertical line if it exists
    if (!drawLine)
        return;
    drawMutex->lock();
    zoomIn(false);
    centerGviewOnVerticalLine();
    drawMutex->unlock();
}

void TLGView::verticalLineZoomOut() {
    //zoomOut()
    // center view on red vertical line if it exists
    if (!drawLine)
        return;
    drawMutex->lock();
    zoomOut(false);
    centerGviewOnVerticalLine();
    drawMutex->unlock();
}

void TLGView::mouseDoubleClickEvent(QMouseEvent* event) {
    drawMutex->lock();
    QPointF mousePosition(event->x(), event->y());
    uint64_t mouseTimestamp = position2time(mousePosition.rx());

    zoomIn();
    placeMouseTimestampOnPoint(mouseTimestamp, mousePosition);
    update();
    drawMutex->unlock();
}

void TLGView::scrollWithPixels(const QPoint &pixel)
{
    int s = pixel.y();
    std::cout << "Type of mouse wheel not yet implemented" << s << std::endl;
}

void TLGView::scrollWithDegrees(const QPoint &step)
{
    drawMutex->lock();
    int s = step.y();
    if (s==1)
        zoomIn();
    else
        zoomOut();

    drawMutex->unlock();
}

void TLGView::gotoTimestamp(int64_t t){
    drawMutex->lock();
    updateVerticalLine(QPointF(time2position(t), 0));
    drawMutex->unlock();
    //centerOn(t, sceneRect().height() / 2);
}

void TLGView::sliderMoved(int x){
    drawMutex->lock();
    std::cout << "SliderMoved" << std::endl;
    timeOffset = x * maxTime / scrollBarMaxRange;
    std::cout << "timeOffset " << timeOffset << std::endl;
    std::cout << "zoomFactor " << zoomFactor << std::endl;
    getData(timeOffset, timeOffset + nbPixels/zoomFactor, nbPixels);
    drawMutex->unlock();
}


void TLGView::addSignalToGview(const QString signalPath) {
    return;
    //add_trace_to_timeline(signalPath.toStdString());
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
        for (i=0; i< signalsView->model->rowCount(parent) ; i++) {
            idx = signalsView->model->index(i,0,parent);
            siblingIdx = signalsView->model->index(i,1,parent);
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

    drawMutex->lock();
    qDebug() << "[-] @@@@@@@ handleSignalNodeExpanded: Adding/removing  Signals to Gview" ;
    // First get signals to add to the timeline
    getData(start_ts,end_ts,nbPixels);

    //addSignalsToGview(signalsView->rootIndex(),signalsView->rootIndex());
    // Update stats Models & view if already created
    // We must check here if gvsoc has already been run. If not , this
    // call to updateSignalsStatView ends in a core dump because .....
    // gvsocRunFlag is useless as it is set before gvsoc is really started
    // qDebug() << "[-] gvsocRunFlag: " << tl->gvsocRunFlag;
    /*if (tl->gvsocRunFlag) {
      tl->updateSignalsStatView(signalsView);
    }
    */
    // Then, sync the two views so signals are displayed in front of their name
    /*this->repaint();
    tl->setGviewVScrollValue();
    tl->update();
    this->update();
    this->show();
    tl->show()*/
    drawMutex->unlock();
}
