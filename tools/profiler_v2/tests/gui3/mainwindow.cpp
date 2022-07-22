#include <QDesktopWidget>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "timeline.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)

{
     std::cout << "MainWindow-1" << std::endl;
    setWindowTitle("Profiler");
    resize(QDesktopWidget().availableGeometry(this).size() * 0.9);
    TimeLine* tl=new TimeLine(this);
     std::cout << "MainWindow-2" << std::endl;
    setCentralWidget(tl);
     std::cout << "MainWindow-3" << std::endl;
    tl->update();
    std::cout << "MainWindow-4" << std::endl;
    tl->show();
    std::cout << "MainWindow-5" << std::endl;

}

MainWindow::~MainWindow()
{

}
