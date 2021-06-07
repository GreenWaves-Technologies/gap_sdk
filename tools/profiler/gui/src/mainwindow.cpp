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
#include <QDockWidget>
#include <QPalette>
#include <QColor>
#include <QDebug>
#include <QDesktopWidget>
#include <QMessageBox>
#include "mainwindow.hpp"

#include "functiondetails.hpp"
#include "backend_interface.hpp"
#include "stallchart.hpp"
#include "execoverview.hpp"
#include <QAction>
#include <QtDebug>
#include <assert.h>


void MainWindow::makeToolBar(){

  toolBar = this->addToolBar(tr("File"));

  // Add the GvSoc command buttons
  QString closeIconFile = ":/images/Stop-red-icon.png";
  QString runIconFile = ":/images/Start-icon.png";
  QString pauseIconFile = ":/images/pause-icon.png";

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


void MainWindow::switchCoresSig(bool state){
  switchSignalsGroup(coresSig, state);
}

void MainWindow::switchDebugSig(bool state){
  switchSignalsGroup(debugSymbolsSig,state);
}

void MainWindow::switchDmaSig(bool state){
  switchSignalsGroup(dmaSig,state);
}

void MainWindow::switchStallsSig(bool state){
  switchSignalsGroup(stallsSig, state);
}


void MainWindow::switchSignalsGroup(std::vector<std::string> &signalsTable, bool state){
  qDebug() << "[-] Switching Signals";
  // Switch ON  specific traces event in Gvsoc
  for ( auto& signalName : signalsTable){
    if (state){
      qDebug() << " Signal " <<  signalName.c_str() << " ON";
      gvsoc->add_event_regex(signalName);
    }
    else
    {
      qDebug() << " Signal " <<  signalName.c_str() << " OFF";
      gvsoc->remove_event_regex(signalName);
    }
  }
  qDebug() << "[-] Switching Signals Done";
}

void MainWindow::initGvsoc() {

  gvsoc->add_event_regex("/sys/board/chip/cluster_clock/cycles");
  qDebug() << "[-] Initialising Gvsoc Settings";
  if (firstGvsocInit) {
    // Set default settings with cores and debug signals ON
    qDebug() << "[-] Default Signals Init";
    switchSignalsGroup(coresSig,true);
    switchSignalsGroup(debugSymbolsSig,true);
    coresBox->setCheckState(Qt::Checked);
    debugBox->setCheckState(Qt::Checked);
    firstGvsocInit = false;
    qDebug() << "[-] END Default Signals Init";
  }
  else {
    // Keep previous user settings
      qDebug() << "[-] Keep User Signals Init";
      switchSignalsGroup(coresSig,coresBox->isChecked());
      switchSignalsGroup(debugSymbolsSig,debugBox->isChecked());
      switchSignalsGroup(dmaSig,dmaBox->isChecked());
      switchSignalsGroup(stallsSig,stallsBox->isChecked());
      qDebug() << "[-] End Keep User Signals Init";
  }

  qDebug() << "[-] Gvsoc initialised" ;
}

//void MainWindow::handleParametersB(){
//  dialog.show();
//}

void MainWindow::closeWindows(){

  // Delete Dock windows
  assert(timeline);
  assert(overviewDock);
  assert(sourceCodeDock);
  assert(asmCodeDock);
  assert(stallChartDock);
  assert(functionsDock);
  delete timeline;
  delete overviewDock;
  delete sourceCodeDock;
  delete asmCodeDock;
  delete stallChartDock;
  delete functionsDock;
  timeline=NULL;
  overviewDock= NULL;
  sourceCodeDock = NULL;
  asmCodeDock=NULL;
  stallChartDock = NULL;
  functionsDock = NULL;
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


QDialog* MainWindow::createNonExclusiveGroup(){

  // Create non exclusive check boxes
  coresBox = new QCheckBox(tr("&Cores"));
  debugBox = new QCheckBox(tr("&Debug Symbols"));
  dmaBox = new QCheckBox(tr("&DMAs"));
  stallsBox = new QCheckBox(tr("&Stalls"));
  statisticsBox = new QCheckBox(tr("&Statistics"));
  cachesBox = new QCheckBox(tr("&Caches"));
  powerBox = new QCheckBox(tr("&Power"));

  // Initialize Cores box to state ON
  coresBox->setChecked(true);
  // Initialize all other boxes to state OFF
  debugBox->setChecked(false);
  dmaBox->setChecked(false);
  stallsBox->setChecked(false);
  statisticsBox->setChecked(false);
  cachesBox->setChecked(false);
  powerBox->setChecked(false);

  // make 3 last checkboxes non usable
  statisticsBox->setAttribute(Qt::WA_TransparentForMouseEvents);
  statisticsBox->setFocusPolicy(Qt::NoFocus);
  cachesBox->setAttribute(Qt::WA_TransparentForMouseEvents);
  cachesBox->setFocusPolicy(Qt::NoFocus);
  powerBox->setAttribute(Qt::WA_TransparentForMouseEvents);
  powerBox->setFocusPolicy(Qt::NoFocus);

  // Create QDialogButtonBox
  signalsDialog = new QDialog();

  // Add all button boxes in a vertical layout
  QVBoxLayout* vLayout = new QVBoxLayout;
  vLayout->addWidget(coresBox);
  vLayout->addWidget(debugBox);
  vLayout->addWidget(dmaBox);
  vLayout->addWidget(stallsBox);
  vLayout->addWidget(statisticsBox);
  vLayout->addWidget(cachesBox);
  vLayout->addWidget(powerBox);
  signalsDialog->setLayout(vLayout);

  // Create the connections between button triggers and slots
  connect(coresBox,SIGNAL(clicked(bool)), this, SLOT(switchCoresSig(bool)));
  connect(debugBox,SIGNAL(clicked(bool)), this, SLOT(switchDebugSig(bool)));
  connect(dmaBox,SIGNAL(clicked(bool)), this, SLOT(switchDmaSig(bool)));
  connect(stallsBox,SIGNAL(clicked(bool)), this, SLOT(switchStallsSig(bool)));

  return signalsDialog;
}


void MainWindow::createMenus()
{
  mainMenu = menuBar()->addMenu(tr("&View"));
  gvsocSettingsMenu = menuBar()->addMenu(tr("&Settings"));
  // Create signals Dialog
  signalsDialog = createNonExclusiveGroup();
  gvsocSettingsMenu->addAction("Gvsoc Settings",signalsDialog, SLOT(exec()));
}

MainWindow::MainWindow( std::string exampleDir,
                        std::string path_to_elf,
                        std::string configFileName):
      exampleDir(exampleDir),
      path_to_elf(path_to_elf),
      configFileName(configFileName)
{
  qDebug() << "[-] " ;
  qDebug() << "[-] Create MainWindow" ;
  createMenus();
  setWindowTitle(tr("PROFILER"));
  makeToolBar();
  resize(QDesktopWidget().availableGeometry(this).size() * 0.7);
  // Create gvsox proxy server
  gvsoc = new Gvsoc_proxy(configFileName);
  // Opens gvsoc Proxy server
  qDebug() << "[-] handleOpenB from MainWindow" ;
  handleOpenB();
  qDebug() << "[-] handleOpenB from MainWindow ENDED" ;
  update();
}

MainWindow::~MainWindow()
{
  handleCloseB();
  closeWindows();
}

void MainWindow::foo(){
  get_function_table();
}

void MainWindow::createDockWindows()
{
// Create Overview Dock Window
  qDebug() << "[-] Create overview dock window" ;
  overviewDock = new QDockWidget(tr("Overview"), this);
  overviewDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea | Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
  overview= new ExecOverview(overviewDock);
  overviewDock->setWidget(overview);

  mainMenu->addAction(overviewDock->toggleViewAction());

  // Create Source Code Dock Window
  qDebug() << "[-] Create code window" ;
  sourceCodeDock = new QDockWidget(tr("Source Code"), this);
  sourceCodeDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
  sourceCode = new QPlainTextEdit("",sourceCodeDock);
  sourceCode->setWordWrapMode(QTextOption::NoWrap);
  sourceCode->setReadOnly(true);
  sourceCodeDock->setWidget(sourceCode);
  mainMenu->addAction(sourceCodeDock->toggleViewAction());

  // create ASM Code Dock Window
  qDebug() << "[-] Create ASM code window" ;
  asmCodeDock = new QDockWidget(tr("ASM Code"));
  asmCodeDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
  asmCode = new QPlainTextEdit("",asmCodeDock);
  asmCode->setWordWrapMode(QTextOption::NoWrap);
  asmCode->setReadOnly(true);
  asmCodeDock->setWidget(asmCode);
  addDockWidget(Qt::BottomDockWidgetArea,asmCodeDock);
  mainMenu->addAction(asmCodeDock->toggleViewAction());

  // Create StallChart Window
  qDebug() << "[-] Create stallchart window" ;
  stallChartDock = new QDockWidget(tr("Stall Chart"), this);
  stallChartDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::TopDockWidgetArea);
  stallchart = new StallChart(stallChartDock);
  stallChartDock->setWidget(stallchart);
  addDockWidget(Qt::RightDockWidgetArea,stallChartDock);
  stallChartDock->hide();
  mainMenu->addAction(stallChartDock->toggleViewAction());

  // Create Function Details Dock Window
  qDebug() << "[-] Create functions Window" ;
  functionsDock = new QDockWidget(tr("Functions"), this);
  functionsDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
  fd = new FunctionDetails(functionsDock, sourceCode, asmCode, stallchart);
  functionsDock->setWidget(fd);

  // Add all Bottom Widgets
  //qDebug() << " **** Add Widgets ****" ;
  addDockWidget(Qt::BottomDockWidgetArea,overviewDock);
  overviewDock->hide();
  addDockWidget(Qt::BottomDockWidgetArea, functionsDock);
  addDockWidget(Qt::BottomDockWidgetArea,sourceCodeDock);
  asmCodeDock->hide();

  mainMenu->addAction(functionsDock->toggleViewAction());
  dockWindowsCreated=true;

  // add signalsStatAction to pilote visibility of SignalsTableView
  signalsStatAction = new QAction(tr("&Signals Statistics"), this);
  signalsStatAction->setCheckable(true);
  signalsStatAction->setChecked(true);
  mainMenu->addAction(signalsStatAction);
  connect(signalsStatAction , SIGNAL(triggered()), this, SLOT(signalsStatActionChecked()));

}

void MainWindow::signalsStatActionChecked() {
  // called when the "signals statistics" table action has been triggered

  if(signalsStatAction->isChecked())
    timeline->changeSignalsStatVisibility(true);
  else
    timeline->changeSignalsStatVisibility(false);
}

void MainWindow::handleCloseB()
{
  // Handles Closing Gvsoc
  qDebug() << "[-] handleCloseB " ;

  // Change button color
  if (gvsocOpened && gvsocRunning)
  {
    qDebug() << "[-] Closing Gvsoc" ;
    assert(gvsoc);
    gvsoc->close(); // What does it do?
    qDebug() << "[-] gvsoc Closed ";
    changeColor(runB,Qt::white);
    // closes all sub windows, including the timeline window.
    // Is this really necessary?
    // closeWindows();
  }

  // Reinitialise Booleans
  signalsAdded =false;
  gvsocRunning = false;
  gvsocOpened=false;
  qDebug() << "[-] END handleCloseB " ;
}

void MainWindow::handlePauseB()
{
  // Handles pausig gvsoc
  qDebug() << "[-] handlePauseB" ;
  if (gvsocOpened && gvsocRunning){
    // Change button color
    changeColor(runB,Qt::red);
    gvsoc->pause();
    qDebug() << "[-] gvsoc paused ";
    gvsocRunning = false;
  }

  qDebug() << "[-] End handlePauseB" ;
}

void MainWindow::handleRunB()
{
  qDebug() << "[-] handleRunB" ;
  // handles running Gvsoc for first time or after a Pause or after a stop


  // Run GVSoc
  if (gvsocRunning) {
    std::cout << "[-] Gvsoc is already running" << std::endl;
    return;
  }
  else
  {
    if (!gvsocOpened) {
    handleOpenB();
    }
    // Change button color
    changeColor(runB,Qt::green);
    qDebug() << "[-] Start Running Gvsoc" ;
    gvsoc->run();
    qDebug() << "[-] gvsoc Running ";
    gvsocRunning = true;
    gvsocRun = true;
    if (timeline!=NULL)
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
  qDebug() << "[-] handleOpenB2 " << "gvsocOpened " << gvsocOpened << "gvsocRunning " << gvsocRunning;
  qDebug() << "dockWindowsCreated " << dockWindowsCreated ;

  if (gvsoc->open()){
    qDebug() << "[-] Gvsoc process didn't open correctly" ;
    QMessageBox msgBox;
    msgBox.setText("Gvsoc didn't open correctly");
    msgBox.exec();
    qDebug() << "Gvsoc didn't open correctly";
  }
  qDebug() << "[-] Gvsoc process opened correctly2" ;

  // re-initializing gvsocRun
  gvsocRun = false;
  if (timeline!=NULL)
      timeline->updateFlags(gvsocRun);
  // It's crashing here obviously ... ??
  gvsoc->remove_event_regex(".*@all.bin");

   //if (!gvsocOpened) {
    qDebug() << "[-] Initializing Backend Process" ;
    printf("[.] Profiling @ %u KHz\n", (uint) 100000 / 1000);
    if (! init_backend("all.bin", path_to_elf, 100000)){
      QMessageBox msgBox;
      msgBox.setText("[.] Fatal error: impossible to init backend. Program exits");
      msgBox.exec();
      printf("[.] Fatal error: impossible to init backend");
      qDebug() << "Fatal error: impossible to init backend";
      exit (1);
    }
    qDebug() << "[-] Backend Process initialized correctly" ;
  //}

  if (!dockWindowsCreated) {
    qDebug() << "[-] Creating Dock Windows " ;
    createDockWindows();
    dockWindowsCreated=true;
    qDebug() << "[-] Dock Windows created" ;
  }

  if (timeline == NULL)
  {
    qDebug() << " [-] Creating new Timeline Window " ;
    timeline = new Timeline(this, toolBar, fd, functionsDock, sourceCode, asmCode, stallchart);
    if (timeline!=NULL ) {
      setCentralWidget(timeline);
      timeline->show();
    }
    else
       qDebug() << " [-] Timeline Window hasn't been created" ;
  }

  gvsocOpened = true;
  initGvsoc();
  update();
  qDebug() << "[-] End handleOpenB " ;
}
