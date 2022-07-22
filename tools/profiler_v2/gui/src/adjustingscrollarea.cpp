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


#include <QtDebug>
#include <QScrollBar>
#include "math.h"
#include "adjustingscrollarea.hpp"

AdjustingScrollArea::AdjustingScrollArea(SignalTree* signalsView,
                                         TLGView* gview,
                                         QWidget * parent) : QScrollArea{parent} {
    qDebug() << "[-] Create ScrollArea";

    signalsTreeView = signalsView;
    tlgView=gview;

    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setWidget(gview);
    this->ensureWidgetVisible(gview);
    this->verticalScrollBar()->setPageStep(signalsTreeView->verticalScrollBar()->pageStep());
    this->verticalScrollBar()->setMinimum(signalsTreeView->verticalScrollBar()->minimum());
    this->verticalScrollBar()->setMaximum(signalsTreeView->verticalScrollBar()->maximum());
    this->verticalScrollBar()->setSingleStep(signalsTreeView->verticalScrollBar()->singleStep());
    this->setWidgetResizable(true);
    this->adjustSize();

}

bool AdjustingScrollArea::eventFilter(QObject * obj, QEvent * ev) {
    if (obj == widget() && ev->type() == QEvent::Resize) {
        qDebug() << "[-] eventFilter";
        // Before sync, we should repaint the graphic view unless it's not
        // right
        tlgView->addSignalsToGview(signalsTreeView->rootIndex(),signalsTreeView->rootIndex());
        tlgView->update();
        syncWithTreeView();
        tlgView->update();
        tlgView->show();
        this->update();
        this->show();
    }

    //std::cout << "[-] end eventFilter" << std::endl;
    return QScrollArea::eventFilter(obj, ev);
}

void AdjustingScrollArea::syncWithTreeView() {
    return;
    qDebug() << "[-] syncWithTreeView" ;
    // Problem is when we enter this function, sometimes, the SignalsTreeView scrollbar
    // range is not yet updated.
    // So first, update signalsTreeView
    //int signalsNb = tlgView->getSignalsNb();

    signalsTreeView->update();
    signalsTreeView->verticalScrollBar()->update();
    //update();
    //verticalScrollBar()->update();

    //int eltNb = treeCount((QTreeWidget *) signalsTreeView);

    syncMutex.lock();
    /*std::cout << "SignalsTreeView  height " << signalsTreeView->height() << std::endl;
    std::cout << "Scrollarea  height " << this->height() << std::endl;
    std::cout << "gview  height " << tlgView->height() << std::endl;
*/
    // First block the scrollArea vertical bar signals
    this->verticalScrollBar()->blockSignals(true);
    // First calculate the percentage of move on the signalsTreeView
    double  min1 = (double) signalsTreeView->verticalScrollBar()->minimum();
    double  max1 = (double) signalsTreeView->verticalScrollBar()->maximum();
    if ((max1 - min1) <= 0) {
        qDebug() << "min-max " << min1 << " " << max1 ;
        qDebug() << "Error no sync possible: max1 -min1 <=0";
        syncMutex.unlock();
        return;
    } else {
        // Then get min and max of the   Vartical bar
        double min2 = (double) this->verticalScrollBar()->minimum();
        double max2 = (double) this->verticalScrollBar()->maximum();
        double factor = (max2 - min2) / (max1-min1) ;
        qDebug() << "min-max " << min1 << " " << max1 << " " << min2 << " " << max2;
        qDebug() << "factor " << factor;
        double value4= factor * (double) signalsTreeView->verticalScrollBar()->value();

        /*if ((value4 > (max2-min2)/3.0) && (signalsNb>0))
          value4= value4 + (max2-min2)/signalsNb;
        */
        double value3= round(value4);
        int value2 = value3;
        qDebug() << "signals slider value" << signalsTreeView->verticalScrollBar()->value();
        qDebug() << "scrollarea slider value (double) " << value4;
        qDebug() << "round'scrollarea slider value) (double) " << value3;
        qDebug() << "scrollarea slider value (int)" << value2;
        // set the scrollArea scroll bar value
        this->verticalScrollBar()->setValue(value2);

        // update widget position
        QPoint topLeft = viewport()->rect().topLeft();
        qDebug() << "topLeft" << topLeft.y() ;
        this->widget()->move(0, topLeft.y() - value2);

        // Finally unblock scrollarea signals
        this->verticalScrollBar()->blockSignals(false);
        this->update();
        this->show();
    }
    syncMutex.unlock();
    qDebug() << "End syncWithTreeView";
}

void AdjustingScrollArea::updateWidgetPosition(){
    qDebug() << "[-] updateWidgetPosition" ;
    int vvalue = verticalScrollBar()->value();
    QPoint topLeft = viewport()->rect().topLeft();

    widget()->move(0, topLeft.y() - vvalue);
}

void AdjustingScrollArea::setWidget(QWidget *w) {
    QScrollArea::setWidget(w);
    // It happens that QScrollArea already filters widget events,
    // but that's an implementation detail that we shouldn't rely on.
    w->installEventFilter(this);
}

int AdjustingScrollArea::treeCount(QTreeWidget *tree, QTreeWidgetItem *parent)
//runs over the hierarchy of a QTreeWidget and counts all visible items.
{
    int count = 0;
    if (parent == 0) {
        int topCount = tree->topLevelItemCount();
        for (int i = 0; i < topCount; i++) {
            QTreeWidgetItem *item = tree->topLevelItem(i);
            if (item->isExpanded()) {
                count += treeCount(tree, item);
            }
        }
        count += topCount;
    } else {
        int childCount = parent->childCount();
        for (int i = 0; i < childCount; i++) {
            QTreeWidgetItem *item = parent->child(i);
            if (item->isExpanded()) {
                count += treeCount(tree, item);
            }
        }
        count += childCount;
    }
    return count;
}

