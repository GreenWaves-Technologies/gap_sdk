/*
 * Copyright (C) 2019 GreenWaves Technologies
 * All rights reserved.
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



void MainWindow::makeToolBar(){

  toolBar = this->addToolBar(tr("File"));
  // Add push button for definining GVSoc Parameters
  //parametersB = new QPushButton("Set Parameters");
  //parametersB->setGeometry(QRect(QPoint(100, 100),QSize(200, 50)));
  //connect(parametersB, SIGNAL (released()), this, SLOT (handleParametersB()));
  //toolBar->addWidget(parametersB);

  
  // Add the GvSoc command buttons
  QString openIconFile = ":/images/launch-24px.png";
  QString closeIconFile = ":/images/Stop-red-icon.png";
  QString runIconFile = ":/images/Start-icon.png";
  QString pauseIconFile = ":/images/pause-icon.png";
  //openB = new QPushButton(QIcon(openIconFile),"");
  closeB = new QPushButton(QIcon(closeIconFile),"");
  runB = new QPushButton(QIcon(runIconFile),"");
  pauseB = new QPushButton(QIcon(pauseIconFile),"");
  //openB->setToolTip(QString("Open GvSoc"));
  closeB->setToolTip(QString("Close Gvsoc"));
  runB->setToolTip(QString("Run Gvsoc"));
  pauseB->setToolTip(QString("Pause Gvsoc"));
  //openB->update();
  closeB->update();
  runB->update();
  pauseB->update();

  
  connect(closeB, SIGNAL (released()), this, SLOT (handleCloseB()));
  connect(runB, SIGNAL (released()), this, SLOT (handleRunB()));
  connect(pauseB, SIGNAL (released()), this, SLOT (handlePauseB()));
  
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

/*void MainWindow::handleGvsocSlow(){
  std::cout << "[-] Switching Gvsoc to Slow Mode" << std::endl;

  if (gvsocOpened) {
    // Deactivate specific traces
    for ( auto& signalName : gvsocSignals)
      gvsoc->remove_event_regex(signalName);

    // activate everything
    gvsoc->add_event_regex(".*@all.bin"); 

    // Will specify Profiler server we are in slow mode
    setGvsocSlowMode(true);

    timeline->getTLGView()->setGvsocSlowMode(true);
  }
  
}
*/

/*void MainWindow::handleGvsocFast(){
  std::cout << "[-] Switching Gvsoc to fast Mode" << std::endl;
  std::cout << gvsocOpened << std::endl;
  if (gvsocOpened) {
    //Deactivate everything
    gvsoc->remove_event_regex(".*@all.bin"); 

    // Activate specific traces event
    for ( auto& signalName : gvsocSignals)
      gvsoc->add_event_regex(signalName);
    
    // Will specify Profiler_server we are in fast mode
    setGvsocSlowMode(false);

    timeline->getTLGView()->setGvsocSlowMode(false);
  }
  std::cout << "[-] End Switching Gvsoc to fast Mode" << std::endl;
}
*/

/*void MainWindow::initGvsocSlow(){

    std::cout << "[-] Initializing  Gvsoc to Slow Mode" << std::endl;

    // Activate Traces for Slow Mode
    gvsoc->add_event_regex(".*@all.bin"); 

    // Will specify Profiler server we are in slow mode
    setGvsocSlowMode(true);
    timeline->getTLGView()->setGvsocSlowMode(true);
}
*/

/*
void MainWindow::initGvsocFast(){
  
  std::cout << "[-] Initialising Gvsoc to fast Mode" << std::endl;
  // Activate specific traces event
  for ( auto& signalName : gvsocSignals){
    std::cout << " Signal Name " <<  signalName.c_str() << std::endl;
    gvsoc->add_event_regex(signalName);
  }
  // Will specify Profiler_server we are in fast mode
  setGvsocSlowMode(false);
  timeline->getTLGView()->setGvsocSlowMode(false);
}
*/

void MainWindow::switchSignalsGroup(auto &signalsTable, bool state){
  std::cout << "[-] Switching Signals" << std::endl;
  // Switch ON  specific traces event in Gvsoc
  for ( auto& signalName : signalsTable){
    if (state){
      std::cout << " Signal " <<  signalName.c_str() << " ON"<< std::endl;
      gvsoc->add_event_regex(signalName);
    }
    else
    {
      std::cout << " Signal " <<  signalName.c_str() << " OFF"<< std::endl;
      gvsoc->remove_event_regex(signalName);
    }  
  }
  std::cout << "[-] Switching Signals Done" << std::endl;
}

