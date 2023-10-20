#include "../headers/statwindow.h"
#include "./ui_statwindow.h"

Statwindow::Statwindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Statwindow)
{
    ui->setupUi(this);

    yearGraph = new QStackedBarSeries;
    chart = new QChart;
    layout = new QGridLayout;

    monthGraphArrivals = new QSplineSeries;
    monthGraphDepartures = new QSplineSeries;
    monthChart = new QChart;
    monthLayout = new QGridLayout;

    this->setWindowTitle("Статистика загруженности");    

    ui->tab_year->setWindowTitle("Статистика за год");
    ui->tab_month->setWindowTitle("Статистика за месяц");
}

Statwindow::~Statwindow()
{
    delete yearGraph;
    delete chart;
    delete chartView;
    delete layout;
    delete monthGraphArrivals;
    delete monthGraphDepartures;
    delete monthChart;
    delete monthLayout;
    delete monthChartView;

    delete ui;    
}

void Statwindow::on_pb_close_clicked()
{
    qDebug() << "DEBUG: in Statwindow::on_pb_close_clicked";

    this->close();
}

void Statwindow::recieve_sendDataFromMain(QString _port, QDate _date_from, QTime _time_from, QDate _date_till, QTime _time_till)
{
    qDebug() << "DEBUG: in Statwindow::recieve_sendDataFromMain";

    // Data sent from main form

    port = _port;

    date_from = _date_from;
    time_from = _time_from;
    date_till = _date_till;
    time_till = _time_till;

    ui->lb_port_info->setText(port);
    ui->lb_from_date->setText(date_from.toString() + " " + time_from.toString());
    ui->lb_till_date->setText(date_till.toString() + " " + time_till.toString());

    int index = -1;

    if (date_from.year() == 2016)
        index = 0;
    if (date_from.year() == 2017)
        index = 1;

    ui->cb_select_year->setCurrentIndex(index);
    ui->cb_select_month->setCurrentIndex(date_from.month() - 1);

    gatherData(port);
}

// get stats and form initial data in user view
void Statwindow::recieve_sendStatData(QMap<QString, QMap<int, QVector<QVector<int>>>> _flightData)
{
    qDebug() << "DEBUG: in Statwindow::recieve_sendStatData";

    flightData = _flightData;
    statLoaded = true;

    paintYearStat();
    paintMonthStat();
}

// Fetch BD data on flights from selected port
void Statwindow::gatherData(QString _port)
{
    qDebug() << "DEBUG: in Statwindow::gatherData";

    QString query = "SELECT flight_no, ad.airport_name->>'ru', ad2.airport_name->>'ru', scheduled_departure, scheduled_arrival "
                    "FROM bookings.flights f JOIN bookings.airports_data ad ON f.arrival_airport = ad.airport_code "
                    "JOIN bookings.airports_data ad2 ON f.departure_airport = ad2.airport_code "
                    "WHERE ad.airport_name->>'ru' = '" + _port + "' OR ad2.airport_name->>'ru' = '" + _port + "' "
                    "ORDER BY scheduled_departure";

    // flight_no | arrival_port | departure_port | departure_date                 | arrival_date
    // PG###     | Шереметьево  | Пулково        | 2017-06-22 19:05:00.000 +0300  | 2017-06-22 19:35:00.000 +0300

    emit signal_gatherData(query, _port);
}

