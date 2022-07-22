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

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QPushButton>
#include <QHBoxLayout>
#include <QWidget>
#include <QSize>
#include <QMdiArea>
#include <QSplitter>
#include <QTabWidget>
#include <QPlainTextEdit>
#include <QMenu>
#include <QMenuBar>
#include <QGroupBox>
#include <gv/gvsoc_proxy.hpp>
#include <QDialog>
#include <QGroupBox>
#include <QCheckBox>

#include "timeline.hpp"
//#include "execoverview.hpp"
//#include "backend_interface.hpp"
#include "dialog.hpp"
#include "profiler_backend_user.hpp"
#include "hotspotswidget.hpp"
#include "hotspotsfunctionwidget.hpp"
#include "hotspotsfilewidget.hpp"
#include "codeeditorwrap.hpp"
#include "pccheckbox.hpp"
#include "json.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow( std::string jsonConfigFileName,
                         QString signalsTreeFileName,
                         Profiler_backend_user* backendUserRef,
                         js::config *json,
                         QWidget *parent = 0);
    ~MainWindow();

private:

    Timeline* timeline = NULL;
    SignalTree* signalsTreeView = NULL;
    //FunctionDetails* fd;
    HotspotsWidget* ht;
    HotspotsFunctionWidget* functionHt;
    HotspotsFileWidget* fileHt;
    HotspotsLineWidget* lineHt;
    //ExecOverview* overview;
    CodeEditorWrap* sourceCodeWrap;
    QPlainTextEdit* asmCode;
    StallChart* stallchart;
    QGroupBox* commands;

    // Dock Widgets
    //QDockWidget* functionsDock;
    QDockWidget* hotspotsDock;
    QDockWidget* hotspotsFunctionDock;
    QDockWidget* hotspotsFileDock;
    QDockWidget* hotspotsLineDock;
    QDockWidget* stallChartDock;
    QDockWidget* sourceCodeDock;
    QDockWidget* asmCodeDock;
    QDockWidget* overviewDock;
    QDockWidget* signalsDock;
    QDockWidget* commandsDock;

    // Buttons
    QPushButton* closeB;
    QPushButton* runB;
    QPushButton* pauseB;
    QPushButton* parametersB;
    QButtonGroup* gvsocModeGroup;

    // Buttons for switching signals On or Off
    QDialog* signalsDialog;
    /*QGroupBox *signalsBox;
  QCheckBox *coresBox;
  QCheckBox *debugBox;
  QCheckBox *dmaBox;
  QCheckBox *stallsBox;
  QCheckBox *statisticsBox;
  QCheckBox *cachesBox;
  QCheckBox *powerBox;
  */
    QMap<QString,QCheckBox*> groupBoxesMap;
    QMap<QString,bool> groupStateMap;


    bool firstGvsocInit = true;
    QDialog* createNonExclusiveGroup();

    // Dialog winfow for enabling/disabling pc signals hotspots
    PcCheckBox* hotspotsBox;
    QDialog* hotspotsDialog;
    QDialog* createPcCheckBox();
    // changes color of a button
    void changeColor(QPushButton* button, QColor color);

    // Tool & Menu bars
    QToolBar* toolBar;
    void makeToolBar();
    QMenu *mainMenu=NULL;
    QMenu* gvsocSettingsMenu;
    QAction* signalsStatAction;
    void createDockWindows();
    void createMenus();
    void closeWindows();

    // path to the example directory containing its Makefile
    std::string exampleDir;
    // path to the executable of the example
    std::string path_to_elf;
    // Gvsoc configuration file name
    std::string configFileName;
    // Name of the txt file containing the signals tree to be uploaded
    QString signalsTreeFileName;
    // Name of .json backend configuration file
    std::string jsonConfigFileName;

    // Name of the FIFO file
    char const* fifoName="all.bin";

    bool dockWindowsCreated = false;
    bool gvsocRun = false; // gvsoc run at least once
    bool gvsocRunning = false;
    bool gvsocOpened = false;
    bool signalsAdded = false;

    // Backend
    Profiler_backend_user *backend = NULL;

    js::config *json;

    // Dialog Widget
    Dialog dialog;

    void switchSignalsGroup(QString groupName, bool state);
    void handleOpenB();

private slots:
    //void foo();

    void handleCloseB();
    void handleRunB();
    void handlePauseB();

    //void initGvsoc();
    //void switchCoresSig(bool state);
    //void switchDebugSig(bool state);
    //void switchDmaSig(bool state);
    //void switchStallsSig(bool state);
    void signalsStatActionChecked();
};

#endif // MAINWINDOW_HPP