void MainWindow::initGvsoc() {

  gvsoc->add_event_regex("/sys/board/chip/cluster_clock/cycles");
  std::cout << "[-] Initialising Gvsoc Settings" << std::endl;
  if (firstGvsocInit) {
    // Set default settings with cores and debug signals ON
    switchSignalsGroup(coresSig,true);
    switchSignalsGroup(debugSymbolsSig,true);
    coresBox->setCheckState(Qt::Checked);
    debugBox->setCheckState(Qt::Checked);
    firstGvsocInit = false;
  } 
  else { 
    // Keep previous user settings
      switchSignalsGroup(coresSig,coresBox->isChecked());
      switchSignalsGroup(debugSymbolsSig,debugBox->isChecked());
      switchSignalsGroup(dmaSig,dmaBox->isChecked());
      switchSignalsGroup(stallsSig,stallsBox->isChecked());
  }

  std::cout << "[-] Gvsoc initialised" << std::endl;
}

void MainWindow::handleParametersB(){
  dialog.show();
}


  

void MainWindow::closeWindows(){
 
  // Delete Dock windows
  delete timeline;
  //delete signalsDock;
  delete overviewDock;
  delete sourceCodeDock;
  delete asmCodeDock;
  delete stallChartDock;
  delete functionsDock;
  timeline=NULL;
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
  std::cout << "Create MainWindow" << std::endl;
  createMenus();
  setWindowTitle(tr("PROFILER"));
  makeToolBar();
  resize(QDesktopWidget().availableGeometry(this).size() * 0.7);
  handleOpenB();
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
  std::cout << "[-] Create overview dock window" << std::endl;
  overviewDock = new QDockWidget(tr("Overview"), this);
  overviewDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea | Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
  overview= new ExecOverview(overviewDock);
  overviewDock->setWidget(overview);
  
  mainMenu->addAction(overviewDock->toggleViewAction());

  std::cout << "[-] Overview dock window created" << std::endl;
  
  // Create Source Code Dock Window
  std::cout << "[-] Create code window" << std::endl;
  sourceCodeDock = new QDockWidget(tr("Source Code"), this); 
  sourceCodeDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
  sourceCode = new QPlainTextEdit("",sourceCodeDock);
  sourceCode->setWordWrapMode(QTextOption::NoWrap);
  sourceCode->setReadOnly(true);
  sourceCodeDock->setWidget(sourceCode);
  mainMenu->addAction(sourceCodeDock->toggleViewAction());
  std::cout << "[-] Code window created" << std::endl;

  // create ASM Code Dock Window
  std::cout << "[-] Create ASM code window" << std::endl;
  asmCodeDock = new QDockWidget(tr("ASM Code"));
  asmCodeDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
  asmCode = new QPlainTextEdit("",asmCodeDock);
  asmCode->setWordWrapMode(QTextOption::NoWrap);
  asmCode->setReadOnly(true);
  asmCodeDock->setWidget(asmCode);
  addDockWidget(Qt::BottomDockWidgetArea,asmCodeDock);
  mainMenu->addAction(asmCodeDock->toggleViewAction());
  std::cout << "[-] ASM Code window created" << std::endl;

  // Create StallChart Window
  std::cout << "[-] Create stallchart window" << std::endl;
  stallChartDock = new QDockWidget(tr("Stall Chart"), this);
  stallChartDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::TopDockWidgetArea);
  stallchart = new StallChart(stallChartDock);
  stallChartDock->setWidget(stallchart);
  addDockWidget(Qt::RightDockWidgetArea,stallChartDock);
  stallChartDock->hide();
  mainMenu->addAction(stallChartDock->toggleViewAction());
  std::cout << "[-] Stallchart window created" << std::endl;
  
  // Create Function Details Dock Window
  std::cout << "[-] Create functions Window" << std::endl;
  functionsDock = new QDockWidget(tr("Functions"), this);
  functionsDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
  fd = new FunctionDetails(functionsDock, sourceCode, asmCode, stallchart);
  functionsDock->setWidget(fd);
  std::cout << "[-] Functions Window created" << std::endl;

  // Add all Bottom Widgets
  //std::cout << " **** Add Widgets ****" << std::endl;
  addDockWidget(Qt::BottomDockWidgetArea,overviewDock);
  overviewDock->hide();
  addDockWidget(Qt::BottomDockWidgetArea, functionsDock);
  addDockWidget(Qt::BottomDockWidgetArea,sourceCodeDock);
  asmCodeDock->hide();

  mainMenu->addAction(functionsDock->toggleViewAction());
  dockWindowsCreated=true;
  std::cout << "[-] Dock Windows created" << std::endl;
}

