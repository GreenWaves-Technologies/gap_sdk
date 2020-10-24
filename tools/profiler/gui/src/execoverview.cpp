#include <iostream>
#include "execoverview.hpp"

#include "backend_interface.hpp"



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

  chart->removeAllSeries();
  auto v = get_cycles_per_pe();
  series = new QBarSeries();
  QBarSet* set = new QBarSet("PE");
  float vmin=0.0;
  float vmax=0.0;
  for (uint i = 0; i < v.size(); i++){
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
  }
  series->append(set);
  chart->addSeries(series);
  //chart->createDefaultAxes();
  QValueAxis *axisY;
  if (chart->axisY()==nullptr) {
    axisY = new QValueAxis;
    chart->setAxisY(axisY);
  }
  else 
    axisY=(QValueAxis *)(chart->axisY());
  axisY->setRange(vmin, vmax);
  axisY->setTickCount(5);
  axisY->setLabelFormat("%'\''d \n");

  chart->legend()->setVisible(true);
  chart->legend()->setAlignment(Qt::AlignRight);
}
