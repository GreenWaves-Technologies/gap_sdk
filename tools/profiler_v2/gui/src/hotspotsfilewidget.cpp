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
#include <assert.h>
#include <QMessageBox>

#include "hotspotsfilewidget.hpp"
#include <fstream>
#include <sstream>
#include <QtDebug>
#include "codeeditor.hpp"
#include <QDockWidget>

const char* HotspotsFileWidget::columnHeader[] = {
    "Time %",
    "Total Time",
    "From file"
};

const int HotspotsFileWidget::nbColumn = 3;
/*const int HotspotsFileWidget::nbColumn = \
    sizeof(HotspotsFileWidget::columnHeader) / sizeof(HotspotsFileWidget::columnHeader[0]);
*/

HotspotsFileWidget::HotspotsFileWidget(QWidget* parent,
                                       Profiler_backend_user *backend,
                                       CodeEditorWrap *sourceCodeWrap):
    backend(backend),
    sourceCodeWrap(sourceCodeWrap)

{
    qDebug() << "Construct HotspotWidget";
    this->setParent(parent);
    //fillTable();
    //sourceCode->setWordWrapMode(QTextOption::NoWrap);
    //asmCode->setWordWrapMode(QTextOption::NoWrap);
    //sourceCode->setReadOnly(true);
    //asmCode->setReadOnly(true);

    //cppHighlighter = new Highlighter(sourceCode->document());
    //asmHighlighter = new Highlighter(asmCode->document());
    if (table == nullptr) table = new QTableWidget();
    header = table->horizontalHeader();

    this->current_ht_result = NULL;

    table->sizePolicy().setHorizontalPolicy(QSizePolicy::Ignored);
    table->setRowCount(0);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setColumnCount(nbColumn);
    // Fill Column headers
    for (int i = 0; i < nbColumn; i++){
        QTableWidgetItem* headeri = new QTableWidgetItem();
        headeri->setText(QString(columnHeader[i]));
        table->setHorizontalHeaderItem(i,headeri);
    }


    layout = new QHBoxLayout;
    setLayout(layout);

    layout->addWidget(table);
    sourceCodeWrap->sourceCode->setWordWrapMode(QTextOption::NoWrap);
    //asmCode->setWordWrapMode(QTextOption::NoWrap);
    sourceCodeWrap->sourceCode->setReadOnly(true);
    //asmCode->setReadOnly(true);
    connect(table, SIGNAL(itemClicked(QTableWidgetItem*)), this,
            SLOT(selectFunction(QTableWidgetItem*)));
    connect(table->verticalHeader(), SIGNAL(sectionClicked(int)), this,
            SLOT(selectFunction(int)));

    //timer = new QTimer(this);
    connect(this, SIGNAL(hotspotsReady()),this,SLOT(fillHotspotsTable()),Qt::QueuedConnection);
    QObject::connect(this, SIGNAL(visibilityChanged(bool)),
                     this , SLOT(handleVisibilityChanged(bool)) );
    //connect(timer, SIGNAL(timeout()), this, SLOT(getHotspots()),Qt::QueuedConnection);
    //timer->start(20000);
    qDebug() << "End Construct HotspotWidget";
}


void HotspotsFileWidget::getHotspots(uint64_t t0Select, uint64_t t1Select){
    // Getting hotspots for a time range selected by the user
    //assert(backend);
    //backend->getBackend()->get_hotspots(this, -1, t0Select, t1Select);
}

void HotspotsFileWidget::getHotspots() {
    //assert(backend);
    //qDebug() << "getHotSpots Null, -1, 0, 60000000000";
    //backend->getBackend()->get_hotspots(this, -1, 0, 60000000000);
    //qDebug() << "Done getHotSpots Null, -1, 0, 60000000000";
}

/*
void HotspotsFileWidget::switchLegendMode(LegendMode newMode)
{
    this->currentMode = newMode;
    this->updateTimeStamps();
}

*/

/*void HotspotsFileWidget::updateTimeStamps(void)
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
*/

