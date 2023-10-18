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

    if (!dbQuery->isActive())
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
