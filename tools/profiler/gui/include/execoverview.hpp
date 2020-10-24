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
