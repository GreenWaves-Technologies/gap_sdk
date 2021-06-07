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

#include "statmodel.hpp"
#include <QDebug>
#include <iostream>
#include <cmath>

extern Data_manager* core;

StatModel::StatModel(Timeline* timeline, QObject *parent) : QAbstractTableModel(parent)
{
  tl=timeline;

  // Refreshing Implementation
  //timer = new QTimer(this);
  //connect(timer, SIGNAL(timeout()), this, SLOT(refreshData()));
  //timer->start(2000);
}

//updates the parameters allowing the right construction
//of the tree model. Called each time the signalsTreeView
//is expanded or collapsed under user action
// Function called from two functions:
// handleSignalNodeExpanded()
// handleSignalNodeCollapsed()
void StatModel::updateTreeParameters(TreeModel* sigModel,
                                     QTreeView* signalsTreeView) {
  this->signalsView = signalsTreeView;
  this->signalsModel = sigModel;
}

template <typename T >
uint64_t StatModel::getTimeUp(const Data_with_time<T>& dwt,
                        uint64_t t0,
                        uint64_t t1) {
  //std::cout << "[-]  StatModel::getTimeUp t0: " << t0 << "t1: " << t1 <<std::endl;
  // First calculate the interval
  int avg_factor =0;
  uint64_t timeUp=0;
  uint64_t tBegin = dwt.begin;
  uint64_t tEnd = dwt.end;
  if (tBegin< t0)
    tBegin=t0;
  if (tEnd > t1)
    tEnd=t1;

  std::string txtValue;
  txtValue = std::string("1");
  if (! dwt.mixed_up){
    if (std::is_same<T, const char*>::value){
      // pointer trick, because the compiler does not understand that T is
      // nothing but const char* in this "if"
      const char* s;
      memcpy(&s, &(dwt.d), sizeof(const char*));
      //txtValue = std::to_string(signalId) + "-" + std::string(s);
      txtValue =  std::string(s);
    }
    else if (std::is_same<T, generic_data_t>::value){
      generic_data_t g;
      memcpy(&g, &(dwt.d), sizeof(generic_data_t));
      //txtValue = std::to_string(signalId) + "-" + std::to_string(g);
      txtValue =  std::to_string(g);
    }
    else if (std::is_same<T, g_decompressed_data_t>::value){
      g_decompressed_data_t ud;
      memcpy(&ud, &(dwt.d), sizeof(g_decompressed_data_t));
      txtValue = std::to_string(ud.value);
      avg_factor = ud.n_items_in_avg;
      if (ud.value!=0 && avg_factor!=1)
        //txtValue = std::to_string(signalId) + "-" + (std::to_string(ud.value) + "(avg. "
        txtValue =  (std::to_string(ud.value) + "(avg. "
                 + std::to_string(avg_factor) + ")" );
      else if (ud.value!=0)
        //txtValue = std::to_string(signalId) + "-" + (std::to_string(ud.value));
        txtValue =   (std::to_string(ud.value));
    }
    else {
      std::cout << "[-] Warning data type not recognized by the GUI" << std::endl;
      //std::cout << "[-] timeUp" << 0<< std::endl;
      return 0;
    }
    //painter.setBrush(QBrush(getColor(txtValue)));
  }
  else {
    // Trace is mixed up .. means it is composed of merged traces.
    // painter.setBrush(QBrush(Qt::black));
    // shouldn't happen
    std::cout << "[-] Signal shouldn't be mixed up" << std::endl;
    //std::cout << "[-] timeUp" << 0<< std::endl;
    return 0;
  }

  if (txtValue.compare(std::string("0")) != 0) {
    timeUp= tEnd - tBegin;
    //std::cout << "[-] timeUp" << timeUp << std::endl;
  }

  return timeUp;
}


// Calculates the time the signal is Up within the [t0,T1] range
uint64_t StatModel::calculateTimeUp(const QString signalPath,
                    std::vector<TLData<const char*>> data,
                    std::vector<int> signalIdList,
                    uint64_t t0,
                    uint64_t t1) {

  //std::cout << "[-]  ******* StatModel::calculateTimeUp " << std::endl;
  //std::cout << "[-]  ******* SignalPath " << signalPath.toStdString() << std::endl ;
  QString pcSignalPath = signalPath;
  uint64_t timeUp = 0;
  int signalId = 0;

  // First Look if it's a core or fc & state signal
  // if it's a peX/state signal, or fc/state signal, we define the path to a new signal that should be taken
  // for the calculation in pcSignalPath. In the other cases, pcSignalPath is set to "" .
  if (pcSignalPath.contains("pe") & pcSignalPath.contains("state")) {
    pcSignalPath.replace("state", "pcer_instr");
  } else if (pcSignalPath.contains("fc") & pcSignalPath.contains("state")) {
    pcSignalPath.replace("state", "pc");
  }

  // Second look for the signal ID
  signalId = getSignalIdFromBackend(pcSignalPath.toStdString());
  // Get the signal index in list
  int signalIdx = tl->getTLGView()->getSignalIndex(signalIdList, signalId);
  //std::cout << "[-]  ******* signalIdx" << signalIdx <<  std::endl;
  //std::cout <<  "[-] ******* pcSignalPath " << pcSignalPath.toStdString() << std::endl;

  if (signalIdx != -1) { // signal found in core signals
    //qDebug() << "[-] Calculate timeUp of core Signal " << signalId << " " << pcSignalPath  ;
    // Index exist ==> just calculate TimeUp value for the timeline interval
    for(auto iter = data[signalIdx].between(t0, t1, 1); !iter.done(); ++iter){
      // Gets the time the signal is up during the [t0,t1] range
      timeUp+= getTimeUp<const char*>(*iter,t0,t1);
    }
    //qDebug() << "timeUp " << timeUp ;
    return timeUp;
  }
  //else
  //  std::cout << " Signal ID " <<  signalId << " not found in signalIdList or not to be displayed" << std::endl;

  //Regular signal ==> just calculate timeUp value
  auto l = event_timestamps(signalId);
  if (l != nullptr){
    //qDebug() << "[-] Calculate TimeUp for  Other Signal " << signalId << " " << pcSignalPath ;
    for(auto item : decompress(l->between(t0, t1, 1))){
      timeUp += getTimeUp<g_decompressed_data_t>(item,t0,t1);
    }
    //qDebug() << "timeUp " << timeUp ;
    return timeUp;
  }

  return timeUp;
}

