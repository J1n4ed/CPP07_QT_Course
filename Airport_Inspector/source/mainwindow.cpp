#include "../headers/mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    about = new About(this);
    msg = new QMessageBox(this);
    stats = new Statwindow(this);

    database = new Database(this);

    this->setWindowTitle("Инспектор аэропортов, версия 1.0");

    ui->lb_connection_status->setStyleSheet("color:red");
    ui->lb_connection_status->setText("ОТКЛЮЧЕНО ОТ БАЗЫ ДАННЫХ");

    // ui->lb_selection->setText(">  Выберите рейс из таблицы рейсов  <"); optional to-do

    widget = new QWidget(this);
    QGridLayout * layout = new QGridLayout(widget);
    layout->addWidget(ui->lb_connection_status, 0, 0, 1, 1, Qt::AlignVCenter | Qt::AlignLeft);
    layout->addWidget(ui->lb_selection, 0, 1, 1, 1, Qt::AlignVCenter | Qt::AlignHCenter);
    layout->addWidget(ui->lb_connection_anim, 0, 2, 1, 1, Qt::AlignVCenter | Qt::AlignRight);
    ui->statusbar->addWidget(widget,1);

    ui->lb_connection_anim->setMaximumHeight(50);

    mov_halt = new QMovie(this);
    mov_halt->setFileName(":/images/process_halt");
    mov_go = new QMovie(this);
    mov_go->setFileName(":/images/process_go");
    ui->lb_connection_anim->setMovie(mov_halt);
    mov_halt->start(); // ON START

    pix = new QPixmap;
    pix->load(":/logo/logo");
    ui->lb_logo->setPixmap(pix->scaledToHeight(pix->height()/4));

    ui->pb_reconnect->setEnabled(false);
    ui->pb_search->setEnabled(false);

    ui->rb_incoming->setChecked(false);
    ui->rb_outgoing->setChecked(true);

    msg->setStandardButtons(QMessageBox::Ok);
    msg->setIcon(QMessageBox::Critical);
    msg->setWindowTitle("Сообщение об ошибке");
    msg->setText("PLACEHOLDER ERROR TITLE");
    msg->setInformativeText("PLACEHOLDER ERROR DESCRIPTION.");

    // SIGNAL-SLOTS INIT

    connect(this, &MainWindow::signal_connectToDBase, database, &Database::recieve_connectToDBase);
    connect(database, &Database::signal_connectionStatus, this, &MainWindow::recieve_connectionStatus);
    connect(this, &MainWindow::signal_disconnectFromDBase, database, &Database::recieve_disconnectFromDBase);
    connect(this, &MainWindow::signal_requestAirportList, database, &Database::recieve_requestAirportList);
    connect(database, &Database::signal_sendAirportList, this, &MainWindow::recieve_sendAirportList);
    connect(database, &Database::signal_returnError, this, &MainWindow::recieve_returnError);
    connect(this, &MainWindow::signal_buildTable, database, &Database::recieve_buildTable);
    connect(database, &Database::signal_paintTable, this, &MainWindow::recieve_paintTable);

    connect(ui->tb_content->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::on_tableViewSelection);

    // DB connection INIT

    connection_data.insert(host, "981757-ca08998.tmweb.ru");
    connection_data.insert(port, "5432");
    connection_data.insert(dbase, "demo");
    connection_data.insert(user, "netology_usr_cpp");
    connection_data.insert(password, "CppNeto3");

    auto con = [&](){connectToDB(connection_data);};
    auto run = QtConcurrent::run(con);    
}

MainWindow::~MainWindow()
{
    disconnectFromDB(connection_data);
    delete ui;
}

void MainWindow::connectToDB(QMap<connection_info, QString> connection_data)
{
    qDebug() << "DEBUG: in MainWindow::connectToDB";

    emit signal_connectToDBase(connection_data);
}

void MainWindow::disconnectFromDB(QMap<connection_info, QString> data)
{
    qDebug() << "DEBUG: in MainWindow::disconnectFromDB";

    emit signal_disconnectFromDBase(data);
}

void MainWindow::requestAirportList()
{
    qDebug() << "DEBUG: in MainWindow::requestAirportList";

    emit signal_requestAirportList();
}

void MainWindow::buildTable(QString modelQuery)
{
    emit signal_buildTable(modelQuery);
}

void MainWindow::on_action_about_triggered()
{
    qDebug() << "DEBUG: in MainWindow::on_action_about_triggered";

    about->show();
}

void MainWindow::on_action_close_triggered()
{
    qDebug() << "DEBUG: in MainWindow::on_action_close_triggered";

    this->close();
}

void MainWindow::on_pb_close_clicked()
{
    qDebug() << "DEBUG: in MainWindow::on_pb_close_clicked";

    on_action_close_triggered();
}

