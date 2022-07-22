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

#include "json.hpp"


//! [0]
TreeModel::TreeModel(js::config *json, QObject *parent)
    : QAbstractItemModel(parent)
{
    rootItem = new TreeItem({tr("Signal"), tr("Path")});
    setupModelData(json, rootItem);
}


TreeModel::TreeModel(js::config *json, const QString &path, QObject *parent)
    : QAbstractItemModel(parent)
{
    rootItem = new TreeItem({tr("Signal"), tr(path.toStdString().c_str())});
    setupModelData(json, rootItem);
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
        return QSize(100, signalHeight); // for all rows
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


void TreeModel::parse_signals(TreeItem *parent, js::config *json)
{

    for (js::config *child_json: json->get_elems())
    {
        QString child_name = QString::fromStdString(child_json->get("name")->get_str());
        QVector<QVariant> columnData;

        columnData.reserve(2);
        columnData << child_name;

        js::config *path_json = child_json->get("path");

        // Here , we need to check if the signal has been received from the backend.
        // This means that the backend needs to be opened before building the signals
        // tree model.

        if (path_json != NULL)
        {
            columnData << QString::fromStdString(path_json->get_str());
        }
        else
        {
            columnData << QString::fromStdString("null");
        }


        TreeItem *child = new TreeItem(columnData, parent);
        js::config *child_signals = child_json->get("signals");

        parent->appendChild(child);

        // check if this element is the root of a group
        js::config *group_json = child_json->get("group");

        if (group_json != NULL)   {
            // Store this group in the map with the group root as the key
            // and the TreeItem as the second element of the map
            QString rootName = QString::fromStdString(group_json->get_str());
            dynamicGroups.insert(rootName,child);
        }

        if (child_signals != NULL)
        {
            this->parse_signals(child, child_signals);
        }

    }
}


void TreeModel::setupModelData(js::config *json, TreeItem *parent)
{
    QVector<TreeItem*> parents;
    QVector<int> indentations;
    parents << parent;
    indentations << 0;

    int number = 0;

    this->parse_signals(parent, json->get("**/views/timeline/signals"));
}

/*!
    Returns a list of indexes for the items in the column of the \a start index
    where data stored under the given \a role matches the specified \a value.
    The way the search is performed is defined by the \a flags given. The list
    that is returned may be empty. Note also that the order of results in the
    list may not correspond to the order in the model, if for example a proxy
    model is used. The order of the results cannot be relied upon.
    The search begins from the \a start index, and continues until the number
    of matching data items equals \a hits, the search reaches the last row, or
    the search reaches \a start again - depending on whether \c MatchWrap is
    specified in \a flags. If you want to search for all matching items, use
    \a hits = -1.
    By default, this function will perform a wrapping, string-based comparison
    on all items, searching for items that begin with the search term specified
    by \a value.
    \note The default implementation of this function only searches columns.

*/
QModelIndexList TreeModel::match(const QModelIndex &start, int role,
                                 const QVariant &value, int hits,
                                 Qt::MatchFlags flags) const
{
    QModelIndexList result;
    uint matchType = flags & 0x0F;
    Qt::CaseSensitivity cs = flags & Qt::MatchCaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;
    bool recurse = flags & Qt::MatchRecursive;
    bool wrap = flags & Qt::MatchWrap;
    bool allHits = (hits == -1);
    QString text; // only convert to a string if it is needed
    const int column = start.column();
    QModelIndex p = parent(start);
    int from = start.row();
    int to = rowCount(p);
    // iterates twice if wrapping
    for (int i = 0; (wrap && i < 2) || (!wrap && i < 1); ++i) {
        for (int r = from; (r < to) && (allHits || result.count() < hits); ++r) {
            QModelIndex idx = index(r, column, p);
            if (!idx.isValid())
                continue;
            QVariant v = data(idx, role);
            // QVariant based matching
            if (matchType == Qt::MatchExactly) {
                if (value == v)
                    result.append(idx);
            } else { // QString based matching
                if (text.isEmpty()) // lazy conversion
                    text = value.toString();
                QString t = v.toString();
                switch (matchType) {
                case Qt::MatchRegExp:
                    if (QRegExp(text, cs).exactMatch(t))
                        result.append(idx);
                    break;
                case Qt::MatchWildcard:
                    if (QRegExp(text, cs, QRegExp::Wildcard).exactMatch(t))
                        result.append(idx);
                    break;
                case Qt::MatchStartsWith:
                    if (t.startsWith(text, cs))
                        result.append(idx);
                    break;
                case Qt::MatchEndsWith:
                    if (t.endsWith(text, cs))
                        result.append(idx);
                    break;
                case Qt::MatchFixedString:
                    if (t.compare(text, cs) == 0)
                        result.append(idx);
                    break;
                case Qt::MatchContains:
                default:
                    if (t.contains(text, cs))
                        result.append(idx);
                }
            }
            if (recurse) {
                const auto parent = column != 0 ? idx.sibling(idx.row(), 0) : idx;
                if (hasChildren(parent)) { // search the hierarchy
                    result += match(index(0, column, parent), role,
                                    (text.isEmpty() ? value : text),
                                    (allHits ? -1 : hits - result.count()), flags);
                }
            }
        }
        // prepare for the next iteration
        from = 0;
        to = start.row();
    }
    return result;
}

void TreeModel::insertChild(QString parentPath, QString childName, QString childPath){
    // First search for the parent

    QModelIndexList indexes = this->match (  this->index(0,1),
                                              Qt::DisplayRole,
                                              parentPath,
                                              -1,
                                              Qt::MatchRecursive);
    if (!indexes.isEmpty()) {
        // Information: with this code, expands ONLY first level in QTreeView
        //view->setExpanded(Items.first(), true);
        std::cout << "Parent signal found" << parentPath.toStdString() << std::endl;
    } else
         std::cout << "Parent signal not Found" << parentPath.toStdString() << std::endl;

    // Now we have the index  column #2 of the  parent item
    QModelIndex parentPathIdx = indexes.first();
    // Let's get the parent item itself
    TreeItem* parentItem = static_cast<TreeItem*> (parentPathIdx.internalPointer());

    // We need to create the child Item data
    QVector<QVariant> columnData;

    columnData.reserve(2);
    columnData << childName;

    if (childPath != NULL)
    {
        columnData << childPath;
    }
    else
    {
        columnData << QString::fromStdString("null");
    }

    // Then create the child Item and insert it in the list of the parent item childs
    TreeItem *child = new TreeItem(columnData, parentItem);
    parentItem->appendChild(child);
}

TreeItem* TreeModel::insertChild(TreeItem* parentItem, QString childName, QString childPath){

    // We need to create the child Item data
    QVector<QVariant> columnData;
    //std::cout << "Insert child " << childPath.toStdString() << std::endl;
    columnData.reserve(2);
    columnData << childName;

    if (childPath != NULL)
    {
        columnData << childPath;
    }
    else
    {
        columnData << QString::fromStdString("null");
    }
    // Check if the child already exists
    if (parentItem->childCount()>0)
        for (int i=0; i< parentItem->childCount(); i++)
            if (childName.compare(parentItem->child(i)->data(0).toString())==0) {
                // update the path that can be set to null
                //std::cout << "Update signal path of " << childPath.toStdString() << std::endl;
                parentItem->child(i)->setData(1,childPath);
                return parentItem->child(i);
            }

    // If not, hen create the child Item and insert it in the list of the parent item childs
    TreeItem *child = new TreeItem(columnData, parentItem);
    parentItem->appendChild(child);
    return child;

}


