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

#include <iostream>
#include "execoverview.hpp"

#include "backend_interface.hpp"
#include <QBarCategoryAxis>
#include <QDebug>
#include <QColor>
#include <random>


QT_CHARTS_USE_NAMESPACE

ExecOverview::ExecOverview(QWidget* parent){
  this->setParent(parent);
  chart = new QChart();
  chart->setTheme(QChart::ChartThemeBrownSand);
  chart->setTitleFont(QFont("Arial", 22));
  chart->setTitle("Total number of cycles per PE");
  QChartView *chartView = new QChartView(chart);
  chartView->setRenderHint(QPainter::Antialiasing);

  QHBoxLayout* layout = new QHBoxLayout;
  layout->addWidget(chartView);
  setLayout(layout);

  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(refresh()));
  timer->start(2000);
  refresh();
}

ExecOverview::~ExecOverview(){
  delete timer;
}

void ExecOverview::refresh(){
  QColor colors[8] = {QColor(196,216,46), QColor(119,187,0),
                  QColor(17,158,0), QColor(17,119,0), QColor(17,102,0),
                  QColor(0,85,0),QColor(0,51,0), QColor(0,17,0)};

  chart->removeAllSeries();
  auto v = get_cycles_per_pe();
  series = new QBarSeries();

  float vmin=0.0;
  float vmax=0.0;


  for (uint i = 0; i < v.size(); i++){
    if (i==8)
      break;
    QBarSet* set = new QBarSet("nothing");

    set->setLabel(QString("PE")+ QString::number(i));

    set->setColor(colors[i]);
    *set << v[i];
    if (i==0) {
      vmin=v[i];
      vmax=v[i];
    } else {
      if (v[i] < vmin){
        vmin=v[i];
      }
      if (v[i] > vmax){
        vmax=v[i];
      }
    }
    series->append(set);
  }


  chart->addSeries(series);
  //chart->setAnimationOptions(QChart::SeriesAnimations);

  chart->legend()->setVisible(true);
  chart->legend()->setAlignment(Qt::AlignBottom);

  QList<QAbstractAxis*> axisy_list = chart->axes(Qt::Vertical);

  if (axisy_list.isEmpty()) {
    axisy_list.append(new QValueAxis);
  }

  QValueAxis* axisY= static_cast<QValueAxis*>(axisy_list.back());

  axisY->setRange(vmin, vmax);
  axisY->setTickCount(5);
  axisY->setLabelFormat("%'\''d \n");
}
