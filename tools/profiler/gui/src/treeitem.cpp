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

#include "treeitem.hpp"

//! [0]
TreeItem::TreeItem(const QVector<QVariant> &data, TreeItem *parent)
    : m_itemData(data), m_parentItem(parent)
{}
//! [0]

//! [1]
TreeItem::~TreeItem()
{
    qDeleteAll(m_childItems);
}
//! [1]

//! [2]
void TreeItem::appendChild(TreeItem *item)
{
    m_childItems.append(item);
}
//! [2]

//! [3]
TreeItem *TreeItem::child(int row)
{
    if (row < 0 || row >= m_childItems.size())
        return nullptr;
    return m_childItems.at(row);
}
//! [3]

//! [4]
int TreeItem::childCount() const
{
    return m_childItems.count();
}
//! [4]

//! [5]
int TreeItem::columnCount() const
{
    return m_itemData.count();
}
//! [5]

//! [6]
QVariant TreeItem::data(int column) const
{
    if (column < 0 || column >= m_itemData.size())
        return QVariant();
    return m_itemData.at(column);
}
//! [6]

//! [7]
TreeItem *TreeItem::parentItem()
{
    return m_parentItem;
}
//! [7]

//! [8]
int TreeItem::row() const
{
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<TreeItem*>(this));

    return 0;
}
//! [8]
