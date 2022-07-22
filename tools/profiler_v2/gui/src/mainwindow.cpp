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

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <cstdlib>
#include <QDockWidget>
#include <QPalette>
#include <QColor>
#include <QDebug>
#include <QDesktopWidget>
#include <QMessageBox>
#include "mainwindow.hpp"

//#include "functiondetails.hpp"
#include "hotspotswidget.hpp"
#include "hotspotsfunctionwidget.hpp"
#include "hotspotsfilewidget.hpp"
#include "hotspotslinewidget.hpp"
#include "stallchart.hpp"
#include <QAction>
#include <QtDebug>
#include <assert.h>

#include "profiler_backend.hpp"
#include <unistd.h>
#include "codeeditorwrap.hpp"


using std::cout; using std::getenv;
using std::string; using std::cerr;
using std::endl;

const char *ENV_VAR = "GAP_SDK_HOME";

void MainWindow::makeToolBar(){

    toolBar = this->addToolBar(tr("File"));

    const char *tmp = getenv("GAP_SDK_HOME");
    string env_var(tmp ? tmp : "");
    if (env_var.empty()) {
        QMessageBox::information(this,"[ERROR] Environment variable not found:", QString("GAP_SDK_HOME") );
        exit(EXIT_FAILURE);
    }

    // Add the GvSoc command buttons
    QString closeIconFile = QString::fromStdString(env_var) +  "/tools/profiler_v2/gui/images/Stop-red-icon.png";
    QString runIconFile = QString::fromStdString(env_var) + "/tools/profiler_v2/gui/images/Start-icon.png";
    QString pauseIconFile = QString::fromStdString(env_var) + "/tools/profiler_v2/gui/images/pause-icon.png";

    runB = new QPushButton(QIcon(runIconFile),"");
    pauseB = new QPushButton(QIcon(pauseIconFile),"");
    closeB = new QPushButton(QIcon(closeIconFile),"");

    runB->setToolTip(QString("Run Gvsoc"));
    pauseB->setToolTip(QString("Pause Gvsoc"));
    closeB->setToolTip(QString("Close Gvsoc"));


    runB->update();
    pauseB->update();
    closeB->update();


    connect(runB, SIGNAL (released()), this, SLOT (handleRunB()));
    connect(pauseB, SIGNAL (released()), this, SLOT (handlePauseB()));
    connect(closeB, SIGNAL (released()), this, SLOT (handleCloseB()));

    //toolBar->addWidget(openB);
    toolBar->addWidget(runB);
    toolBar->addWidget(pauseB);
    toolBar->addWidget(closeB);

    update();
}


void MainWindow::switchSignalsGroup(QString groupName, bool state){
    //std::vector<std::string> &signalsTable
    qDebug() << "[-] Switching Signals for group " << groupName.toStdString().c_str() << " " ;
    if (backend!=NULL)
        backend->signal_group_setup(groupName.toStdString(), state);
    qDebug() << "[-] Switching Signals Done";
}



void MainWindow::closeWindows(){

    // Delete Dock windows
    assert(timeline);
    //assert(overviewDock);
    assert(sourceCodeDock);
    //assert(asmCodeDock);
    //assert(stallChartDock);
    assert(hotspotsDock);
    assert(hotspotsFunctionDock);
    assert(hotspotsFileDock);
    assert(hotspotsLineDock);
    delete timeline;
    //delete overviewDock;
    delete sourceCodeDock;
    //delete asmCodeDock;
    //delete stallChartDock;
    delete hotspotsDock;
    delete hotspotsFunctionDock;
    delete hotspotsFileDock;
    delete hotspotsLineDock;
    timeline=NULL;
    overviewDock= NULL;
    sourceCodeDock = NULL;
    asmCodeDock=NULL;
    stallChartDock = NULL;
    hotspotsDock = NULL;
    hotspotsFunctionDock = NULL;
    hotspotsFileDock = NULL;
    hotspotsLineDock = NULL;
    dockWindowsCreated=false;
}


void MainWindow::changeColor(QPushButton* button, QColor color)
{
    QPalette pal = button->palette();
    pal.setColor(QPalette::Button, QColor(color));
    button->setAutoFillBackground(true);
    button->setPalette(pal);
    button->update();
}

