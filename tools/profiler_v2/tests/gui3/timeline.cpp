#include "timeline.h"
#include <QColor>
#include <unistd.h>
#include <iostream>

TimeLine::TimeLine(QWidget *parent) : QWidget(parent)
{
    std::cout << "Create Timeline" << std::endl;
    //backendUser= backendUserRef;
    //connect(&timer, SIGNAL(timeout()), this, SLOT(getData()), Qt::QueuedConnection);
    //timer.start(2000);

}

QImage* TimeLine::makeImage(int nb_slots) {
    // This procedure transforms the trace vector in a QImage nb_slots x 30
    // For a  "1" value, we fill top column pixel
    // For a  "0" values, we fill bottom column pixel
    // For a "-1" value, we fill all the column
    // with green color
    // Image background is white
    std::cout << "makeImage" << std::endl;
    QImage* sigImage= new QImage(nb_slots, 30, QImage::Format_RGB32);
    sigImage->fill(QColor(Qt::red).rgb());
    //std::cout << "trace_slot.size() " << trace_slot.size() << " nb_slots: " << nb_slots << std::endl;

    return sigImage;
}

void TimeLine::updateMe( std::vector<std::vector<Profiler_backend_slot_info>> &trace_slots,
                    int64_t start_timestamp,
                    int64_t end_timestamp,
                    int nb_slots) {


    std::cout << "updateMe" << std::endl;

    /*std::cout << "start: " << start_timestamp << " end: " << end_timestamp << " nb_slots " << nb_slots << " " << trace_slots.size()<< std::endl;
    // just store data
    t0=start_timestamp;
    t1=end_timestamp;
    nbPixels = nb_slots;
    traces=trace_slots; // stores a reference to the trace_slots vector
    if (!dataReceived)
        dataReceived=true;
    // Call update & show to repaint the TimeLine window.
    update();
    show();*/
}

void TimeLine::paintEvent(QPaintEvent *event)
{
    // This procedure creates an image for each last graphical data
    // that has been stored in the traces vector. Then, the image
    // is displayed in the TimeLine window.


    std::cout << "paintEvent width:" << this->width()<< std::endl;
    QImage* signalImage;
    QPainter painter(this);
    drawMutex.lock();

    signalImage = makeImage(this->width());
    std::cout << "paintEvent-2" << std::endl;
    // signal image must be painted on the Timeline widget
    painter.drawImage( 0,30, *signalImage,0,0);
    painter.drawImage( 0,90, *signalImage,0,0);
    painter.drawImage( 0,150, *signalImage,0,0);
    std::cout << "paintEvent-4" << std::endl;


    drawMutex.unlock();
 std::cout << "paintEvent-5" << std::endl;
}
