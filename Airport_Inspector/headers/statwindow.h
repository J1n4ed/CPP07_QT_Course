#ifndef STATWINDOW_H
#define STATWINDOW_H

#include <QDialog>
#include <QWidget>
#include <QDate>
#include <QTime>
#include <QtCharts>
#include <QChartView>
#include <QStackedBarSeries>
#include <QSplineSeries>
#include <QMap>
#include <QString>
#include <QtConcurrent>

#include "enumerations.h"

namespace Ui {
class Statwindow;
}

class Statwindow : public QDialog
{
    Q_OBJECT

public:

    explicit Statwindow(QWidget *parent = nullptr);
    ~Statwindow();  

private slots:

    void on_pb_close_clicked();
    void on_tab_statistics_tabBarClicked(int index);
    void on_cb_select_year_currentIndexChanged(const QString &arg1);
    void on_cb_select_year_currentIndexChanged(int index);
    void on_cb_select_month_currentIndexChanged(int index);

    void on_pb_paint_year_clicked();

    void on_pb_paint_month_clicked();

signals:

    void signal_gatherData(QString, QString);

public slots:

    void recieve_sendDataFromMain(QString, QDate, QTime, QDate, QTime);
    void recieve_sendStatData(QMap<QString, QMap<int, QVector<QVector<int>>>>);

private:

    Ui::Statwindow *ui;

    QString port;
    QDate date_from;
    QTime time_from;
    QDate date_till;
    QTime time_till;

    QStackedBarSeries * yearGraph;
    QChart * chart;
    QChartView * chartView;
    QGridLayout * layout;

    QSplineSeries * monthGraphArrivals;
    QSplineSeries * monthGraphDepartures;
    QChart * monthChart;
    QChartView * monthChartView;
    QGridLayout * monthLayout;

    bool statLoaded = false;
    bool updateOnlyYear = false;
    bool updateOnlyMonth = false;

    //  arr & dep    / year / Vector of monthes / Vector of days (int)
    QMap<QString, QMap<int, QVector<QVector<int>>>> flightData;


    void gatherData(QString);
    void paintYearStat();
    void paintMonthStat();
    void paintYearGraph();
    void paintMonthGraph();
};

#endif // STATWINDOW_H