QDialog* MainWindow::createPcCheckBox(){

    // Create non exclusive check boxes for enabling/disabling  pc signals hotspots
    hotspotsBox = new PcCheckBox();
    // Create QDialogButtonBox
    hotspotsDialog = new QDialog();

    // Add all button boxes in a vertical layout
    QVBoxLayout* vLayout = new QVBoxLayout;
    vLayout->addWidget(hotspotsBox);

    hotspotsDialog->setLayout(vLayout);

    return hotspotsDialog;
}

QDialog* MainWindow::createNonExclusiveGroup(){

    // Create non exclusive check boxes from json file
    for (js::config *group: json->get("**/signal_groups")->get_elems())
    {
        std::string groupName = "&" + group->get("name")->get_str();
        groupBoxesMap[QString::fromStdString(group->get("name")->get_str())] = new QCheckBox(tr(groupName.c_str()));
        groupStateMap[QString::fromStdString(group->get("name")->get_str())] = group->get("enabled")->get_bool();
    }

    // Initialize Cores box to json state
    // And put them in a vertical layout
    QMapIterator<QString, QCheckBox*> i(groupBoxesMap);
    QVBoxLayout* vLayout = new QVBoxLayout;

    while (i.hasNext()) {
        i.next();
        if (groupStateMap[i.key()])
            i.value()->setCheckState(Qt::Checked);
        else
            i.value()->setCheckState(Qt::Unchecked);
        vLayout->addWidget(i.value());
        QString text=i.key();
        QObject::connect(i.value(), &QCheckBox::clicked,
                         [this,text](bool state){this->switchSignalsGroup(text, state);});
    }

    // make some checkboxes non usable
    //statisticsBox->setAttribute(Qt::WA_TransparentForMouseEvents);
    //statisticsBox->setFocusPolicy(Qt::NoFocus);


    // Create QDialogButtonBox and include the check boxes layout
    signalsDialog = new QDialog();
    signalsDialog->setLayout(vLayout);

    return signalsDialog;
}


void MainWindow::createMenus()
{
    qDebug() << "[-] Create Menus" ;
    mainMenu = menuBar()->addMenu(tr("&View"));
    gvsocSettingsMenu = menuBar()->addMenu(tr("&Settings"));
    // Create signals Dialog
    signalsDialog = createNonExclusiveGroup();
    gvsocSettingsMenu->addAction("Gvsoc Settings",signalsDialog, SLOT(exec()));
    hotspotsDialog= createPcCheckBox();
    gvsocSettingsMenu->addAction("Hotspots Settings", hotspotsDialog, SLOT(exec()));
    qDebug() << "[-] END Create Menus" ;
}

MainWindow::~MainWindow()
{
    handleCloseB();
    //closeWindows();
}

/*void MainWindow::foo(){
  get_function_table();
}
*/

