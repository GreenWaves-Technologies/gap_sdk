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

#include <QScrollBar>
#include <QFile>

#include "signaltree.hpp"
#include "treemodel.hpp"
#include <QMessageBox>

SignalTree::SignalTree(js::config *json, QWidget* parent)
    : QTreeView(parent) {
    //td::cout << "[-] Opening Signals Tree File " << signalsTreeFileName.toStdString() << std::endl;

    // need to check if model exists
    this->model = new TreeModel(json);
    // Test something only
    //model->insertChild(QString("/chip/soc/fc/active_function"),QString("toto"), QString("/chip/cluster/pe0/busy"));
    //std::cout << "model " << model << std::endl;
    // need to check if it's OK before
    this->setModel(model);
    this->setWindowTitle(QObject::tr("Signals"));
    this->setStyleSheet("alternate-background-color:yellow;");
    // hide the column containing the full path of the signal
    this->hideColumn(1);
    this->setColumnWidth(200, 800);
    this->expandToDepth(0);
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    this->setHeaderHidden(true);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    this->verticalScrollBar()->setSingleStep(signalHeight);
    this->verticalScrollBar()->setPageStep(120);
    this->update();
}