void StatModel::insertRow(const QString signalPath,
                          std::vector<TLData<const char*>> data,
                          std::vector<int> signalIdList,
                          int line,
                          uint64_t t0,
                          uint64_t t1) {
  //qDebug() << "[-]  StatModel::insertRow " ;
  //std::cout <<  "[-] ******* timestamp " << t0 << "  " << t1 <<  std::endl;
  //beginInsertRows();

  // First get signal ID
  //uint32_t signalId = getSignalId(signalPath.toStdString());
  // Calculate Time Up

  // Get signal Stat from id
  //int dutyTime = core->signalDutyTime[signalId];
  //auto maxTime = get_max_time();
  uint64_t dutyTime = calculateTimeUp(signalPath, data, signalIdList,t0,t1);

  uint64_t rangeTime = (t0 > t1) ? t0 - t1 : t1 - t0;
  //qDebug() << "[-] dutyTime  " << dutyTime ;
  //qDebug() << "[-] rangeTime  " << rangeTime ;
  int percentage =0;
  if (rangeTime>0)
    percentage = (int)(100.0 * dutyTime / rangeTime);
  else
    percentage =0;

  //std::cout << "[-] line " << line << "percentage  " << percentage << std::endl;
  // Enter data in smDutyTime list at the row n° line .. or in order?? check it
  if (signalPath.contains("pe") & signalPath.contains("state"))
    smDutyTime.insert(line,QString::number(((float)percentage) / 100.00) );
  else
    smDutyTime.insert(line,QString::number(percentage) + QString("%"));

  //endInsertRows()
}

void StatModel::buildModel( QModelIndex parent,
                            QModelIndex signalIdx,
                            std::vector<TLData<const char*>> data,
                            std::vector<int> signalIdList,
                            int* line,
                            uint64_t t0,
                            uint64_t t1) {
  // This function walks recursively through the signals tree
  // and fills up the signals statistics table accordingly
  //qDebug() << "[-]  StatModel::buildModel " ;
  //std::cout <<  "[-] ******* timestamp " << t0 << "  " << t1 <<  std::endl;
  int i =0;
  QModelIndex idx;
  QModelIndex siblingIdx;
  // dump parent node
  if (parent != signalsView->rootIndex()){
      (*line)++;
      insertRow(signalIdx.data().toString(), data, signalIdList,*line,
                          t0, t1);
  }

  if ((parent == signalsView->rootIndex()) || signalsView->isExpanded(parent)) {
      for (i=0; i< signalsModel->rowCount(parent) ; i++) {
          idx = signalsModel->index(i,0,parent);
          siblingIdx = signalsModel->index(i,1,parent);
          buildModel(idx,siblingIdx, data, signalIdList,line,
                          t0, t1);
      }
  }

}

// Create a method to populate the model with data:
void StatModel::populateData(uint64_t t0 , uint64_t t1)
{
    //qDebug() << "[-]  StatModel::populateData ";

    // clear signals stats list
    smDutyTime.clear();

    auto data = get_timeline_data();

    auto signalIdList = get_timeline_id();
    int line = -1;
     //std::cout <<  "[-] ******* timestamp " << t0 << "  " << t1 <<  std::endl;
    if (signalsView != NULL) {
      //std::cout << "[-]  signalsView: " << signalsView << std::endl;
      auto index = signalsView->rootIndex();
      //std::cout <<  "[-] ******* timestamp " << t0 << "  " << t1 <<  std::endl;
      buildModel(index,
              signalsView->rootIndex(),
              data, signalIdList, &line,
              t0, t1);
    }
    QModelIndex top =  createIndex(0,0);
    QModelIndex bottom = createIndex(rowCount() - 1, 0);
    emit dataChanged(top, bottom);
    return;
}



int StatModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return smDutyTime.length();
}

int StatModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant StatModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())  {
        return QVariant();
    }

    if (role == Qt::SizeHintRole) {
        return QSize(100, 30); // for all rows
    } else if (index.column() == 0 && role == Qt::TextAlignmentRole) {
        return Qt::AlignRight;
    }
    else if (role != Qt::DisplayRole)
        return QVariant();

    if (index.column() == 0) {
        return smDutyTime[index.row()];
    }
    /*else if (index.column() == 1) {
        return tm_contact_phone[index.row()];
    }
    */
    return QVariant();
}

QVariant StatModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        if (section == 0) {
            //return QString("Name");
            return QVariant();
        }
        else if (section == 1) {
            //return QString("Phone");
            return QVariant();
        }

    }
    return QVariant();
}
