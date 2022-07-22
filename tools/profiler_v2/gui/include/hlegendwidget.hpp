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

#ifndef HLEGENDWIDGET_HPP
#define HLEGENDWIDGET_HPP

#include <QPaintEvent>
#include <QWidget>
#include <QPen>
#include "tlgview.hpp"

// This class contains the horizontal time legend of the graphics view. 
class HLegendWidget : public QWidget
{
    Q_OBJECT

public:

    HLegendWidget(TLGView* gview);
    int xGridMin;
    int xGridMax;
    int xGridInc;
    void setWindowInterval(uint64_t start_ts, uint64_t end_ts);
protected:
    void paintEvent(QPaintEvent* event) override;

private:
    TLGView* gview;
    QPen pen;
    uint64_t t0;// first TLGView window point timestamp
    uint64_t t1;// last TLGView  window point timestamp

};

#endif // HLEGENDWIDGET_HPP
