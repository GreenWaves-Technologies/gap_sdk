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

#ifndef SIGNALTREE_HPP
#define SIGNALTREE_HPP

#include <iostream>
#include <QTreeView>
#include <QWidget>
#include <QString>
#include "treemodel.hpp"

// Class for displaying the  list of all the signals required by the user from Gvsoc
// will be displayed as a hierarchical tree at the left of the graphics view. 
class SignalTree : public QTreeView
{
    Q_OBJECT
public:
    SignalTree(js::config *json, QWidget* parent=NULL);
    TreeModel* model;
};

#endif // SIGNALTREE
