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
    explicit TimeLine(QWidget *parent = nullptr);
    void updateMe(std::vector<std::vector<Profiler_backend_slot_info>> &trace_slots, int64_t start_timestamp, int64_t end_timestamp, int nb_slots);
signals:

public slots:
private:
     void paintEvent(QPaintEvent *event);
     QTimer timer;
     QMutex drawMutex;
     QImage* makeImage( int nb_slots);
};

#endif // TIMELINE_H
