#include "../headers/database.h"

Database::Database(QObject * parent)
    : QObject{parent}
{
    dataBase = new QSqlDatabase();
    dbQuery = new QSqlQuery();
    qModel = new QSqlQueryModel();
}

Database::~Database()
{
    delete dataBase;
    delete dbQuery;
    delete qModel;
}

void Database::recieve_connectToDBase(QMap<connection_info, QString> data)
{
    qDebug() << "DEBUG: in Database::recieve_connectToDBase";

    emit signal_connectionStatus(connectToDBase(data));
}

void Database::recieve_disconnectFromDBase(QMap<connection_info, QString> data)
{
    qDebug() << "DEBUG: in Database::recieve_disconnectFromDBase";

    *dataBase = QSqlDatabase::database(data[dbase]);
    dataBase->close();
}

// Recieveing request to send list of airports
void Database::recieve_requestAirportList()
{
    qDebug() << "DEBUG: in Database::recieve_requestAirportList";

    QString query = "select airport_name->>'ru' from bookings.airports_data ad";

    makeRequest(query);
}

void Database::recieve_buildTable(QString query)
{
    qDebug() << "DEBUG: in Database::recieve_buildTable";

    // BUILD TABLE WITH AVAILABLE FLIGHTS

    *dbQuery = QSqlQuery(*dataBase);

    if (dbQuery->exec(query))
    {

        qModel->setQuery(std::move(*dbQuery));
        qModel->setHeaderData(0, Qt::Horizontal, QObject::tr("НОМЕР РЕЙСА"));
        qModel->setHeaderData(1, Qt::Horizontal, QObject::tr("ПОРТ ВЫЛЕТА"));
        qModel->setHeaderData(2, Qt::Horizontal, QObject::tr("ПОРТ НАЗНАЧЕНИЯ"));
        qModel->setHeaderData(3, Qt::Horizontal, QObject::tr("ВЫЛЕТ"));
        qModel->setHeaderData(4, Qt::Horizontal, QObject::tr("ПРИБЫТИЕ"));

        emit signal_paintTable(qModel);
    }
    else
    {
        emit signal_returnError(dbQuery->lastError());
    }
}

void Database::recieve_gatherData(QString _query, QString _port)
{
    gatherData(_query, _port);
}

QSqlError Database::connectToDBase(QMap<connection_info, QString> data)
{
    qDebug() << "DEBUG: in Database::connectToDBase";

    QSqlError err;

    addDataBase("QPSQL", data[dbase]);

    dataBase->setHostName(data[host]);
    dataBase->setPort(data[port].toInt());
    dataBase->setDatabaseName(data[dbase]);
    dataBase->setUserName(data[user]);
    dataBase->setPassword(data[password]);

    qDebug() << "DBHOST: " << data[host];
    qDebug() << "DBPORT: " << data[port];
    qDebug() << "DBNAME: " << data[dbase];
    qDebug() << "DBUSER " << data[user];
    qDebug() << "DBPWD: " << data[password];

    if (!dataBase->open())
    {
        emit signal_returnError(dataBase->lastError());
    }

    qDebug() << "DEBUG: CONNECTED TO: " << dataBase->databaseName();

    err = dataBase->lastError();

    qDebug() << "DEBUG: lastError : " << err.text();


    return err;
}

void Database::addDataBase(QString driver, QString nameDB)
{
    qDebug() << "DEBUG: in Database::addDataBase";

    *dataBase = QSqlDatabase::addDatabase(driver, nameDB);
}

void Database::sendAirportList(QVector<QString> list)
{
    qDebug() << "DEBUG: in Database::sendAirportList";

    emit signal_sendAirportList(list);
}

QSqlError Database::simpleDbQuery(QString query)
{
    qDebug() << "DEBUG: in Database::simpleDbQuery";

    qDebug() << "DEBUG: Processing database: " << dataBase->databaseName();

    // if (!dbQuery->isActive())
    *dbQuery = QSqlQuery(*dataBase);

    dbQuery->exec(query);

    return dbQuery->lastError();
}

