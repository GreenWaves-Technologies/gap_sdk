#include "mainwindow.h"
#include <QApplication>
#include "profiler_backend_user.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Profiler_backend_user *backend = new Profiler_backend_user();
    std::cout << " starting backend " << std::endl;
    backend->start(argv[1]);
    std::cout << " End starting backend " << std::endl;

    MainWindow w;
    std::cout << " show mainwindow " << std::endl;
    w.show();
    std::cout << " End show mainwindow " << std::endl;

    return a.exec();

    std::cout << " End Main" << std::endl;
}
