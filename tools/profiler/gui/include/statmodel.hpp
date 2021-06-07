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

#ifndef STATMODEL_HPP
#define STATMODEL_HPP
#include <unistd.h>
#include <QAbstractTableModel>
#include "treemodel.hpp"
#include <QTreeView>
#include <QTimer>
#include "backend_interface.hpp"
#include "datamanager.hpp"
#include "timeline.hpp"

class Timeline;

class StatModel : public QAbstractTableModel
{
    Q_OBJECT

public:

    StatModel(Timeline* timeLine, QObject *parent = 0);
    void populateData(uint64_t t0 , uint64_t t1);

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    Timeline* tl; // timeline widget

public slots:

    void updateTreeParameters(TreeModel* sigModel,
                              QTreeView* signalsTreeView);
private:
    QList<QString> smDutyTime; // the data stored by the model

    // usefull objects to construct the model
    TreeModel* signalsModel = NULL;
    QTreeView* signalsView = NULL;
    QTimer* timer;
    int rowNb = -1;

    // utility functions to help construct the model
    template <typename T>
    uint64_t getTimeUp(const Data_with_time<T>& dwt,
                                    uint64_t t0,
                                    uint64_t t1
                                    );
    uint64_t calculateTimeUp(const QString signalPath,
                        std::vector<TLData<const char*>> data,
                        std::vector<int> signalIdList,
                        uint64_t t0,
                        uint64_t t1);
    void insertRow( const QString signalPath,
                    std::vector<TLData<const char*>> data,
                    std::vector<int> signalIdList,
                    int line,
                    uint64_t t0,
                    uint64_t t1);
    void buildModel(QModelIndex parent,
                    QModelIndex signalIdx,
                    std::vector<TLData<const char*>> data,
                    std::vector<int> signalIdList,
                    int* line,
                    uint64_t t0,
                    uint64_t t1);
    //void updateModel(QModelIndex parent,QModelIndex signalIdx);
    //void updateRow(const QString signalPath);
//private slots:
    //void refreshData();
};

#endif // STATMODEL_HPP
