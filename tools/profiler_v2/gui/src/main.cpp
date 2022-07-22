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
//#include "backend_interface.hpp"


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

    // ~/profiler_v2/build/gui/main gvsoc_config.json

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
    parser.addPositionalArgument("config", "Backend configuration file");

    QCommandLineOption signalTreeOption("signal-tree-file",
                                        "Path to the signal tree configuration file, default is profiler/gui/src/images/signalstree.txt",
                                        "file");
    parser.addOption(signalTreeOption);

    parser.process(app);

    const QStringList args = parser.positionalArguments();
    if (args.isEmpty() || args.size() < 1)
    {
        parser.showHelp();
        return 0;
    }

    QString signalsTreeFileName("/home/corine/profiler_v2/gui/images/signalstree_gap9.json");
    if (parser.isSet(signalTreeOption))
    {
        signalsTreeFileName = parser.value(signalTreeOption);
        qDebug() << "[-] Using custom signal tree " << signalsTreeFileName;
    }

    js::config *json = js::import_config_from_file(signalsTreeFileName.toStdString());

    if (json == NULL) {
        std::cout << "[-] Error: File didn't open" << signalsTreeFileName.toStdString() << std::endl;
        return -1;
    }
    std::string jsonConfigurationFile = args.at(0).toUtf8().constData();

    Profiler_backend_user *backend = new Profiler_backend_user();

    MainWindow win(
                jsonConfigurationFile,
                signalsTreeFileName,
                backend,
                json
                );

    win.show();
    int result = app.exec();
    qDebug() << "[-] Ending Profiler Main Application";

    return result;
}

