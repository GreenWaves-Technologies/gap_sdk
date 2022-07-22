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


#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include "json.hpp"


class TreeItem;
// height of a signal image in pixels
const int signalHeight = 19;

// This class contains the tree model of all the signals 
// received from gvsoc
//! [0]
class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit TreeModel(js::config *json, QObject *parent = nullptr);
    TreeModel(js::config *json, const QString &path, QObject *parent);
    ~TreeModel();

    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    // Inserts a new child signal in the tree under a specific parent
    void insertChild(QString parentPath, QString childName, QString childPath);
    TreeItem* insertChild(TreeItem* parentItem, QString childName, QString childPath);
    // override of the match function to allow searching recursively
    QModelIndexList match(const QModelIndex &start, int role,
                          const QVariant &value, int hits,
                          Qt::MatchFlags flags) const override;
    // This map is used to store the dynamic groups declarations
    // detected during the analysis of the Json file
    // The key element stores the group root path and the second element
    // stores the corresponding tree item for this root
    QMap<QString,TreeItem*> dynamicGroups;

private:
    void setupModelData(js::config *json, TreeItem *parent);
    void parse_signals(TreeItem *parent, js::config *json);

    TreeItem *rootItem;
};
//! [0]

#endif // TREEMODEL_H