QString HotspotsFileWidget::formatTimeStamp(QString ts) const {

    const int step = 3;
    const char mychar = ',';
    for (int i = ts.length()-step; i>0; i=i-step)
        ts.insert(i, mychar);

    return ts;
}

void HotspotsFileWidget::removeTableRows(){
    for(int i=0; i<table->rowCount(); i++)
        table->removeRow(i);
}

void HotspotsFileWidget::updateHotspotsTable(Hospot_results *ht_result,
                                             int64_t ht_start_timestamp,
                                             int64_t ht_end_timestamp,
                                             int64_t ht_max_timestamp)
{
    hotspotsMutex.lock();

    this->current_ht_result = ht_result;

    beHotspots.clear();
    hotspots.clear();
    beAllHotspots.clear();
    allHotspots.clear();

    if (ht_result==NULL){
        qDebug() << "No hotspots yet " << result;
        return;
    }
    ht_result->get_file_hotspots(beHotspots);
    ht_result->get_line_hotspots(beAllHotspots);
    for (auto x: beHotspots)
        hotspots.push_back(SafeHotspotFileInfo(x));
    for (auto x: beAllHotspots)
        allHotspots.push_back(SafeHotspotLineInfo(x));

    start_timestamp = ht_start_timestamp;
    end_timestamp = ht_end_timestamp;
    max_timestamp = ht_max_timestamp;

    hotspotsMutex.unlock();

    emit hotspotsReady();


}

void HotspotsFileWidget::fillHotspotsTable()

{
    // Fills the hotspot Table when hotspots data is available
    // hotspot parameters
    // pc: program counter for this hotspot
    // duration: time spent on this PC
    // function: name of the function to which the pc belongs
    qDebug() << "updateHotspotsTable";

    int64_t time = 0;
    int z = 0;

    hotspotsMutex.lock();

    this->removeTableRows();
    table->setRowCount(hotspots.size());
    table->setSortingEnabled(false);

    // Create Hotspot rows

    // Initialize text version of the Table
    //textVersion = "";

    for (auto x: hotspots)
    {
        time += x.duration;

        QTableWidgetItem* itPercentage =  new QTableWidgetItem();
        itPercentage->setData(Qt::DisplayRole,(double)(x.percentage));
        table->setItem(z, 0,itPercentage );
        itPercentage->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);


        QTableWidgetItem* itDuration =  new QTableWidgetItem();
        //itDuration->setData(Qt::DisplayRole,x->duration);
        itDuration->setData(Qt::DisplayRole,(qulonglong)(x.duration));
        table->setItem(z, 1,itDuration );
        itDuration->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

        table->setItem(z, 2, new QTableWidgetItem(QString::fromStdString(x.fileName)));

        // Fill up text version
        /*textVersion +=  std::to_string(x->pc) + "\t"\
                    + std::to_string(x->duration) + "\t"\
                    + time_string.toStdString() + "\t"\
                    + std::string(x->function) +  "\t"\
                    + std::string(x->file)+  "\t"\
                    + std::to_string(x->line) +  "\n";
      */
        z++;
    }

    //this->updateTimeStamps();
    // resize the sections to fill available space
    header->resizeSections(QHeaderView::Stretch);
    // specify columns that can be interactively resized
    header->setSectionResizeMode(2,QHeaderView::Interactive);
    // specify sections that can be ordered
    table->setSortingEnabled(true);
    table->sortItems(0,Qt::DescendingOrder);

    if (end_timestamp >= max_timestamp) {
        // simulation is finished
        table->setSortingEnabled(true);
    }

    //exportTableToTextFile("hotspot_statistics.txt");
    qDebug() << "END updateHotspotsTable";

    hotspotsMutex.unlock();
}

void HotspotsFileWidget::exportTableToTextFile(const char* filename) const {
    QMessageBox messageBox;
    std::ofstream out(filename, std::ofstream::trunc);
    if (out.is_open()){
        out << textVersion;
        out.close();
    }
    else {
        //messageBox.information(this,tr("[-] Error: Impossible to export hotspot table to file"),tr(filename));
    }
}

