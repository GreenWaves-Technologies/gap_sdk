#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "profiler_backend_user.h"



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(Profiler_backend_user* backendUserRef, QWidget *parent = 0);
    ~MainWindow();

private:
    //Ui::MainWindow *ui;
    Profiler_backend_user* backendUser;
};

#endif // MAINWINDOW_H
