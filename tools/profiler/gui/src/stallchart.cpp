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


#include "stallchart.hpp"
#include "backend_interface.hpp"

#include <iostream>
#include <QBarCategoryAxis>
#include <QValueAxis>

QT_CHARTS_USE_NAMESPACE

StallChart::StallChart(QWidget* parent){
  this->setParent(parent);
  series = new QBarSeries();

  chart = new QChart();
  chart->addSeries(series);
  chart->setTheme(QChart::ChartThemeBrownSand);

  QChartView *chartView = new QChartView(chart);
  chartView->setRenderHint(QPainter::Antialiasing);

  QHBoxLayout* layout = new QHBoxLayout;
  layout->addWidget(chartView);
  setLayout(layout);
}

void StallChart::construct(std::string functionName){
  if (functionName == name) return;
  const Function_stat* fs = get_function_stat(functionName);
  if (fs == nullptr){
    std::cout << "[-] error: impossible to get stall stat for function ";
    std::cout << functionName << std::endl;
    return;
  }
  name = functionName;

  chart->removeAllSeries();
  uint64_t sum = 0;
  for (int i = 0; i < N_STALL_REASONS; i++){
    sum += (uint64_t) fs->stall_info.lost_cycles[i];
  }
  if (sum == 0){
      chart->setTitle("No stall reasons identified for that function");
      return;
  }
  series = new QBarSeries();
  QBarSet* set = new QBarSet("cycles");
  QStringList categories;
  *set << fs->tot_time;
  categories << "Total time";
  // TODO: handle case fs->tot_time = 0...
  for (int i = 0; i < N_STALL_REASONS; i++){
    /*series->append((std::string(stall_strings[i]) + " " +
      std::to_string(100 * ss->lost_cycles[i] / sum) + "%").c_str(),
      ss->lost_cycles[i]);*/
    *set << fs->stall_info.lost_cycles[i];
    categories << (std::string(stall_strings[i]) + " " +
      std::to_string(100 * fs->stall_info.lost_cycles[i] / fs->tot_time) + "%").c_str();
  }
  series->append(set);
  /*for (auto& c: series->slices()){
    c->setLabelVisible();
  }*/


  /*QValueAxis *axisY = new QValueAxis();
  axisY->setRange(0,15);
  chart->addAxis(axisY, Qt::AlignLeft);
  series->attachAxis(axisY);*/

  chart->setTitle(functionName.c_str());
  chart->setTitleFont(QFont("Arial", 22));
  chart->addSeries(series);

  chart->createDefaultAxes();
  QBarCategoryAxis *axisX = new QBarCategoryAxis();
  axisX->append(categories);
  auto l = chart->axes(Qt::Horizontal);
  if (! l.empty()) chart->removeAxis(l.front());
  chart->addAxis(axisX, Qt::AlignBottom);
  series->attachAxis(axisX);

  chart->legend()->setVisible(true);
  chart->legend()->setAlignment(Qt::AlignRight);
}
