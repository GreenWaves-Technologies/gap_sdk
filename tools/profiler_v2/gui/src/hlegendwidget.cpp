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

#include "hlegendwidget.hpp"
#include "tlgview.hpp"

void HLegendWidget::paintEvent(QPaintEvent* event) {

    //std::cout << "[-] Paint HLegendWidget 1" << std::endl;
    //std::cout << "HL ZoomFactor " << gview->zoomFactor << std::endl;

    uint X = size().width();
    uint Y = gview->peHeight;

    QPainter painter(this);
    // color background in black
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.drawRect(rect());

    // DEfine white pen
    pen = QPen(Qt::white);
    pen.setWidthF(0);
    // Define gridPen with the blue/violet color

    QPen gridPen = QPen(QColor(qRgb(95,91,218)));
    //QPen gridPenMain = QPen(QColor(qRgb(245,23,7)));

    painter.setPen(gridPen);
    // Paint horizontal line of the legend
    painter.drawLine(0, gview->peHeight, X,Y );

    // Draw all the sticks -- Note xe add  pixel to the x abcisse as the
    // hLegned Widget is one pixel wider on both sides.
    for(std::vector<Stick>::iterator it = std::begin(gview->gridStick); it != std::end(gview->gridStick); ++it) {
        if (it->main){
            // This is a main gridStick
             painter.setPen(gridPen);
             painter.drawLine(it->x+1, it->y1, it->x+1, it->y2);
             // Paint the text rectangle with the tic time value
             painter.setPen(pen);
             QRect rect( it->x - gview->legendWidth / 2,
                         Y - gview->peHeight,
                         gview->legendWidth,
                         gview->peHeight - gview->ticSize - 1);
             painter.drawText(rect, Qt::AlignCenter, it->text);
        }
        else {
             painter.setPen(gridPen);
             painter.drawLine(it->x+1, it->y1, it->x+1, it->y2);
        }
    }

    //std::cout << "[-] End Paint HLegendWidget " << std::endl;
}

HLegendWidget::HLegendWidget(TLGView* gview): gview(gview) {

    //std::cout << "[-] create HLegendWidget 0" << std::endl;
    //this->setStyleSheet("background-color:black;");

}

void HLegendWidget::setWindowInterval(uint64_t start_ts, uint64_t end_ts){
    t0=start_ts;
    t1=end_ts;
}