void Statwindow::paintYearStat()
{
    qDebug() << "DEBUG: in Statwindow::paintYearStat";

    ui->te_y_info->clear();

    // Отправления
    ui->te_y_info->append("Статистика отправлений за " + ui->cb_select_year->currentText() + " год: ");

    int allCount = 0;

    //  arr & dep    / year / Vector of monthes / Vector of days (int)
    // QMap<QString, QMap<int, QVector<QVector<int>>>> flightData;

    //    flightData.insert("departures", departures);
    //    flightData.insert("arrivals", arrivals);

    for (int month = 0; month < 12; ++month)
    {
        int counter = 0;

        for (int day = 0; day < 31; ++day)
        {
            counter += flightData.value("departures").value(ui->cb_select_year->currentText().toInt())[month][day];
        }

        switch (month)
        {
        case 0:
        {
            ui->te_y_info->append("- за Январь: " + QString::number(counter) + " рейсов");

            break;
        }
        case 1:
        {
            ui->te_y_info->append("- за Февраль: " + QString::number(counter) + " рейсов");

            break;
        }
        case 2:
        {
            ui->te_y_info->append("- за Март: " + QString::number(counter) + " рейсов");

            break;
        }
        case 3:
        {
            ui->te_y_info->append("- за Апрель: " + QString::number(counter) + " рейсов");

            break;
        }
        case 4:
        {
            ui->te_y_info->append("- за Май: " + QString::number(counter) + " рейсов");

            break;
        }
        case 5:
        {
            ui->te_y_info->append("- за Июнь: " + QString::number(counter) + " рейсов");

            break;
        }
        case 6:
        {
            ui->te_y_info->append("- за Июль: " + QString::number(counter) + " рейсов");

            break;
        }
        case 7:
        {
            ui->te_y_info->append("- за Август: " + QString::number(counter) + " рейсов");

            break;
        }
        case 8:
        {
            ui->te_y_info->append("- за Сентабрь: " + QString::number(counter) + " рейсов");

            break;
        }
        case 9:
        {
            ui->te_y_info->append("- за Октябрь: " + QString::number(counter) + " рейсов");

            break;
        }
        case 10:
        {
            ui->te_y_info->append("- за Ноябрь: " + QString::number(counter) + " рейсов");

            break;
        }
        case 11:
        {
            ui->te_y_info->append("- за Декабрь: " + QString::number(counter) + " рейсов");

            break;
        }
        default:
            qDebug() << "Unexcepted behavior, index out of bounds!";
            break;
        }

        allCount += counter;
    }

    ui->te_y_info->append("ВСЕГО ЗА ГОД: " + QString::number(allCount) + " рейсов");


    // ПРИБЫТИЯ

    ui->te_y_info->append("   ------    ");

    ui->te_y_info->append("Статистика прибытий за " + ui->cb_select_year->currentText() + " год: ");

    allCount = 0;

    for (int month = 0; month < 12; ++month)
    {
        int counter = 0;

        for (int day = 0; day < 31; ++day)
        {
            counter += flightData.value("arrivals").value(ui->cb_select_year->currentText().toInt())[month][day];
        }

        switch (month)
        {
        case 0:
        {
            ui->te_y_info->append("- за Январь: " + QString::number(counter) + " рейсов");

            break;
        }
        case 1:
        {
            ui->te_y_info->append("- за Февраль: " + QString::number(counter) + " рейсов");

            break;
        }
        case 2:
        {
            ui->te_y_info->append("- за Март: " + QString::number(counter) + " рейсов");

            break;
        }
        case 3:
        {
            ui->te_y_info->append("- за Апрель: " + QString::number(counter) + " рейсов");

            break;
        }
        case 4:
        {
            ui->te_y_info->append("- за Май: " + QString::number(counter) + " рейсов");

            break;
        }
        case 5:
        {
            ui->te_y_info->append("- за Июнь: " + QString::number(counter) + " рейсов");

            break;
        }
        case 6:
        {
            ui->te_y_info->append("- за Июль: " + QString::number(counter) + " рейсов");

            break;
        }
        case 7:
        {
            ui->te_y_info->append("- за Август: " + QString::number(counter) + " рейсов");

            break;
        }
        case 8:
        {
            ui->te_y_info->append("- за Сентабрь: " + QString::number(counter) + " рейсов");

            break;
        }
        case 9:
        {
            ui->te_y_info->append("- за Октябрь: " + QString::number(counter) + " рейсов");

            break;
        }
        case 10:
        {
            ui->te_y_info->append("- за Ноябрь: " + QString::number(counter) + " рейсов");

            break;
        }
        case 11:
        {
            ui->te_y_info->append("- за Декабрь: " + QString::number(counter) + " рейсов");

            break;
        }
        default:
            qDebug() << "Unexcepted behavior, index out of bounds!";
            break;
        }

        allCount += counter;
    }

    ui->te_y_info->append("ВСЕГО ЗА ГОД: " + QString::number(allCount) + " рейсов");

    paintMonthStat();
}

