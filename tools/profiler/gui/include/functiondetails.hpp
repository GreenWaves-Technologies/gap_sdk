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
    void switchLegendMode(LegendMode newMode);

  private :
    void updateTimeStamps(void);
    void setSourceCode(std::string fname);
    QTableWidget* table = nullptr;
    QPlainTextEdit* sourceCode;
    QPlainTextEdit* asmCode;
    QHBoxLayout* layout;
    QTimer* timer;
    std::string textVersion;
    LegendMode currentMode=TIME_MODE;

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