QString Database::eraser(const QString & input)
{
    qDebug() << "DEBUG: in Database::eraser";

    QString tmp = input;

    tmp.erase(std::remove(tmp.end() - 1, tmp.end(), '}'), tmp.end());
    tmp.erase(std::remove(tmp.end() - 1, tmp.end(), '"'), tmp.end());

    QString::Iterator iter;

    for (auto it = tmp.begin(); it != tmp.end(); ++it)
    {
        if (*it == '"')
        {
            iter = it;
        }
    }

    tmp.erase(tmp.begin(), iter + 1);

    return tmp;
}

void Database::getDataFromDB()
{
    qDebug() << "DEBUG: in Database::getDataFromDB";

    QVector<QString> airports;
    int count = 0;

    while(dbQuery->next())
    {

        ++count;
        QString tmp = dbQuery->value(0).toString();
        // tmp = eraser(tmp);
        qDebug() << "DEBUG: Value #" << QString::number(count) << ", String: " << tmp;
        airports.push_back(tmp);
    }

    airports.resize(count);
    airports.sort();

    emit signal_sendAirportList(airports);
}

void Database::makeRequest(QString query)
{
    qDebug() << "DEBUG: in Database::makeRequest";

    QSqlError result = simpleDbQuery(query);

    if (result.type() == QSqlError::NoError)
    {
        getDataFromDB();
    }
    else
    {
        emit signal_returnError(result);
    }
}

// Gather stat data on selected port using query
void Database::gatherData(QString _query, QString _port)
{
    qDebug() << "DEBUG: in Database::gatherData";

    QSqlError result = simpleDbQuery(_query);

    if (result.type() == QSqlError::NoError)
    {
        getStatFromDB(_port);
    }
    else
    {
        emit signal_returnError(result);
    }
}

void Database::getStatFromDB(QString _port)
{
    qDebug() << "DEBUG: in Database::getStatFromDB";

    QMap<QString, QMap<int, QVector<QVector<int>>>> flightData;
    QMap<int, QVector<QVector<int>>> departures;
    QMap<int, QVector<QVector<int>>> arrivals;

    // flight_no | arrival_port | departure_port | departure_date                 | arrival_date
    // PG###     | Шереметьево  | Пулково        | 2017-06-22 19:05:00.000 +0300  | 2017-06-22 19:35:00.000 +0300
    // 0         | 1            | 2              | 3                              | 4

    qDebug() << "DEBUG: running data process from query";

    int counter = 0;

    while(dbQuery->next())
    {
        counter++;

        // DEBUG
        qDebug() << dbQuery->value(0).toString() << " | "
                 << dbQuery->value(1).toString() << " | "
                 << dbQuery->value(2).toString() << " | "
                 << dbQuery->value(3).toString() << " | "
                 << dbQuery->value(4).toString();

        QDate departure_date = dbQuery->value(3).toDate();
        QDate arrival_date = dbQuery->value(4).toDate();

        QVector<int> tmp = {    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0     }; // 31 day fresh tmp

        if (dbQuery->value(1).toString() == _port)      // arrival
        {
            if (arrivals[arrival_date.year()].isEmpty())
            {
                arrivals.insert(arrival_date.year(), {tmp, tmp, tmp, tmp, tmp, tmp, tmp, tmp, tmp, tmp, tmp, tmp});
                arrivals[arrival_date.year()][arrival_date.month() - 1][arrival_date.day() - 1]++;
            }
            else
            {
                arrivals[arrival_date.year()][arrival_date.month() - 1][arrival_date.day() - 1]++;
            }
        }
        else if (dbQuery->value(2).toString() == _port) // departure
        {
            if (departures[departure_date.year()].isEmpty())
            {
                departures.insert(departure_date.year(), {tmp, tmp, tmp, tmp, tmp, tmp, tmp, tmp, tmp, tmp, tmp, tmp});
                departures[departure_date.year()][departure_date.month() - 1][departure_date.day() - 1]++;
            }
            else
            {
                departures[departure_date.year()][departure_date.month() - 1][departure_date.day() - 1]++;
            }
        }
    }

    flightData.insert("departures", departures);
    flightData.insert("arrivals", arrivals);

    qDebug() << "DEBUG: ROWS READ: " << QString::number(counter);

    emit signal_sendStatData(flightData);
}
