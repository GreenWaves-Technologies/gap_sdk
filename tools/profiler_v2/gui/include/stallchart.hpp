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


#ifndef STALLCHART_HPP
#define STALLCHART_HPP

#include <QWidget>
#include <QFrame>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QHBoxLayout>

// Class used fo the "Stall reasons" tab in the functions tab
class StallChart : public QWidget
{
    Q_OBJECT

public:
    StallChart(QWidget* parent);
    void construct(std::string functionName);

private:
    std::string name;
    QtCharts::QBarSeries *series = nullptr;
    QtCharts::QChart *chart = nullptr;
};

#endif  //STALLCHART_HPP