void HotspotsFileWidget::selectFunction(int row){

    QTableWidgetItem* fileItem = table->item(row, 2);

    if (fileItem == nullptr){
        QMessageBox::information(this,tr("[-] Error: impossible to get file name from selection in table"),tr(" "));
        return;
    }

    selectFunction(std::string(""),
                   fileItem->text().toStdString(),
                   1);
}

void HotspotsFileWidget::selectFunction(QTableWidgetItem* it){
    selectFunction(it->row());
}

void HotspotsFileWidget::selectFunction(std::string funcName,
                                        std::string fileName,
                                        int hotspotLine){
    setSourceCode(funcName, fileName,hotspotLine);
    //asmCode->setPlainText(QString::fromUtf8(get_asm_code(fname).c_str()));
    //stallChart->construct(fname);

}

std::string HotspotsFileWidget::read_file(std::string filename){

    std::ifstream sourceFile (filename);
    if (sourceFile.is_open()){
        std::stringstream buf;
        buf << sourceFile.rdbuf();
        return buf.str();
    } else {
        std::cout << "[-] Error: Unable to open file " << filename << std::endl;
        return "";
    }

}


std::string HotspotsFileWidget::get_source_code(std::string funcName,
                                                std::string fileName,
                                                int hotspotLine) {

    if (fileName == ""){
        std::cout << "[-] Warning: get_source_code of function  ";
        std::cout << funcName ;
        std::cout << " in file " << fileName;
        std::cout << "not available" << std::endl;
        return std::string("");
    }

    return read_file(fileName);
}



void HotspotsFileWidget::getFileHotspotList(const std::string & fileName,
                                            std::list<SafeHotspotLineInfo> * fileHotspots){
    // gets the list of all hotspots with the same fileName

    // This procedure creates a list of hotspots that all belong to the file
    // of name filename
    hotspotsMutex.lock();

    std::list<Hotspot_line_info *> hotspots;
    this->current_ht_result->get_line_hotspots(hotspots, fileName.c_str());

    for (Hotspot_line_info *x : hotspots) {
        // add the element to the new list
        fileHotspots->push_back(SafeHotspotLineInfo(x));
    }
    hotspotsMutex.unlock();
}

void HotspotsFileWidget::setSourceCode(std::string funcName,
                                       std::string fileName,
                                       int hotspotLine){

    std::list<SafeHotspotLineInfo> fileHotspots;
    getFileHotspotList(fileName, &fileHotspots);
    QDockWidget* dockW = qobject_cast<QDockWidget*>(sourceCodeWrap->parentWidget());
    dockW->setWindowTitle(QString::fromStdString(fileName));
    sourceCodeWrap->sourceCode->setHotspotsList(fileHotspots);
    sourceCodeWrap->sourceCode->setButtonsStatus(
                sourceCodeWrap->radioPercent->isChecked(),
                sourceCodeWrap->radioCall->isChecked(),
                sourceCodeWrap->radioDuration->isChecked());
    // Then clear fileHotspots ----
    sourceCodeWrap->sourceCode->setPlainText(QString::fromStdString(get_source_code(funcName,fileName,hotspotLine)));
    int line = hotspotLine;
    if (line > 0){
        sourceCodeWrap->sourceCode->moveCursor(QTextCursor::End);
        sourceCodeWrap->sourceCode->setTextCursor(QTextCursor(
                                                      sourceCodeWrap->sourceCode->document()->findBlockByLineNumber(
                                                          std::max(0, line - 5))));
        sourceCodeWrap->sourceCode->highlightCurrentHotspotLine(line);
    }

}



void HotspotsFileWidget::selectRow(const char* name){
    if (table == nullptr) return;
    for (int i = 0; i < table->rowCount(); i++){
        if (table->item(i, 0)->text() == QString(name)){
            table->selectRow(i);
            return;
        }
    }
}

void HotspotsFileWidget::handleVisibilityChanged(bool visible){

    if(visible) {
        fillHotspotsTable();
        update();
        show();
    }

}
