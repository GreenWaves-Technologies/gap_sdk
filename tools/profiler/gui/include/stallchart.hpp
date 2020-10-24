/*
 * Copyright (C) 2019 GreenWaves Technologies
 * All rights reserved.
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