// Paint month stats day-by-day
void Statwindow::paintMonthStat()
{
    qDebug() << "DEBUG: in Statwindow::paintMonthStat";

    ui->te_m_info->clear();

    int currentYear = ui->cb_select_year->currentText().toInt();
    int currentMonth = ui->cb_select_month->currentIndex();

    ui->te_m_info->append("Статистика отправлений за " + QString::number(currentMonth + 1) + "." + QString::number(currentYear));

    for (int day = 0; day < 31; ++day)
    {
        if (flightData.value("departures").value(currentYear)[currentMonth][day] != 0)
        {
            ui->te_m_info->append("За " + QString::number(day + 1)+ "."
                                        + QString::number(currentMonth + 1) + "."
                                        + QString::number(currentYear) + ": "
                                        + QString::number(flightData.value("departures").value(currentYear)[currentMonth][day])
                                        + " рейсов");
        }
    }

    ui->te_m_info->append("   ------    ");

    ui->te_m_info->append("Статистика прибытий за " + QString::number(currentMonth + 1) + "." + QString::number(currentYear));

    for (int day = 0; day < 31; ++day)
    {
        if (flightData.value("arrivals").value(currentYear)[currentMonth][day] != 0)
        {
            ui->te_m_info->append("За " + QString::number(day + 1)+ "."
                                        + QString::number(currentMonth + 1) + "."
                                        + QString::number(currentYear) + ": "
                                        + QString::number(flightData.value("departures").value(currentYear)[currentMonth][day])
                                        + " рейсов");
        }
    }
}

// YEAR GRAPH DISPLAY
void Statwindow::paintYearGraph()
{
    qDebug() << "DEBUG: in Statwindow::paintYearGraph";

    //  arr & dep    / year / Vector of monthes / Vector of days (int)
    // QMap<QString, QMap<int, QVector<QVector<int>>>> flightData;

    //    flightData.insert("departures", departures);
    //    flightData.insert("arrivals", arrivals);

    ui->viewport_year->update();


    QBarSet * set_year_departures = new QBarSet("Отправления");
    QBarSet * set_year_arrivals = new QBarSet("Прибытия");

    yearGraph->clear();

    int i = 0;

    int currentYear = ui->cb_select_year->currentText().toInt();

    for (int month = 0; month < 12; ++month)
    {
        int counter = 0;

        for (int day = 0; day < 31; ++day)
        {
            counter += flightData.value("departures").value(ui->cb_select_year->currentText().toInt())[month][day];
        }

            *set_year_departures << counter;
    }

    for (int month = 0; month < 12; ++month)
    {
        int counter = 0;

        for (int day = 0; day < 31; ++day)
        {
            counter += flightData.value("arrivals").value(ui->cb_select_year->currentText().toInt())[month][day];
        }

        *set_year_arrivals << counter;
    }

    yearGraph->append(set_year_departures);
    yearGraph->append(set_year_arrivals);

    if (!updateOnlyYear)
    {
        chart->addSeries(yearGraph);
        chart->setTitle("График загруженности аэропорта " + port);
        chart->setAnimationOptions(QChart::SeriesAnimations);

        QStringList categories {"Январь", "Февраль", "Март", "Апрель", "Май", "Июнь", "Июль", "Август", "Сентябрь", "Октябрь", "Ноябрь", "Декабрь"};
        auto axisX = new QBarCategoryAxis;
        axisX->append(categories);
        chart->addAxis(axisX, Qt::AlignBottom);
        yearGraph->attachAxis(axisX);

        auto axisY = new QValueAxis;
        chart->addAxis(axisY, Qt::AlignLeft);
        yearGraph->attachAxis(axisY);

        chart->legend()->setVisible(true);
        chart->legend()->setAlignment(Qt::AlignBottom);

        chartView = new QChartView(chart);

        chartView->setRenderHint(QPainter::Antialiasing);

        ui->viewport_year->setRenderHint(QPainter::Antialiasing);
        ui->viewport_year->setLayout(layout);
        layout->addWidget(chartView);

        chartView->show();
        updateOnlyYear = true;
    }
}

