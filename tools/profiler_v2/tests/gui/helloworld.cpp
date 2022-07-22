// QT helloworld application 

#include <QApplication>
#include <QDebug>

#include "profiler_backend.hpp"
#include <unistd.h>
#include <QWidget>
#include <QTimer>
#include <QPainter>
#include <QColor>
#include <QObject>
#include <QMessageBox>
#include <QMap>
#include <QString>
#include <iostream>

class Profiler_backend_user: public Profiler_backend_user_interface
{
public:
    void start(std::string fifo_path);
    void update_view(   void* widget,std::vector<std::vector<Profiler_backend_slot_info>> &trace_slots, 
                        int64_t start_timestamp, 
                        int64_t end_timestamp, 
                        int nb_slots);
    void new_trace(Profiler_backend_trace &trace);
    Profiler_backend* backend;
    QMap<int, QString> traceMap;
    std::vector<int> traceId;
    std::vector<int> getIds();
    bool enoughTraces = false;
    bool canGetView() {return enoughTraces;};
};

std::vector<int> Profiler_backend_user::getIds() {
    return traceId;
}

class MyWidget: public QWidget
{
    Q_OBJECT
        public:
            MyWidget(Profiler_backend_user* backendUserRef);
            //~MyWidget();
            void updateMe(std::vector<std::vector<Profiler_backend_slot_info>> &trace_slots, int64_t start_timestamp, int64_t end_timestamp, int nb_slots);
        private: 
            Profiler_backend_user* backendUser;
            bool firstData=true;
            int64_t t0=0; // first window point timestamp
            int64_t t1=0; // last window point timestamp
            void paintEvent(QPaintEvent *event);
            QTimer timer;
        private slots:
            void getData();
            
            
};

void Profiler_backend_user::start(std::string gvsoc_config)
{
    std::cout << "start " << gvsoc_config << std::endl;
    backend = new Profiler_backend();

    backend->open(this, gvsoc_config);

}


void Profiler_backend_user::new_trace(Profiler_backend_trace &trace)
{   
    //std::cout << "new_trace" << std::endl;
    
    traceMap.insert(trace.id, QString::fromStdString(trace.path)); // stores a trace Map 
    traceId.push_back(trace.id); // stores the ids in a vector
    if (trace.path=="/chip/soc/fc/is_active") 
        enoughTraces = true;

}


void Profiler_backend_user::update_view(void* widget, std::vector<std::vector<Profiler_backend_slot_info>> &trace_slots, 
                                        int64_t start_timestamp, 
                                        int64_t end_timestamp, 
                                        int nb_slots)
{
    std::cout << "update_View" << std::endl;
    ((MyWidget*)widget)->updateMe(trace_slots, start_timestamp, end_timestamp, nb_slots);
}



MyWidget::MyWidget(Profiler_backend_user* backendUserRef) {

    std::cout << "Create MyWidget" << std::endl;
    backendUser= backendUserRef;
    connect(&timer, SIGNAL(timeout()), this, SLOT(getData()));
    timer.start(2000);
}

void MyWidget::getData() {

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
   

   std::cout << "getData" << std::endl;
   std::vector<int> ids = backendUser->getIds();
   std::cout << "ids size " << ids.size() << std::endl;


    if (firstData)
    {
        std::cout << "firstData " << firstData << std::endl;

        backendUser->backend->get_view(this, ids, -1,-1,this->width());
        firstData=false;
        std::cout << "firstData " << firstData << std::endl;

    }
    else 
       backendUser->backend->get_view(this, ids, t0, t1,this->width()); 

    std::cout << "End getData " << std::endl;

    
}

void MyWidget::updateMe( std::vector<std::vector<Profiler_backend_slot_info>> &trace_slots, 
                    int64_t start_timestamp, 
                    int64_t end_timestamp, 
                    int nb_slots) {

    std::cout << "updateMe" << std::endl;
    //messageBox1.setText(QString::number(start_timestamp) + " " + QString::number(end_timestamp) + 
    //                            " nb_slots" + QString::number(nb_slots));
    std::cout << "start: " << start_timestamp << " end: " << end_timestamp << " nb_slots " << nb_slots << " " << trace_slots.size()<< std::endl;
    t0=start_timestamp;
    t1=end_timestamp;

    for (int i = 0; i < trace_slots.size(); i++)
    {
        for (int j = 0; j < trace_slots[i].size(); j++)
        {
            std::cout << trace_slots[i][j].value;
        }
        std::cout << std::endl;
    }

   // Here, signals must be painted on the screen ..

}

void MyWidget::paintEvent(QPaintEvent *event)
{
    std::cout << "paintEvent" << std::endl;
    getData();
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    std::cout << "Hello from Profiler" << std::endl;

    Profiler_backend_user *backend = new Profiler_backend_user();

    std::cout << " starting backend " << std::endl;
    backend->start(argv[1]);
    std::cout << "Afer starting backend " << std::endl;

    MyWidget* mywidget = new MyWidget(backend);
    mywidget->resize(700,500);

    //mywidget->activateWindow();
    mywidget->show();   

    std::cout << "Showed widget" << std::endl;

    int result = app.exec();
    std::cout << "result " << result << std::endl;
    std::cout << "Graphic application launched correctly" << std::endl;

    return result;

}


#include "helloworld.moc"
