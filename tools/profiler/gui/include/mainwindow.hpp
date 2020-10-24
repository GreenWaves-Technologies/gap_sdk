/*
 * Copyright (C) 2019 GreenWaves Technologies
 * All rights reserved.
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
#include "execoverview.hpp"
#include "backend_interface.hpp"
#include "dialog.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(std::string exampleDir, 
                        std::string path_to_elf, 
                        std::string configFileName);
    ~MainWindow();

private:
  
  Timeline* timeline = NULL;
  FunctionDetails* fd;
  ExecOverview* overview;
  SignalTree* signalsView;
  QPlainTextEdit* sourceCode;
  QPlainTextEdit* asmCode;
  StallChart* stallchart;
  QGroupBox* commands;

  // Dock Widgets
  QDockWidget* functionsDock;
  QDockWidget* stallChartDock;
  QDockWidget* sourceCodeDock;
  QDockWidget* asmCodeDock;
  QDockWidget* overviewDock;
  QDockWidget* signalsDock;
  QDockWidget* commandsDock;
  
  // Buttons
  QPushButton* openB;
  QPushButton* closeB;
  QPushButton* runB;
  QPushButton* pauseB;
  QPushButton* parametersB;
  QButtonGroup* gvsocModeGroup; 

  // Buttons for switching signals On or Off
  QDialog* signalsDialog;
  QGroupBox *signalsBox; 
  QCheckBox *coresBox;
  QCheckBox *debugBox;
  QCheckBox *dmaBox;
  QCheckBox *stallsBox;
  QCheckBox *statisticsBox;
  QCheckBox *cachesBox;
  QCheckBox *powerBox;
  bool firstGvsocInit = true;
  
  QDialog* createNonExclusiveGroup();

  // changes color of a button
  void changeColor(QPushButton* button, QColor color);
  
  // Tool & Menu bars
  QToolBar* toolBar;
  void makeToolBar();
  QMenu *mainMenu;
  QMenu* gvsocSettingsMenu;
  void createDockWindows();
  void createMenus();
  void closeWindows();

  // path to the example directory containing its Makefile
  std::string exampleDir;
  // path to the executable of the example
  std::string path_to_elf; 
  // Gvsoc configuration file name
  std::string configFileName;
  
  // Name of the FIFO file
  char const* fifoName="all.bin";

  bool dockWindowsCreated = false;
  bool gvsocRun = false;
  bool gvsocRunning = false;
  bool gvsocOpened = false;
  bool signalsAdded = false;

  // Gvsoc Proxy
  Gvsoc_proxy *gvsoc;

  // Dialog Widget
  Dialog dialog;

// Gvsoc signals for fast Gvsoc Mode
std::array<std::string, 58>  gvsocSignals = std::array<std::string, 58> {
  std::string("/sys/board/chip/soc/fc/state@all.bin"),
  std::string("/sys/board/chip/soc/udma/spim0_rx/state@all.bin"),
  std::string("/sys/board/chip/soc/udma/spim0_tx/state@all.bin"),
  std::string("/sys/board/chip/soc/udma/spim1_rx/state@all.bin"),
  std::string("/sys/board/chip/soc/udma/spim1_tx/state@all.bin"),
  std::string("/sys/board/chip/soc/udma/hyper0_rx/state@all.bin"),
  std::string("/sys/board/chip/soc/udma/hyper0_tx/state@all.bin"),
  std::string("/sys/board/chip/soc/udma/i2c0_rx/state@all.bin"),
  std::string("/sys/board/chip/soc/udma/i2c0_tx/state@all.bin"),
  std::string("/sys/board/chip/soc/udma/i2c1_rx/state@all.bin"),
  std::string("/sys/board/chip/soc/udma/i2c1_tx/state@all.bin"),
  std::string("/sys/board/chip/soc/udma/i2s0_rx/state@all.bin"),
  std::string("/sys/board/chip/soc/udma/i2s0_tdm_0/state@all.bin"),
  std::string("/sys/board/chip/soc/udma/i2s0_tdm_1/state@all.bin"),
  std::string("/sys/board/chip/soc/udma/i2s0_tdm_2/state@all.bin"),
  std::string("/sys/board/chip/soc/udma/i2s0_tdm_3/state@all.bin"),
  std::string("/sys/board/chip/soc/udma/i2s0_tdm_4/state@all.bin"),
  std::string("/sys/board/chip/soc/udma/i2s0_tdm_5/state@all.bin"),
  std::string("/sys/board/chip/soc/udma/i2s0_tdm_6/state@all.bin"),
  std::string("/sys/board/chip/soc/udma/i2s0_tdm_7/state@all.bin"),
  std::string("/sys/board/chip/soc/udma/uart0_rx/state@all.bin"),
  std::string("/sys/board/chip/soc/udma/uart0_tx/state@all.bin"),
  std::string("/sys/board/chip/soc/udma/cpi0_rx/state@all.bin"),
  std::string("/sys/board/chip/soc_clock/period@all.bin"),
  std::string("/sys/board/chip/cluster/pe0/state@all.bin"),
  std::string("/sys/board/chip/cluster/pe1/state@all.bin"),
  std::string("/sys/board/chip/cluster/pe2/state@all.bin"),
  std::string("/sys/board/chip/cluster/pe3/state@all.bin"),
  std::string("/sys/board/chip/cluster/pe4/state@all.bin"),
  std::string("/sys/board/chip/cluster/pe5/state@all.bin"),
  std::string("/sys/board/chip/cluster/pe6/state@all.bin"),
  std::string("/sys/board/chip/cluster/pe7/state@all.bin"),
  std::string("/sys/board/chip/cluster/dma/channel_0@all.bin"),
  std::string("/sys/board/chip/cluster/dma/channel_1@all.bin"),
  std::string("/sys/board/chip/cluster/dma/channel_2@all.bin"),
  std::string("/sys/board/chip/cluster/dma/channel_3@all.bin"),
  std::string("/sys/board/chip/cluster/dma/channel_4@all.bin"),
  std::string("/sys/board/chip/cluster/dma/channel_5@all.bin"),
  std::string("/sys/board/chip/cluster/dma/channel_6@all.bin"),
  std::string("/sys/board/chip/cluster/dma/channel_7@all.bin"),
  std::string("/sys/board/chip/cluster/dma/channel_8@all.bin"),
  std::string("/sys/board/chip/cluster/dma/channel_9@all.bin"),
  std::string("/sys/board/chip/cluster/dma/channel_10@all.bin"),
  std::string("/sys/board/chip/cluster/dma/channel_11@all.bin"),
  std::string("/sys/board/chip/cluster/dma/channel_12@all.bin"),
  std::string("/sys/board/chip/cluster/dma/channel_13@all.bin"),
  std::string("/sys/board/chip/cluster/dma/channel_14@all.bin"),
  std::string("/sys/board/chip/cluster/dma/channel_15@all.bin"),
  std::string("/sys/board/chip/cluster_clock/period@all.bin"),
  std::string("/sys/board/chip/cluster_clock/cycles"),
  std::string("/sys/board/chip/cluster/pe0/ipc_stat@all.bin"),
  std::string("/sys/board/chip/cluster/pe1/ipc_stat@all.bin"),
  std::string("/sys/board/chip/cluster/pe2/ipc_stat@all.bin"),
  std::string("/sys/board/chip/cluster/pe3/ipc_stat@all.bin"),
  std::string("/sys/board/chip/cluster/pe4/ipc_stat@all.bin"),
  std::string("/sys/board/chip/cluster/pe5/ipc_stat@all.bin"),
  std::string("/sys/board/chip/cluster/pe6/ipc_stat@all.bin"),
  std::string("/sys/board/chip/cluster/pe7/ipc_stat@all.bin")
};

// Gvsoc always active Core signals Table
std::array<std::string, 11>  coresSig = std::array<std::string, 11> {
  std::string("/sys/board/chip/soc/state"),
  std::string("/sys/board/chip/soc/fc/state"),
  std::string("/sys/board/chip/cluster/state"),
  std::string("/sys/board/chip/cluster/pe0/state"),
  std::string("/sys/board/chip/cluster/pe1/state"),
  std::string("/sys/board/chip/cluster/pe2/state"),
  std::string("/sys/board/chip/cluster/pe3/state"),
  std::string("/sys/board/chip/cluster/pe4/state"),
  std::string("/sys/board/chip/cluster/pe5/state"),
  std::string("/sys/board/chip/cluster/pe6/state"),
  std::string("/sys/board/chip/cluster/pe7/state")
};

// Gvsoc debug symbols signals Table
// signal integer 1-32 bits but different from the others 
std::array<std::string, 9>  debugSymbolsSig = std::array<std::string, 9> {
  std::string("/sys/board/chip/soc/fc/pc"),
  std::string("/sys/board/chip/cluster/pe0/pc"),
  std::string("/sys/board/chip/cluster/pe1/pc"),
  std::string("/sys/board/chip/cluster/pe2/pc"),
  std::string("/sys/board/chip/cluster/pe3/pc"),
  std::string("/sys/board/chip/cluster/pe4/pc"),
  std::string("/sys/board/chip/cluster/pe5/pc"),
  std::string("/sys/board/chip/cluster/pe6/pc"),
  std::string("/sys/board/chip/cluster/pe7/pc")
};


// Gvsoc Dma Symbols Signals Table
std::array<std::string, 29>  dmaSig = std::array<std::string, 29> {
  std::string("/sys/board/chip/soc/udma/spim0_rx/state"),
  std::string("/sys/board/chip/soc/udma/spim0_tx/state"),
  std::string("/sys/board/chip/soc/udma/spim1_rx/state"),
  std::string("/sys/board/chip/soc/udma/spim1_tx/state"),
  std::string("/sys/board/chip/soc/udma/hyper0_rx/state"),
  std::string("/sys/board/chip/soc/udma/hyper0_tx/state"),
  std::string("/sys/board/chip/soc/udma/i2c0_rx/state"),
  std::string("/sys/board/chip/soc/udma/i2c0_tx/state"),
  std::string("/sys/board/chip/soc/udma/i2c1_rx/state"),
  std::string("/sys/board/chip/soc/udma/i2c1_tx/state"),
  std::string("/sys/board/chip/soc/udma/uart0_rx/state"),
  std::string("/sys/board/chip/soc/udma/uart0_tx/state"),
  std::string("/sys/board/chip/soc/udma/cpi0_rx/state"),
  std::string("/sys/board/chip/cluster/dma/channel_0"),
  std::string("/sys/board/chip/cluster/dma/channel_1"),
  std::string("/sys/board/chip/cluster/dma/channel_2"),
  std::string("/sys/board/chip/cluster/dma/channel_3"),
  std::string("/sys/board/chip/cluster/dma/channel_4"),
  std::string("/sys/board/chip/cluster/dma/channel_5"),
  std::string("/sys/board/chip/cluster/dma/channel_6"),
  std::string("/sys/board/chip/cluster/dma/channel_7"),
  std::string("/sys/board/chip/cluster/dma/channel_8"),
  std::string("/sys/board/chip/cluster/dma/channel_9"),
  std::string("/sys/board/chip/cluster/dma/channel_10"),
  std::string("/sys/board/chip/cluster/dma/channel_11"),
  std::string("/sys/board/chip/cluster/dma/channel_12"),
  std::string("/sys/board/chip/cluster/dma/channel_13"),
  std::string("/sys/board/chip/cluster/dma/channel_14"),
  std::string("/sys/board/chip/cluster/dma/channel_15")
};

// Gvsoc Stalls Signals Table
std::array<std::string, 154>  stallsSig  = std::array<std::string, 154> {
  std::string("/sys/board/chip/soc/fc/pcer_cycles"),
  std::string("/sys/board/chip/soc/fc/pcer_instr"),
  std::string("/sys/board/chip/soc/fc/pcer_ld_stall"),
  std::string("/sys/board/chip/soc/fc/pcer_jmp_stall"),
  std::string("/sys/board/chip/soc/fc/pcer_imiss"),
  std::string("/sys/board/chip/soc/fc/pcer_ld"),
  std::string("/sys/board/chip/soc/fc/pcer_st"),
  std::string("/sys/board/chip/soc/fc/pcer_jump"),
  std::string("/sys/board/chip/soc/fc/pcer_branch"),
  std::string("/sys/board/chip/soc/fc/pcer_taken_branch"),
  std::string("/sys/board/chip/soc/fc/pcer_rvc"),
  std::string("/sys/board/chip/soc/fc/pcer_ld_ext"),
  std::string("/sys/board/chip/soc/fc/pcer_st_ext"),
  std::string("/sys/board/chip/soc/fc/pcer_ld_ext_cycles"),
  std::string("/sys/board/chip/soc/fc/pcer_st_ext_cycles"),
  std::string("/sys/board/chip/soc/fc/pcer_tcdm_cont"),
  std::string("/sys/board/chip/soc/fc/misaligned"),
  std::string("/sys/board/chip/cluster/pe0/pcer_cycles"),
  std::string("/sys/board/chip/cluster/pe0/pcer_instr"),
  std::string("/sys/board/chip/cluster/pe0/pcer_ld_stall"),
  std::string("/sys/board/chip/cluster/pe0/pcer_jmp_stall"),
  std::string("/sys/board/chip/cluster/pe0/pcer_imiss"),
  std::string("/sys/board/chip/cluster/pe0/pcer_ld"),
  std::string("/sys/board/chip/cluster/pe0/pcer_st"),
  std::string("/sys/board/chip/cluster/pe0/pcer_jump"),
  std::string("/sys/board/chip/cluster/pe0/pcer_branch"),
  std::string("/sys/board/chip/cluster/pe0/pcer_taken_branch"),
  std::string("/sys/board/chip/cluster/pe0/pcer_rvc"),
  std::string("/sys/board/chip/cluster/pe0/pcer_ld_ext"),
  std::string("/sys/board/chip/cluster/pe0/pcer_st_ext"),
  std::string("/sys/board/chip/cluster/pe0/pcer_ld_ext_cycles"),
  std::string("/sys/board/chip/cluster/pe0/pcer_st_ext_cycles"),
  std::string("/sys/board/chip/cluster/pe0/pcer_tcdm_cont"),
  std::string("/sys/board/chip/cluster/pe0/misaligned"),
  std::string("/sys/board/chip/cluster/pe1/pcer_cycles"),
  std::string("/sys/board/chip/cluster/pe1/pcer_instr"),
  std::string("/sys/board/chip/cluster/pe1/pcer_ld_stall"),
  std::string("/sys/board/chip/cluster/pe1/pcer_jmp_stall"),
  std::string("/sys/board/chip/cluster/pe1/pcer_imiss"),
  std::string("/sys/board/chip/cluster/pe1/pcer_ld"),
  std::string("/sys/board/chip/cluster/pe1/pcer_st"),
  std::string("/sys/board/chip/cluster/pe1/pcer_jump"),
  std::string("/sys/board/chip/cluster/pe1/pcer_branch"),
  std::string("/sys/board/chip/cluster/pe1/pcer_taken_branch"),
  std::string("/sys/board/chip/cluster/pe1/pcer_rvc"),
  std::string("/sys/board/chip/cluster/pe1/pcer_ld_ext"),
  std::string("/sys/board/chip/cluster/pe1/pcer_st_ext"),
  std::string("/sys/board/chip/cluster/pe1/pcer_ld_ext_cycles"),
  std::string("/sys/board/chip/cluster/pe1/pcer_st_ext_cycles"),
  std::string("/sys/board/chip/cluster/pe1/pcer_tcdm_cont"),
  std::string("/sys/board/chip/cluster/pe1/misaligned"),
  std::string("/sys/board/chip/cluster/pe2/pcer_cycles"),
  std::string("/sys/board/chip/cluster/pe2/pcer_instr"),
  std::string("/sys/board/chip/cluster/pe2/pcer_ld_stall"),
  std::string("/sys/board/chip/cluster/pe2/pcer_jmp_stall"),
  std::string("/sys/board/chip/cluster/pe2/pcer_imiss"),
  std::string("/sys/board/chip/cluster/pe2/pcer_ld"),
  std::string("/sys/board/chip/cluster/pe2/pcer_st"),
  std::string("/sys/board/chip/cluster/pe2/pcer_jump"),
  std::string("/sys/board/chip/cluster/pe2/pcer_branch"),
  std::string("/sys/board/chip/cluster/pe2/pcer_taken_branch"),
  std::string("/sys/board/chip/cluster/pe2/pcer_rvc"),
  std::string("/sys/board/chip/cluster/pe2/pcer_ld_ext"),
  std::string("/sys/board/chip/cluster/pe2/pcer_st_ext"),
  std::string("/sys/board/chip/cluster/pe2/pcer_ld_ext_cycles"),
  std::string("/sys/board/chip/cluster/pe2/pcer_st_ext_cycles"),
  std::string("/sys/board/chip/cluster/pe2/pcer_tcdm_cont"),
  std::string("/sys/board/chip/cluster/pe2/misaligned"),
  std::string("/sys/board/chip/cluster/pe3/pcer_cycles"),
  std::string("/sys/board/chip/cluster/pe3/pcer_instr"),
  std::string("/sys/board/chip/cluster/pe3/pcer_ld_stall"),
  std::string("/sys/board/chip/cluster/pe3/pcer_jmp_stall"),
  std::string("/sys/board/chip/cluster/pe3/pcer_imiss"),
  std::string("/sys/board/chip/cluster/pe3/pcer_ld"),
  std::string("/sys/board/chip/cluster/pe3/pcer_st"),
  std::string("/sys/board/chip/cluster/pe3/pcer_jump"),
  std::string("/sys/board/chip/cluster/pe3/pcer_branch"),
  std::string("/sys/board/chip/cluster/pe3/pcer_taken_branch"),
  std::string("/sys/board/chip/cluster/pe3/pcer_rvc"),
  std::string("/sys/board/chip/cluster/pe3/pcer_ld_ext"),
  std::string("/sys/board/chip/cluster/pe3/pcer_st_ext"),
  std::string("/sys/board/chip/cluster/pe3/pcer_ld_ext_cycles"),
  std::string("/sys/board/chip/cluster/pe3/pcer_st_ext_cycles"),
  std::string("/sys/board/chip/cluster/pe3/pcer_tcdm_cont"),
  std::string("/sys/board/chip/cluster/pe3/misaligned"),
  std::string("/sys/board/chip/cluster/pe4/pcer_cycles"),
  std::string("/sys/board/chip/cluster/pe4/pcer_instr"),
  std::string("/sys/board/chip/cluster/pe4/pcer_ld_stall"),
  std::string("/sys/board/chip/cluster/pe4/pcer_jmp_stall"),
  std::string("/sys/board/chip/cluster/pe4/pcer_imiss"),
  std::string("/sys/board/chip/cluster/pe4/pcer_ld"),
  std::string("/sys/board/chip/cluster/pe4/pcer_st"),
  std::string("/sys/board/chip/cluster/pe4/pcer_jump"),
  std::string("/sys/board/chip/cluster/pe4/pcer_branch"),
  std::string("/sys/board/chip/cluster/pe4/pcer_taken_branch"),
  std::string("/sys/board/chip/cluster/pe4/pcer_rvc"),
  std::string("/sys/board/chip/cluster/pe4/pcer_ld_ext"),
  std::string("/sys/board/chip/cluster/pe4/pcer_st_ext"),
  std::string("/sys/board/chip/cluster/pe4/pcer_ld_ext_cycles"),
  std::string("/sys/board/chip/cluster/pe4/pcer_st_ext_cycles"),
  std::string("/sys/board/chip/cluster/pe4/pcer_tcdm_cont"),
  std::string("/sys/board/chip/cluster/pe4/misaligned"),
  std::string("/sys/board/chip/cluster/pe5/pcer_cycles"),
  std::string("/sys/board/chip/cluster/pe5/pcer_instr"),
  std::string("/sys/board/chip/cluster/pe5/pcer_ld_stall"),
  std::string("/sys/board/chip/cluster/pe5/pcer_jmp_stall"),
  std::string("/sys/board/chip/cluster/pe5/pcer_imiss"),
  std::string("/sys/board/chip/cluster/pe5/pcer_ld"),
  std::string("/sys/board/chip/cluster/pe5/pcer_st"),
  std::string("/sys/board/chip/cluster/pe5/pcer_jump"),
  std::string("/sys/board/chip/cluster/pe5/pcer_branch"),
  std::string("/sys/board/chip/cluster/pe5/pcer_taken_branch"),
  std::string("/sys/board/chip/cluster/pe5/pcer_rvc"),
  std::string("/sys/board/chip/cluster/pe5/pcer_ld_ext"),
  std::string("/sys/board/chip/cluster/pe5/pcer_st_ext"),
  std::string("/sys/board/chip/cluster/pe5/pcer_ld_ext_cycles"),
  std::string("/sys/board/chip/cluster/pe5/pcer_st_ext_cycles"),
  std::string("/sys/board/chip/cluster/pe5/pcer_tcdm_cont"),
  std::string("/sys/board/chip/cluster/pe5/misaligned"),
  std::string("/sys/board/chip/cluster/pe6/pcer_cycles"),
  std::string("/sys/board/chip/cluster/pe6/pcer_instr"),
  std::string("/sys/board/chip/cluster/pe6/pcer_ld_stall"),
  std::string("/sys/board/chip/cluster/pe6/pcer_jmp_stall"),
  std::string("/sys/board/chip/cluster/pe6/pcer_imiss"),
  std::string("/sys/board/chip/cluster/pe6/pcer_ld"),
  std::string("/sys/board/chip/cluster/pe6/pcer_st"),
  std::string("/sys/board/chip/cluster/pe6/pcer_jump"),
  std::string("/sys/board/chip/cluster/pe6/pcer_branch"),
  std::string("/sys/board/chip/cluster/pe6/pcer_taken_branch"),
  std::string("/sys/board/chip/cluster/pe6/pcer_rvc"),
  std::string("/sys/board/chip/cluster/pe6/pcer_ld_ext"),
  std::string("/sys/board/chip/cluster/pe6/pcer_st_ext"),
  std::string("/sys/board/chip/cluster/pe6/pcer_ld_ext_cycles"),
  std::string("/sys/board/chip/cluster/pe6/pcer_st_ext_cycles"),
  std::string("/sys/board/chip/cluster/pe6/pcer_tcdm_cont"),
  std::string("/sys/board/chip/cluster/pe6/misaligned"),
  std::string("/sys/board/chip/cluster/pe7/pcer_cycles"),
  std::string("/sys/board/chip/cluster/pe7/pcer_instr"),
  std::string("/sys/board/chip/cluster/pe7/pcer_ld_stall"),
  std::string("/sys/board/chip/cluster/pe7/pcer_jmp_stall"),
  std::string("/sys/board/chip/cluster/pe7/pcer_imiss"),
  std::string("/sys/board/chip/cluster/pe7/pcer_ld"),
  std::string("/sys/board/chip/cluster/pe7/pcer_st"),
  std::string("/sys/board/chip/cluster/pe7/pcer_jump"),
  std::string("/sys/board/chip/cluster/pe7/pcer_branch"),
  std::string("/sys/board/chip/cluster/pe7/pcer_taken_branch"),
  std::string("/sys/board/chip/cluster/pe7/pcer_rvc"),
  std::string("/sys/board/chip/cluster/pe7/pcer_ld_ext"),
  std::string("/sys/board/chip/cluster/pe7/pcer_st_ext"),
  std::string("/sys/board/chip/cluster/pe7/pcer_ld_ext_cycles"),
  std::string("/sys/board/chip/cluster/pe7/pcer_st_ext_cycles"),
  std::string("/sys/board/chip/cluster/pe7/pcer_tcdm_cont"),
  std::string("/sys/board/chip/cluster/pe7/misaligned"),
  std::string("/sys/board/chip/cluster_clock/cycles")
};

void switchSignalsGroup(auto &signalsTable, bool state);


private slots:
  void foo();
  void handleOpenB();
  void handleCloseB();
  void handleRunB();
  void handlePauseB();
  void handleParametersB();
  void initGvsoc();
  void switchCoresSig(bool state);
  void switchDebugSig(bool state);
  void switchDmaSig(bool state);
  void switchStallsSig(bool state);
};

#endif // MAINWINDOW_HPP
