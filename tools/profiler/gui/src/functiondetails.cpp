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


#include <iostream>
#include <QTableWidgetItem>
#include <QAbstractItemView>
#include <QTimer>
#include <QHeaderView>
#include <QSizePolicy>
#include <string>
#include <fstream>

#include "backend_interface.hpp"
#include "functiondetails.hpp"


const char* FunctionDetails::columnHeader[] = {
  "Function name",
  "Total time or cycles",
  "calls",
  "From file"
};
const int FunctionDetails::nbColumn = \
    sizeof(FunctionDetails::columnHeader) / sizeof(FunctionDetails::columnHeader[0]);


FunctionDetails::FunctionDetails(
  QWidget* parent,
  QPlainTextEdit* sourceCode,
  QPlainTextEdit* asmCode,
  StallChart* stallChart) :
  stallChart(stallChart),
  sourceCode(sourceCode),
  asmCode(asmCode)
{
  this->setParent(parent);
  fillTable();
  sourceCode->setWordWrapMode(QTextOption::NoWrap);
  asmCode->setWordWrapMode(QTextOption::NoWrap);
  sourceCode->setReadOnly(true);
  asmCode->setReadOnly(true);

  cppHighlighter = new Highlighter(sourceCode->document());
  asmHighlighter = new Highlighter(asmCode->document());

  layout = new QHBoxLayout;
  layout->addWidget(table);
  setLayout(layout);
  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(fillTable()));
  timer->start(2000);
}

void FunctionDetails::switchLegendMode(LegendMode newMode)
{
    this->currentMode = newMode;
    this->updateTimeStamps();
}

void FunctionDetails::updateTimeStamps(void)
{
  // changes from former representation
  auto data = get_function_table();
  int T = 1;
  switch(this->currentMode)
  {
      case FC_CYCLE_MODE:
          T = get_fc_period();
          break;
      case CLUSTER_CYCLE_MODE:
          T = get_cluster_period();
          break;
      default:
          T = 1;
          break;
  }

  assert(T > 0);

  for (uint i = 0; i < data.size(); i++){
    QString time_string = formatTimeStamp(QString::number(data[i].tot_time/T));
    QTableWidgetItem* item = new QTableWidgetItem(tr("%1").arg(time_string));
    item->setTextAlignment(Qt::AlignRight);
    table->setItem(i, 1, item);
  }
  table->update();
}

QString FunctionDetails::formatTimeStamp(QString ts) const {

  const int step = 3;
  const char mychar = ',';
  for (int i = ts.length()-step; i>0; i=i-step)
      ts.insert(i, mychar);

  return ts;
}

void FunctionDetails::fillTable(){
  if (table == nullptr) table = new QTableWidget();
  QHeaderView* header = table->horizontalHeader();

  auto data = get_function_table();
  table->sizePolicy().setHorizontalPolicy(QSizePolicy::Ignored);
  table->setRowCount(data.size());
  table->setEditTriggers(QAbstractItemView::NoEditTriggers);

  table->setColumnCount(nbColumn);
  for (int i = 0; i < nbColumn; i++){
    table->setHorizontalHeaderItem(i, new QTableWidgetItem(columnHeader[i]));
  }

  connect(table, SIGNAL(itemClicked(QTableWidgetItem*)), this,
                                      SLOT(selectFunction(QTableWidgetItem*)));
  connect(table->verticalHeader(), SIGNAL(sectionClicked(int)), this,
                                      SLOT(selectFunction(int)));
  textVersion = "";
  for (uint i = 0; i < data.size(); i++){
    QTableWidgetItem* it =  new QTableWidgetItem(QString(data[i].name.c_str()));
    table->setItem(i, 0, it);
    QString time_string= formatTimeStamp(QString::number(data[i].tot_time));
    //table->setItem(i, 1, new QTableWidgetItem(tr("%1").arg(time_string)));
    QTableWidgetItem* item = new QTableWidgetItem(tr("%1").arg(data[i].n_calls));
    item->setTextAlignment(Qt::AlignRight);
    table->setItem(i, 2, item);
    table->setItem(i, 3, new QTableWidgetItem(QString::fromUtf8(data[i].file.c_str())));
    textVersion += data[i].name + "\t" + time_string.toStdString() + "\t"\
                   + std::to_string(data[i].n_calls) + "\t" + data[i].file + "\n";
  }

  this->updateTimeStamps();
  header->resizeSections(QHeaderView::Stretch);
  header->setSectionResizeMode(1,QHeaderView::Stretch);
  exportTableToTextFile("function_statistics.txt");
}

void FunctionDetails::exportTableToTextFile(const char* filename) const {
  std::ofstream out(filename, std::ofstream::trunc);
  if (out.is_open()){
    out << textVersion;
    out.close();
  }
  else {
    std::cout << "[-] Error: Impossible to export function table to file " << filename;
    std::cout << std::endl;
  }
}

void FunctionDetails::selectFunction(int row){
  QTableWidgetItem* firstColItem = table->item(row, 0);
  if (firstColItem == nullptr){
    std::cout << "[-] Error: impossible to get function name from selection in table" << std::endl;
    return;
  }
  selectFunction(firstColItem->text().toStdString());
}

void FunctionDetails::selectFunction(QTableWidgetItem* it){
  selectFunction(it->row());
}

void FunctionDetails::selectFunction(std::string fname){
  if (fname == selectedFunction) return;
  selectedFunction = fname;
  setSourceCode(fname);
  asmCode->setPlainText(QString::fromUtf8(get_asm_code(fname).c_str()));
  stallChart->construct(fname);
}

void FunctionDetails::setSourceCode(std::string fname){
  sourceCode->setPlainText(QString::fromUtf8(get_source_code(fname).c_str()));
  int line = function2line_number(fname);
  if (line > 0){
    sourceCode->moveCursor(QTextCursor::End);
    sourceCode->setTextCursor(QTextCursor(
      sourceCode->document()->findBlockByLineNumber(
        std::max(0, line - 5))));
  }
}

void FunctionDetails::selectRow(const char* name){
  if (table == nullptr) return;
  for (int i = 0; i < table->rowCount(); i++){
    if (table->item(i, 0)->text() == QString(name)){
      table->selectRow(i);
      return;
    }
  }
}
