#ifndef TIMELINE_H
#define TIMELINE_H

#include <QObject>
#include <QWidget>
#include <QTimer>
#include <QPainter>
#include <QImage>
#include <QMutex>



#include "profiler_backend_user.h"
#include "profiler_backend.hpp"

class TimeLine : public QWidget
{
    Q_OBJECT
public:
    explicit TimeLine(Profiler_backend_user* probackendUserRef, QWidget *parent = nullptr);
    void updateMe(std::vector<std::vector<Profiler_backend_slot_info>> &trace_slots, int64_t start_timestamp, int64_t end_timestamp, int nb_slots);
private:
    Profiler_backend_user* backendUser;
    // String data received in updateMe
    bool firstData=true; // true when first data received has not been processed yet
    bool dataReceived =false; // true when first data has been received
    int64_t start_ts=0; // first window point timestamp
    int64_t end_ts=0; // last window point timestamp
    int nbPixels=0; // length of the trace in pixels
    std::vector<std::vector<Profiler_backend_slot_info>> traces;
    //
    void paintEvent(QPaintEvent *event);
    QTimer timer;
    QMutex drawMutex;
    double zoomFactor = 1. / 10000000;   /**< number of pixels per time unit */
    uint64_t timeOffset =0;  /**< timestamp of the leftmost displayed pixel */

    void getTimeWindow(int x0, int x1, uint64_t& t0, uint64_t& t1) const;
signals:
    void dataIsReady();
public slots:
     void displayView();
private slots:
    void getData(uint64_t tMin,uint64_t tMax, int pixelsNb);
    QImage* makeImage(std::vector<Profiler_backend_slot_info> &trace_slot, int nb_slots);

};

#endif // TIMELINE_H