void MainWindow::handleCloseB()
{
  // Handles Closing Gvsoc
  std::cout << "[-] handleCloseB" << std::endl;
  std::cout << "[-] gvsocRun" << gvsocRun <<  std::endl;
  std::cout << "[-] gvsocOpened" << gvsocOpened <<  std::endl;
  // First pause 
  handlePauseB();
  /*if (gvsocRun){
    // reset flags
    mainMenu->removeAction(signalsDock->toggleViewAction());
    delete signalsDock;
  }
  */
  // Change button color
  if (gvsocOpened) 
  {
    std::cout << "[-] Closing Gvsoc" << std::endl;
    /*if (timeline != NULL)
      delete timeline;
    */
    gvsoc->close();
    changeColor(runB,Qt::white);
    
  } 
  
  // Reinitialise Booleans
  signalsAdded = false;
  signalsAdded =false;
  gvsocRunning = false;
  gvsocRun = false;
  gvsocOpened=false;
  std::cout << "[-] handleCloseB" << std::endl;
}

void MainWindow::handlePauseB()
{
  // Handles pausig gvsoc
  std::cout << "[-] handlePauseB" << std::endl;
  if (gvsocOpened){
    // Change button color
    changeColor(runB,Qt::red);
    gvsoc->pause();
    gvsocRunning = false;
  }
  std::cout << "[-] End handlePauseB" << std::endl;
}

void MainWindow::handleRunB()
{
  std::cout << "[-] handleRunB" << std::endl;
  // handles running Gvsoc for first time or after a Pause or after a stop
  if (!gvsocOpened) {
    handleOpenB();
  }

  // Run GVSoc
  if (gvsocRunning) {
    std::cout << "[-] Gvsoc is already running" << std::endl;
    return;
  }
  else 
  {
    // Change button color
    changeColor(runB,Qt::green);
    std::cout << "[-] Start Running Gvsoc" << std::endl;
    gvsoc->run();
    gvsocRunning = true;
  }

  if (!signalsAdded){
    timeline->getTLGView()->handleSignalNodeExpanded();
    signalsAdded=true;
  }
    
  timeline->update();
  gvsocRun=true;
  std::cout << "[-] End handleRunB" << std::endl;
}


void MainWindow::handleOpenB()
{
  std::cout << "[-] handleOpenB " << std::endl;
  if (gvsocOpened)
    return;
  
  if (gvsocRun) {
    closeWindows();
    gvsocRun=false;
  }
  gvsoc = new Gvsoc_proxy(configFileName);
  if (gvsoc->open()){
    QMessageBox msgBox;
    msgBox.setText("Gvsoc didn't open correctly");
    msgBox.exec();
    qDebug() << "Gvsoc didn't open correctly";
  }
  std::cout << "[-] Gvsoc process opened correctly" << std::endl;

  // re-initializing everything ?
  gvsoc->remove_event_regex(".*@all.bin");
  
  if (!gvsocOpened) {
    std::cout << "[-] Initializing Backend Process" << std::endl;
    printf("[.] Profiling @ %u KHz\n", (uint) 100000 / 1000);
    if (! init_backend("all.bin", path_to_elf, 100000)){
      QMessageBox msgBox;
      msgBox.setText("[.] Fatal error: impossible to init backend. Program exits");
      msgBox.exec();
      printf("[.] Fatal error: impossible to init backend");
      qDebug() << "Fatal error: impossible to init backend";
      exit (1);
    }
    std::cout << "[-] Backend Process initialized correctly" << std::endl;
  }
  
  if (!dockWindowsCreated) {
    std::cout << "[-] Creating Dock Windows " << std::endl;
    createDockWindows();
    dockWindowsCreated=true;
    std::cout << "[-] Dock Windows created" << std::endl;
  }
  
  if (timeline == NULL)
  {
    std::cout << " [-] Creating new Timeline Window " << std::endl;
    timeline = new Timeline(this, toolBar, fd, functionsDock, sourceCode, asmCode, stallchart);
    setCentralWidget(timeline);
    timeline->show();
    std::cout << " [-] Timeline Window Created" << std::endl;
  }
  timeline->getTLGView()->setFastHasRun(false);

  gvsocOpened = true;
  initGvsoc();
  update();
 std::cout << "[-] End handleOpenB " << std::endl;
}
