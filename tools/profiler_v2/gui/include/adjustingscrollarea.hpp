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

#ifndef ADJUSTINGSCROLLAREA_HPP
#define ADJUSTINGSCROLLAREA_HPP

#include <string>
#include <unordered_map>

#include <QWidget>
#include <QScrollArea>
#include <QGraphicsScene>
#include <QResizeEvent>
#include <QLabel>
#include <QGraphicsItem>
#include <QTreeView>

#include <thread>
#include <mutex>
#include <QMutex>
#include <unistd.h>

#include "treemodel.hpp"
#include "tlgview.hpp"
#include "signaltree.hpp"
#include <QTreeWidget>
#include <QTreeWidgetItem>

// Scroll area that will contain the graphics view gview
// Its vertical scrollbar is piloted by the signals tree view vertical scrollbar
// As such, it's not set visible. 
class AdjustingScrollArea : public QScrollArea {
    Q_OBJECT
    bool eventFilter(QObject * obj, QEvent * ev);
public:
    AdjustingScrollArea(SignalTree* signalsView,
                        TLGView* gview,
                        QWidget * parent = 0);
    void setWidget(QWidget *w);
    void updateWidgetPosition();
    SignalTree* signalsTreeView;
    TLGView* tlgView;
public slots:
    void syncWithTreeView();
private:
    QMutex syncMutex;
    int treeCount(QTreeWidget *tree, QTreeWidgetItem *parent = 0);

};

#endif  //ADJUSTINGSCROLLAREA_HPP
