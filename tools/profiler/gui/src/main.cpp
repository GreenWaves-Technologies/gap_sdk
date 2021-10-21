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
#include <QCommandLineParser>
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

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        //fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
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
    qInstallMessageHandler(myMessageOutput); // Install the handler

    qDebug() << "[-] Entering Profiler Main Application";
    QApplication app(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription(
            "\nProfiler is a part of GWT GAP SDK and used with GVSOC, GWT Full "
            "System SoC Simulator.\n"
            "It gives a visual view of what is happening inside the chip "
            "and allows to control the simulator through a graphic interface.\n"
            "It is extremely useful for developing and debugging applications "
            "on GAP processors.");
    parser.addHelpOption();
    parser.addPositionalArgument("directory", "Program directory");
    parser.addPositionalArgument("executable", "Path of the program to profile");
    parser.addPositionalArgument("config", "GVSoC configuration file");

    QCommandLineOption signalTreeOption("signal-tree-file",
            "Path to the signal tree configuration file, default is profiler/gui/src/images/signalstree.txt",
            "file");
    parser.addOption(signalTreeOption);

    parser.process(app);

    const QStringList args = parser.positionalArguments();
    if (args.isEmpty() || args.size() < 3)
    {
        parser.showHelp();
        return 0;
    }

    QString signalsTreeFileName(":/images/signalstree.txt");
    if (parser.isSet(signalTreeOption))
    {
        signalsTreeFileName = parser.value(signalTreeOption);
        qDebug() << "[-] Using custom signal tree " << signalsTreeFileName;
    }

    std::string exampleDirectory = args.at(0).toUtf8().constData();
    std::string executablePath = args.at(1).toUtf8().constData();
    std::string gvsocConfigurationFile = args.at(2).toUtf8().constData();

    MainWindow win(
            exampleDirectory,
            executablePath,
            gvsocConfigurationFile,
            signalsTreeFileName
            );

    win.show();
    int result = app.exec();
    qDebug() << "[-] Ending Profiler Main Application";

    return result;
}

