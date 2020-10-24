/*
 * Copyright (C) 2019 GreenWaves Technologies
 * All rights reserved.
 */

#ifndef FUNCTIONDETAILS_HPP
#define FUNCTIONDETAILS_HPP

#include <QWidget>
#include <QSplitter>
#include <QTextEdit>
#include <QTableWidget>
#include <QString>
#include <QHBoxLayout>
#include <QFile>
#include <QTimer>
#include <QPlainTextEdit>

#include "highlighter.hpp"
#include "stallchart.hpp"
#include "splitterdesign.hpp"
#include "util.hpp"

/**
  @brief displays per-function metrics, such as total execution time per
         function, stall reasons, ...
  */
class FunctionDetails : public QWidget
{
  Q_OBJECT

  static const char* columnHeader[];
  static const int nbColumn;

  public :
    FunctionDetails(QWidget* parent, 
                    QPlainTextEdit* sourceCode, 
                    QPlainTextEdit* asmCode,
                    StallChart* stallChart);
    /** in order for the app to be more accessible, table data can be exported to
        a text file (that an other program can read out to a blind person for
        example)*/
    void exportTableToTextFile(const char* filename) const;
    StallChart* stallChart;
    // switching the representation mode
    void switch2ClusterCycleMode();
    void switch2FCCycleMode();
    void switch2TimeMode();
    LegendMode currentMode=TIME_MODE;

  private :
    void setSourceCode(std::string fname);
    QTableWidget* table = nullptr;
    QPlainTextEdit* sourceCode;
    QPlainTextEdit* asmCode;
    QHBoxLayout* layout;
    QTimer* timer;
    std::string textVersion;

    Highlighter* cppHighlighter;
    Highlighter* asmHighlighter;
    std::string selectedFunction = "";

  private slots:
    void fillTable();
    QString formatTimeStamp(QString ts) const;

  public slots:
    /** selects the function for which more details are displayed */
    void selectFunction(QTableWidgetItem* it);
    void selectFunction(std::string fname);
    void selectFunction(int row);
    void selectRow(const char* name);
};

#endif //FUNCTIONDETAILS_HPP
