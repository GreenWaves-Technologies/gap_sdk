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

#ifndef EXEC_OVERVIEW_HPP
#define EXEC_OVERVIEW_HPP

#include <QWidget>
#include <QTimer>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QHBoxLayout>
#include <QValueAxis>

class ExecOverview : public QWidget {
  Q_OBJECT

public:
  ExecOverview(QWidget* parent);
  ~ExecOverview();

private:

  QtCharts::QBarSeries *series = nullptr;
  QtCharts::QChart *chart = nullptr;
  QTimer* timer;

private slots:
  void refresh();
};



#endif    //EXEC_OVERVIEW_HPP