// MONTH GRAPH DISPLAY
void Statwindow::paintMonthGraph()
{

    qDebug() << "DEBUG: in Statwindow::paintMonthGraph";

    //  arr & dep    / year / Vector of monthes / Vector of days (int)
    // QMap<QString, QMap<int, QVector<QVector<int>>>> flightData;

    //    flightData.insert("departures", departures);
    //    flightData.insert("arrivals", arrivals);

    ui->viewport_month->update();    

    monthGraphArrivals->clear();
    monthGraphDepartures->clear();

    monthGraphArrivals->setName("Прибытия");
    monthGraphDepartures->setName("Отправления");

    int i = 0;

    int currentMonth = ui->cb_select_month->currentIndex();

    int max = 0;

    for (int day = 0; day < 31; ++day)
    {
        monthGraphDepartures->append(day, flightData.value("departures").value(ui->cb_select_year->currentText().toInt())[currentMonth][day]);

        if (flightData.value("departures").value(ui->cb_select_year->currentText().toInt())[currentMonth][day] > max)
        {
            max = flightData.value("departures").value(ui->cb_select_year->currentText().toInt())[currentMonth][day];
        }
    }

    for (int day = 0; day < 31; ++day)
    {
        monthGraphArrivals->append(day, flightData.value("arrivals").value(ui->cb_select_year->currentText().toInt())[currentMonth][day]);

        if (flightData.value("arrivals").value(ui->cb_select_year->currentText().toInt())[currentMonth][day] > max)
        {
            max = flightData.value("arrivals").value(ui->cb_select_year->currentText().toInt())[currentMonth][day];
        }
    }

    auto axisX = new QBarCategoryAxis;
    auto axisY = new QValueAxis;

    axisY->setMax(max + 3);

    if (!updateOnlyMonth)
    {
        monthChart->addSeries(monthGraphDepartures);
        monthChart->addSeries(monthGraphArrivals);
        monthChart->setTitle("График загруженности аэропорта " + port);
        monthChart->setAnimationOptions(QChart::SeriesAnimations);        

        QStringList categories {"01", "02", "03", "04", "05", "06", "07", "08", "09", "10",
                                "11", "12", "13", "14", "15", "16", "17", "18", "19", "20",
                                "21", "22", "23", "24", "25", "26", "27", "28", "29", "30",
                                "31"};


        axisX->append(categories);
        monthChart->addAxis(axisX, Qt::AlignBottom);
        monthGraphArrivals->attachAxis(axisX);
        monthGraphDepartures->attachAxis(axisX);


        monthChart->addAxis(axisY, Qt::AlignLeft);
        monthGraphArrivals->attachAxis(axisY);
        monthGraphDepartures->attachAxis(axisY);

        monthChart->legend()->setVisible(true);
        monthChart->legend()->setAlignment(Qt::AlignBottom);

        monthChartView = new QChartView(monthChart);

        monthChartView->setRenderHint(QPainter::Antialiasing);

        ui->viewport_month->setRenderHint(QPainter::Antialiasing);
        ui->viewport_month->setLayout(monthLayout);
        monthLayout->addWidget(monthChartView);

        monthChartView->show();

        updateOnlyMonth = true;
    }

    emit axisY->maxChanged(max + 3);

    monthChart->update();
}

// Клики под табам
void Statwindow::on_tab_statistics_tabBarClicked(int index)
{
    qDebug() << "DEBUG: in Statwindow::on_tab_statistics_tabBarClicked";

    qDebug() << "DEBUG: Clicked index = " << index;

    // index 0 - year
    // index 1 - month
}

void Statwindow::on_cb_select_year_currentIndexChanged(const QString &arg1)
{
    qDebug() << "DEBUG: in Statwindow::on_cb_select_year_currentIndexChanged - String";

    if (statLoaded)
        paintYearStat();
}

void Statwindow::on_cb_select_year_currentIndexChanged(int index)
{
    qDebug() << "DEBUG: in Statwindow::on_cb_select_year_currentIndexChanged - Int";

    if (statLoaded)
    {
        paintYearStat();
        paintMonthGraph();
    }
}


void Statwindow::on_cb_select_month_currentIndexChanged(int index)
{
    qDebug() << "DEBUG: in Statwindow::on_cb_select_month_currentIndexChanged - Int";

    if (statLoaded)
    {
        paintMonthStat();
        paintMonthGraph();
    }
}


void Statwindow::on_pb_paint_year_clicked()
{
    qDebug() << "DEBUG: in Statwindow::on_pb_paint_year_clicked";

    paintYearGraph();
}


void Statwindow::on_pb_paint_month_clicked()
{
    qDebug() << "DEBUG: in Statwindow::on_pb_paint_year_clicked";

    paintMonthGraph();
}

