#include <QDesktopWidget>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "timeline.h"

MainWindow::MainWindow(Profiler_backend_user* backendUserRef, QWidget *parent) :
    QMainWindow(parent)//,
   // ui(new Ui::MainWindow)
{
    //ui->setupUi(this);
    setWindowTitle("Profiler");
    resize(QDesktopWidget().availableGeometry(this).size() * 0.9);
    TimeLine* tl=new TimeLine(backendUserRef,this);
    //tl-> resize(700,500);
    setCentralWidget(tl);
    tl->update();
    tl->show();

}

MainWindow::~MainWindow()
{
    //delete ui;
}
