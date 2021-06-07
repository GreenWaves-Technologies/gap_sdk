/*
 * Copyright (C) 2020  GreenWaves Technologies, SAS
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#include <QApplication>
#include <QtGlobal>

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
using namespace std;
#include <unistd.h>
#include <stdio.h>
#include <QtDebug>

#include "mainwindow.hpp"
#include "backend_interface.hpp"

FILE * debugFile;

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        fprintf(stdout, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtInfoMsg:
        fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        abort();
    }
}

int main(int argc, char *argv[])
{
    
  // Init debug file
    debugFile = fopen ("debug.txt","w");
   
    if (argc != 4){
      printf("Usage: ./profiler <example directory > <executable> <configuration file>\n");
      printf("\t<example directory> is the directory of your example \n");
      printf("\t<executable> is the program you want to profile\n");
      printf("\t<configuration file> is the Gvsoc configuration file\n");
      return -1;
    }
    
    qInstallMessageHandler(myMessageOutput); // Install the handler

    qDebug() << "[-] Entering Profiler Main Application";
    QApplication a(argc, argv);
    MainWindow w(argv[1], argv[2], argv[3]); // passing example directory & binary file
    w.show();
    int x = a.exec();
    
    // close debug file
    qDebug() << "[-] Ending Profiler Main Application";
    fclose(debugFile);
    
    return x;
}

