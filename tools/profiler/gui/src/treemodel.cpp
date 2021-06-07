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


/*
    treemodel.cpp

    Provides a simple tree model to show how to create and use hierarchical
    models.
*/

#include "treemodel.hpp"
#include "treeitem.hpp"

#include <QStringList>
#include <QSize>
#include <QColor>
#include <iostream>


//! [0]
TreeModel::TreeModel(const QString &data, QObject *parent)
    : QAbstractItemModel(parent)
{
    rootItem = new TreeItem({tr("Signal"), tr("Path")});
    setupModelData(data.split('\n'), rootItem);
}


TreeModel::TreeModel(const QString &data, const QString &path, QObject *parent)
    : QAbstractItemModel(parent)
{
    rootItem = new TreeItem({tr("Signal"), tr(path.toStdString().c_str())});
    setupModelData(data.split('\n'), rootItem);
}


//! [0]

//! [1]
TreeModel::~TreeModel()
{
    delete rootItem;
}
//! [1]

//! [2]
int TreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    return rootItem->columnCount();
}
//! [2]

//! [3]
QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::SizeHintRole)
      {
        // std::cout << "index.row() = " << index.row() << cout::endl;
        // std::cout << "index.column() = " << index.column() << cout::endl;
        // An example. Set the size of the first cell.
        //if (index.row() == 0 && index.column() == 0)
        //{
          return QSize(100, 30); // for all rows
        //}
      }
      else if (role == Qt::BackgroundRole) {
        if (0 == index.row() % 2)
            return QColor(247,245,243);
            //return QColor(222,221,219);
        else
            return QColor(255,255,255);
      }
      else if (role != Qt::DisplayRole)
        return QVariant();

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    return item->data(index.column());
}

//! [3]

//! [4]
Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}
//! [4]

//! [5]
QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}
//! [5]

//! [6]
QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}
//! [6]

//! [7]
QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}
//! [7]

//! [8]
int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}
//! [8]


void TreeModel::setupModelData(const QStringList &lines, TreeItem *parent)
{
    QVector<TreeItem*> parents;
    QVector<int> indentations;
    parents << parent;
    indentations << 0;

    int number = 0;

    while (number < lines.count()) {
        int position = 0;
        while (position < lines[number].length()) {
            if (lines[number].at(position) != ' ')
                break;
            position++;
        }

        const QString lineData = lines[number].mid(position).trimmed();


        if (!lineData.isEmpty()) {
            // Read the column data from the rest of the line.
            //const QStringList columnStrings = lineData.split('\t', QString::SkipEmptyParts);
            QStringList columnStrings = lineData.split(';', QString::SkipEmptyParts);
            QVector<QVariant> columnData;
            columnData.reserve(columnStrings.count());
            for (QString &columnString : columnStrings) {
                // first eliminate blanks and tabs
                //std::cout << "*" << columnString.toStdString() << "*" << std::endl;
                columnString=columnString.simplified(); // changes all whitespace characters to single instance of ASCII 32
                //std::cout << "*" << columnString.toStdString() << "*" << std::endl;
                columnString.replace(" ", "" );
                //std::cout << "*" << columnString.toStdString() << "*" << std::endl;

                columnData << columnString;
                //std::cout << "columnString[" << number << "] = " << columnString.toStdString() << std::endl;
            }


            if (position > indentations.last()) {
                // The last child of the current parent is now the new parent
                // unless the current parent has no children.

                if (parents.last()->childCount() > 0) {
                    parents << parents.last()->child(parents.last()->childCount()-1);
                    indentations << position;
                }
            } else {
                while (position < indentations.last() && parents.count() > 0) {
                    parents.pop_back();
                    indentations.pop_back();
                }
            }

            // Append a new item to the current parent's list of children.
            parents.last()->appendChild(new TreeItem(columnData, parents.last()));
        }
        ++number;
    }
}
