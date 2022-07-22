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

#include "stattable.hpp"
#include <QHeaderView>
#include <QScrollBar>

StatTable::StatTable(SignalTree* sigTreeView, QWidget *parent) : QTableView(parent) {

    signalsTreeView = sigTreeView;
    // Make table header unvisible
    this->horizontalHeader()->setVisible(false);

    //this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    this->verticalScrollBar()->setPageStep(signalsTreeView->verticalScrollBar()->pageStep());
    this->verticalScrollBar()->setMinimum(signalsTreeView->verticalScrollBar()->minimum());
    this->verticalScrollBar()->setMaximum(signalsTreeView->verticalScrollBar()->maximum());
    this->verticalScrollBar()->setSingleStep(signalsTreeView->verticalScrollBar()->singleStep());
    // this->setColumnWidth(0,100); should be done after setting the model
    this->setAlternatingRowColors(true);
    //this->horizontalHeader()->setSectionResizeMode(QtWidgets.QHeaderView.Stretch);
    //this->setSizeAdjustPolicy(QtWidgets.QAbstractScrollArea.AdjustToContents);

}