void MainWindow::on_pb_search_clicked()
{
    qDebug() << "DEBUG: in MainWindow::on_pb_search_clicked";

    /*
● Номер рейса
● Время вылета
● Аэропорт назначения
    */

//  select flight_no, airport_name->>'ru', scheduled_departure, scheduled_arrival
//  from bookings.flights f join bookings.airports_data ad on f.arrival_airport = ad.airport_code
//  where scheduled_departure BETWEEN timestamp '2016-12-12 23:55:00' AND '2017-02-02 12:00:00';

    QString timeBegin;
    QString timeEnd;

    // '2016-12-12 23:55:00'

    timeBegin = QString::number(ui->de_from_date->date().year()) + '-'
                + QString::number(ui->de_from_date->date().month()) + '-'
                + QString::number(ui->de_from_date->date().day()) + ' '
                + QString::number(ui->de_from_date->dateTime().time().hour()) + ':'
                + QString::number(ui->de_from_date->dateTime().time().minute()) + ':'
                + QString::number(ui->de_from_date->dateTime().time().second());

    timeEnd = QString::number(ui->de_till_date->date().year()) + '-'
              + QString::number(ui->de_till_date->date().month()) + '-'
              + QString::number(ui->de_till_date->date().day()) + ' '
              + QString::number(ui->de_till_date->dateTime().time().hour()) + ':'
              + QString::number(ui->de_till_date->dateTime().time().minute()) + ':'
              + QString::number(ui->de_till_date->dateTime().time().second());

    qDebug() << "DEBUG: selected start time: " << timeBegin;
    qDebug() << "DEBUG: selected end time: " << timeEnd;

    QString modelQuery = "SELECT flight_no, ad.airport_name->>'ru', ad2.airport_name->>'ru', scheduled_departure, scheduled_arrival "
                         "FROM bookings.flights f join bookings.airports_data ad on f.arrival_airport = ad.airport_code "
                         "JOIN bookings.airports_data ad2 on f.departure_airport = ad2.airport_code "
                         "WHERE scheduled_departure BETWEEN timestamp '" + timeBegin + "' AND '" + timeEnd + "'";

    if (ui->rb_incoming->isChecked())
    {
        modelQuery = modelQuery + " AND ad2.airport_name->>'ru' = '" + portSelection + "'";
    }
    else if (ui->rb_outgoing->isChecked())
    {
        modelQuery = modelQuery + " AND ad.airport_name->>'ru' = '" + portSelection + "'";
    }
    else
    {
        qDebug() << "DEBUG: unforseen behavior while performing radio button check!";
    }

    // auto con = [&](){buildTable(modelQuery);};
    // auto ft = QtConcurrent::run(con);

    buildTable(modelQuery);
}

void MainWindow::recieve_connectionStatus(QSqlError recievedStatus)
{
    qDebug() << "DEBUG: in MainWindow::recieve_connectionStatus";

    if (recievedStatus.type() == QSqlError::NoError)
    {
        ui->lb_connection_status->setStyleSheet("color:green");
        ui->lb_connection_status->setText("ПОДКЛЮЧЕНО К БАЗЕ ДАННЫХ");
        ui->pb_reconnect->setEnabled(false);        
        ui->pb_search->setEnabled(true);

        mov_halt->stop();
        ui->lb_connection_anim->setMovie(mov_go);
        mov_go->start();

        // list of airports for combobox requested
        requestAirportList();
    }
    else
    {
        msg->setText("ОШИБКА ПОДКЛЮЧЕНИЯ");
        msg->setInformativeText("Произошла ошибка подключения к базе данных.\nОшибка: " + recievedStatus.text());
        msg->exec();        
        ui->pb_reconnect->setEnabled(true);        
    }
}

// Got list of airports from database
void MainWindow::recieve_sendAirportList(QVector<QString> list)
{
    qDebug() << "DEBUG: in MainWindow::recieve_sendAirportList";

    for (const auto & element : list)
    {
        ui->cb_airport->addItem(element);
    }
}

void MainWindow::recieve_returnError(QSqlError result)
{
    qDebug() << "DEBUG: in MainWindow::recieve_returnError";

    msg->setText("ОШИБКА ЗАПРОСА");
    msg->setInformativeText("Произошла ошибка выполнения запроса к базе данных.\nОшибка: " + result.text());
    msg->exec();
}

void MainWindow::recieve_paintTable(QSqlQueryModel * model)
{
    // paint table on the UI

    qDebug() << "DEBUG: in MainWindow::recieve_paintTable";

    ui->tb_content->setModel(model);
    ui->tb_content->setEditTriggers(QAbstractItemView::NoEditTriggers);    
    ui->tb_content->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->tb_content->show();
}


void MainWindow::on_pb_reconnect_clicked()
{
    qDebug() << "DEBUG: in MainWindow::on_pb_reconnect_clicked";

    connectToDB(connection_data);
}


void MainWindow::on_pb_staticstics_clicked()
{
    qDebug() << "DEBUG: in MainWindow::on_pb_staticstics_clicked";

    stats->show();
}


void MainWindow::on_action_load_triggered()
{
    qDebug() << "DEBUG: in MainWindow::on_action_load_triggered";

    on_pb_staticstics_clicked();
}


void MainWindow::on_action_getports_triggered()
{
    qDebug() << "DEBUG: in MainWindow::on_action_getports_triggered";

    ui->cb_airport->clear();
    emit signal_requestAirportList();
}


void MainWindow::on_cb_airport_currentIndexChanged(int index)
{
    qDebug() << "DEBUG: in MainWindow::on_cb_airport_currentIndexChanged";

    portSelection = ui->cb_airport->currentText();

    qDebug() << "DEBUG: selection changed, current port = " << portSelection;
}

void MainWindow::on_tableViewSelection(const QItemSelection & col, const QItemSelection & row)
{
    auto index = row.at(1);
    qDebug() << "DEBUG: SELECTED - " << index;
}
