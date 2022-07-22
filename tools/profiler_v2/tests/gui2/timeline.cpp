#include "timeline.h"
#include <QColor>
#include <unistd.h>
#include <assert.h>
#include <QRect>
#include <QPaintEvent>

TimeLine::TimeLine(Profiler_backend_user* backendUserRef, QWidget *parent) : QWidget(parent)
{
    std::cout << "Create Timeline" << std::endl;
    backendUser= backendUserRef;
    connect(this,SIGNAL(dataIsReady()),this, SLOT(displayView()));
    connect(&timer, SIGNAL(timeout()), this, SLOT(displayView()), Qt::QueuedConnection);
    timer.start(2000);
    displayView();
}

void TimeLine::getTimeWindow(int x0, int x1, uint64_t& t0, uint64_t& t1) const{
  t0 = timeOffset;
  assert(zoomFactor!= 0);
  t1 = timeOffset + (uint64_t) ((x1 - x0) / zoomFactor);
}

void TimeLine::displayView() {
    update();
    show();
}

void TimeLine::getData(uint64_t tMin,uint64_t tMax, int pixelsNb ) {

    std::cout << "getData" << std::endl;
    /*QMapIterator<QString,int> iter(backend.getMap());

    while(iter.hasNext())
    {
        iter.next();
        qDebug() << iter.key() << " : " << iter.value();
    }
    */

   // If not enough traces have been received, then don't do anything

   if (!(backendUser->canGetView())){
        std::cout << "GetData aborted" << std::endl;
        return;
   }

   std::vector<int> ids = backendUser->getIds();
   std::cout << "ids size " << ids.size() << std::endl;


    /*if (firstData)
    {
        std::cout << "firstData " << firstData << std::endl;
        std::cout << "Call get_view " << ids[0] << " " <<  -1 <<" " << -1 << " " << this->width() << std::endl;
        backendUser->backend->get_view(this, ids, -1,-1,this->width());
        firstData=false;
    }*/
    //else {
        std::cout << "Call get_view " << ids[0] << " " <<  tMin <<" " << tMax  << " " <<  pixelsNb << std::endl;
        backendUser->backend->get_view(this, ids, tMin, tMax,pixelsNb);
    //}
    std::cout << "End getData " << std::endl;
}

QImage* TimeLine::makeImage(std::vector<Profiler_backend_slot_info> &trace_slot, int nb_slots) {
    // This procedure transforms the trace vector in a QImage nb_slots x 30
    // For a  "1" value, we fill top column pixel
    // For a  "0" values, we fill bottom column pixel
    // For a "-1" value, we fill all the column
    // with green color
    // Image background is white
    std::cout << "makeImage" << std::endl;
    QImage* sigImage= new QImage(nb_slots, 30, QImage::Format_RGB32);
    sigImage->fill(QColor(Qt::black).rgb());
    std::cout << " nb_slots: " << nb_slots << std::endl;

    for (int j = 0; j < trace_slot.size(); j++)
    {
        std::cout << trace_slot[j].value;
        if (trace_slot[j].value == 1.0)
           sigImage->setPixel(j,0, qRgb(43,193,20));
        else if (trace_slot[j].value == 0.0)
           sigImage->setPixel(j,29, qRgb(43,193,20));
        else if (trace_slot[j].value == -1.0)
           for (int z=0;z<30; z++)
              sigImage->setPixel(j,z,qRgb(43,193,20));
    }
    std::cout << std::endl;
    return sigImage;
}

void TimeLine::updateMe( std::vector<std::vector<Profiler_backend_slot_info>> &trace_slots,
                    int64_t start_timestamp,
                    int64_t end_timestamp,
                    int nb_slots) {

    std::cout << "updateMe" << std::endl;
    drawMutex.lock();
    if (start_timestamp == end_timestamp){
        start_ts=start_timestamp;
        end_ts=-1;
        return;
    }



    std::cout << "start: " << start_timestamp << " end: " << end_timestamp << " nb_slots " << nb_slots << " " << trace_slots.size()<< std::endl;
    // just store data
    start_ts=start_timestamp;
    end_ts=end_timestamp;
    nbPixels = nb_slots;
    traces=trace_slots; // stores a reference to the trace_slots vector
    if (!dataReceived)
        dataReceived=true;
    // Call update & show to repaint the TimeLine window.
    emit dataIsReady();
    drawMutex.unlock();
}

void TimeLine::paintEvent(QPaintEvent *event)
{
    // This procedure creates an image for each last graphical data
    // that has been stored in the traces vector. Then, the image
    // is displayed in the TimeLine window.


    std::cout << "paintEvent" << std::endl;

    drawMutex.lock();
    const QRect r = event->rect();
    // [x0, x1] correxponds to the TLGView X coordinates
    int x0 = r.x(); // first abcisse of the event rectangle
    int x1 = r.x() + r.width(); // second abcisse of the event rectangle
    uint64_t t0, t1;

    // calculate corresponding Time Range values
    getTimeWindow(x0, x1, t0, t1);

    std::cout << "paintEvent-1 nb_traces:" << traces.size() << std::endl;
    std::cout << "time range: [" << t0<< "," <<  t1   << "]"  << std::endl;
    QImage* signalImage;
    QPainter painter(this);

    if (!dataReceived) {
        std::cout << "Traces not received yet" << std::endl;
        getData(t0,t1,x1-x0);
        drawMutex.unlock();
        return;
    }

    // if data stored doesn't correspond to the one we need
    // then, get new corresponding data
    if ((nbPixels != x1-x0) || (t0!=start_ts) || (t1!=end_ts))
        getData(t0,t1,x1-x0);
    else
        // paint singnals on the window from traces data vectors
        for (int i = 0; i < traces.size(); i++)
        {   signalImage = makeImage(traces[i], nbPixels);
            // signal image must be painted on the Timeline widget
            painter.drawImage( 0,i*30, *signalImage,0,0);
        }
    drawMutex.unlock();

}