void MainWindow::createDockWindows()
{
    // Create Overview Dock Window
    /*qDebug() << "[-] Create overview dock window" ;
  overviewDock = new QDockWidget(tr("Overview"), this);
  overviewDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea | Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
  overview= new ExecOverview(overviewDock);
  overviewDock->setWidget(overview);
*/

    assert(mainMenu!=NULL);
    //mainMenu->addAction(overviewDock->toggleViewAction());

    // Create Source Code Dock Window
    qDebug() << "[-] Create code window" ;
    //sourceCodeDock = new QDockWidget( this);
    sourceCodeDock = new QDockWidget(tr("Source Code"), this);
    sourceCodeDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
    sourceCodeWrap = new CodeEditorWrap(sourceCodeDock);
    sourceCodeDock->setWidget(sourceCodeWrap);
    mainMenu->addAction(sourceCodeDock->toggleViewAction());

    // create ASM Code Dock Window
    /*qDebug() << "[-] Create ASM code window" ;
  asmCodeDock = new QDockWidget(tr("ASM Code"));
  asmCodeDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
  asmCode = new QPlainTextEdit("",asmCodeDock);
  asmCode->setWordWrapMode(QTextOption::NoWrap);
  asmCode->setReadOnly(true);
  asmCodeDock->setWidget(asmCode);
  addDockWidget(Qt::BottomDockWidgetArea,asmCodeDock);
  mainMenu->addAction(asmCodeDock->toggleViewAction());
*/
    // Create StallChart Window
    /* qDebug() << "[-] Create stallchart window" ;
  stallChartDock = new QDockWidget(tr("Stall Chart"), this);
  stallChartDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::TopDockWidgetArea);
  stallchart = new StallChart(stallChartDock);
  stallChartDock->setWidget(stallchart);
  addDockWidget(Qt::RightDockWidgetArea,stallChartDock);
  stallChartDock->hide();
  mainMenu->addAction(stallChartDock->toggleViewAction());
*/

    // Create Function Details Dock Window
    /*qDebug() << "[-] Create functions Window" ;
  functionsDock = new QDockWidget(tr("Functions"), this);
  functionsDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
  fd = new FunctionDetails(functionsDock, sourceCode, asmCode, stallchart);
  functionsDock->setWidget(fd);
*/

    // Create Hotspot Function Dock Window
    qDebug() << "[-] Create Hotspots Function Dock Window" ;
    hotspotsFunctionDock = new QDockWidget(tr("Hotspots by functions"), this);
    hotspotsFunctionDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
    functionHt = new HotspotsFunctionWidget(hotspotsFunctionDock,backend,sourceCodeWrap);
    hotspotsFunctionDock->setWidget(functionHt);

    // Create Hotspot File Dock Window
    qDebug() << "[-] Create Hotspots File Dock  Window" ;
    hotspotsFileDock = new QDockWidget(tr("Hotspots by file"), this);
    hotspotsFileDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
    fileHt = new HotspotsFileWidget(hotspotsFileDock,backend,sourceCodeWrap);
    hotspotsFileDock->setWidget(fileHt);

    // Create Hotspot Line Dock Window
    qDebug() << "[-] Create Hotspots Line Dock  Window" ;
    hotspotsLineDock = new QDockWidget(tr("Hotspots by Line"), this);
    hotspotsLineDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
    lineHt = new HotspotsLineWidget(hotspotsLineDock,backend,sourceCodeWrap);
    hotspotsLineDock->setWidget(lineHt);


    // Create Hotspot Dock Window
    qDebug() << "[-] Create Hotspots Dock Window" ;
    hotspotsDock = new QDockWidget(tr("Hotspots"), this);
    hotspotsDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
    ht = new HotspotsWidget(hotspotsDock,backend,sourceCodeWrap,
                            functionHt,fileHt,lineHt);
    hotspotsDock->setWidget(ht);


    addDockWidget(Qt::BottomDockWidgetArea, hotspotsDock);
    this->tabifyDockWidget(hotspotsDock, hotspotsFunctionDock);
    this->tabifyDockWidget(hotspotsDock, hotspotsFileDock);
    this->tabifyDockWidget(hotspotsDock, hotspotsLineDock);

    addDockWidget(Qt::BottomDockWidgetArea,sourceCodeDock);

    QObject::connect(hotspotsDock, SIGNAL(visibilityChanged(bool)),
                     ht , SLOT(handleVisibilityChanged(bool)) );
    QObject::connect(hotspotsFunctionDock, SIGNAL(visibilityChanged(bool)),
                     functionHt , SLOT(handleVisibilityChanged(bool)) );
    QObject::connect(hotspotsFileDock, SIGNAL(visibilityChanged(bool)),
                     fileHt , SLOT(handleVisibilityChanged(bool)) );
    QObject::connect(hotspotsLineDock, SIGNAL(visibilityChanged(bool)),
                     lineHt , SLOT(handleVisibilityChanged(bool)) );
    mainMenu->addAction(hotspotsDock->toggleViewAction());
    mainMenu->addAction(hotspotsFunctionDock->toggleViewAction());
    mainMenu->addAction(hotspotsFileDock->toggleViewAction());
    mainMenu->addAction(hotspotsLineDock->toggleViewAction());

    //mainMenu->addAction(hotspotsFileDock->toggleViewAction());
    dockWindowsCreated=true;

    // add signalsStatAction to pilote visibility of SignalsTableView
    /*signalsStatAction = new QAction(tr("&Signals Statistics"), this);
  signalsStatAction->setCheckable(true);
  signalsStatAction->setChecked(true);
  mainMenu->addAction(signalsStatAction);
  connect(signalsStatAction , SIGNAL(triggered()), this, SLOT(signalsStatActionChecked()));
    */
}

void MainWindow::signalsStatActionChecked() {
    // called when the "signals statistics" table action has been triggered
    return;
    /*if(signalsStatAction->isChecked())
    timeline->changeSignalsStatVisibility(true);
  else
    timeline->changeSignalsStatVisibility(false);
    */
}

