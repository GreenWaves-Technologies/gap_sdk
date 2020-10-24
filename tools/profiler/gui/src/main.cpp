/*
 * Copyright (C) 2019 GreenWaves Technologies
 * All rights reserved.
 */

#include <QApplication>

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
using namespace std;
#include <unistd.h>

#include "mainwindow.hpp"
#include "backend_interface.hpp"




int main(int argc, char *argv[])
{
   
    if (argc != 4){
      printf("Usage: ./profiler <example directory > <executable> <configuration file>\n");
      printf("\t<example directory> is the directory of your example \n");
      printf("\t<executable> is the program you want to profile\n");
      printf("\t<configuration file> is the Gvsoc configuration file\n");
      return -1;
    }
    
    QApplication a(argc, argv);
    MainWindow w(argv[1], argv[2], argv[3]); // passing example directory & binary file
    w.show();
    int x = a.exec();
    
    return x;
}

