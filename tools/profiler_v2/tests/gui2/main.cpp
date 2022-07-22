#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Profiler_backend_user *backend = new Profiler_backend_user();
    std::cout << " starting backend " << std::endl;
    backend->start(argv[1]);

    MainWindow w(backend);
    w.show();

    return a.exec();
}