void MainWindow::handleCloseB()
{
    // Handles Closing Gvsoc
    qDebug() << "[-] handleCloseB " ;

    // Change button color
    if (gvsocOpened)
    {
        qDebug() << "[-] Closing Gvsoc" ;
        assert(backend);
        backend->close(); // What does it do?
        delete timeline;
        qDebug() << "[-] gvsoc Closed ";
        changeColor(runB,Qt::white);
        // closes all sub windows, including the timeline window.
        // Is this really necessary?
        //closeWindows();

    }

    // Reinitialise Booleans
    signalsAdded =false;
    gvsocRunning = false;
    gvsocOpened=false;
    timeline = NULL;
    qDebug() << "[-] END handleCloseB " ;
}

void MainWindow::handlePauseB()
{
    // Handles pausig gvsoc
    qDebug() << "[-] handlePauseB" ;
    if (gvsocOpened && gvsocRunning){
        // Change button color
        changeColor(runB,Qt::red);
        backend->stop();
        qDebug() << "[-] gvsoc paused ";
        gvsocRunning = false;
    }

    qDebug() << "[-] End handlePauseB" ;
}

void MainWindow::handleRunB()
{
    qDebug() << "[-] handleRunB" ;
    // handles running Gvsoc for first time or after a Pause or after a stop


    //assert(NULL != timeline);
    // Run GVSoc
    if (gvsocRunning) {
        QMessageBox::information(this,"[-] Gvsoc is already running", " ");
        return;
    }
    else
    {
        if (!gvsocOpened) {
            handleOpenB();
        }
        timeline->finishTimeline();
        // Change button color
        changeColor(runB,Qt::green);
        qDebug() << "[-] Start Running Gvsoc" ;
        backend->start(hotspotsBox->fcButton,hotspotsBox->pcButtons);
        qDebug() << "[-] gvsoc Running ";
        gvsocRunning = true;
        gvsocRun = true;
        timeline->updateFlags(gvsocRun);
    }
    qDebug() << "[-] signalsAdded " << signalsAdded << " " << timeline->getTLGView() ;
    if (!signalsAdded){
        timeline->getTLGView()->handleSignalNodeExpanded();
        signalsAdded=true;
    }
    qDebug() << "[-] end SignalsAdded" ;
    timeline->update();
    qDebug() << "[-] End handleRunB" ;
}


void MainWindow::handleOpenB()
{

    if (gvsocOpened)
        return;
    // Gvsoc supposed not to be opened when called
    // Open backend
    qDebug() << " Starting backend " ;
    if (signalsTreeView == NULL) {
        signalsTreeView = new SignalTree(json);
        backend->setTreeModel(signalsTreeView->model);
        backend->setSignalsTreeView(signalsTreeView);
    }
    backend->open(jsonConfigFileName, this->json,groupBoxesMap);
    if (!dockWindowsCreated) {
        qDebug() << "[-] Creating Dock Windows " ;
        createDockWindows();
        dockWindowsCreated=true;
        qDebug() << "[-] Dock Windows created" ;
    }

    if (timeline == NULL)
    {
        qDebug() << " [-] Creating new Timeline Window " ;
        timeline = new Timeline(this,
                                signalsTreeView,
                                ht, toolBar,
                                stallchart,
                                this->json,
                                backend,
                                jsonConfigFileName,
                                groupBoxesMap);
        if (timeline!=NULL ) {
            setCentralWidget(timeline);
            timeline->show();
        }
        else
            timeline->setBackend(backend);
    }


    gvsocOpened = true;
    update();
    qDebug() << "[-] End handleOpenB " ;
}

MainWindow::MainWindow( std::string jsonConfigFileName,
                        QString signalsTreeFileName,
                        Profiler_backend_user* backendUserRef,
                        js::config *json,
                        QWidget *parent):
    jsonConfigFileName(jsonConfigFileName),
    signalsTreeFileName(signalsTreeFileName),
    backend(backendUserRef),
    json(json)
{
    qDebug() << "[-] Create MainWindow" ;

    createMenus();
    setWindowTitle(tr("PROFILER"));
    makeToolBar();
    resize(QDesktopWidget().availableGeometry(this).size() * 0.7);
    if (!gvsocOpened)
        handleOpenB();
    update();

    qDebug() << "[-] End Create MainWindow" ;
}


