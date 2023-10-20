#ifndef DATABASE_H
#define DATABASE_H

#include <QTableWidget>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QMap>
#include <QVector>
#include <QSqlError>
#include <QtAlgorithms>
#include <QSqlQueryModel>
#include <QDate>

#include "enumerations.h"

class Database : public QObject
{
    Q_OBJECT

public:

    explicit Database(QObject *parent = nullptr);
    ~Database();

    void connectionStatus(QSqlError);

signals:

    void signal_connectionStatus(QSqlError);
    void signal_sendAirportList(QVector<QString>);
    void signal_returnError(QSqlError);
    void signal_paintTable(QSqlQueryModel *);
    void signal_sendStatData(QMap<QString, QMap<int, QVector<QVector<int>>>>);

public slots:

    void recieve_connectToDBase(QMap<connection_info, QString>);
    void recieve_disconnectFromDBase(QMap<connection_info, QString>);
    void recieve_requestAirportList();
    void recieve_buildTable(QString);
    void recieve_gatherData(QString, QString);

private:

    QSqlDatabase * dataBase;
    QSqlQuery * dbQuery;
    QSqlQueryModel * qModel;
    bool modelSetFlag = false;

    QSqlError connectToDBase(QMap<connection_info, QString>);
    void disconnectFromDBase(QString nameDb = "");
    void addDataBase(QString driver, QString nameDB);
    void sendAirportList(QVector<QString>);
    QSqlError simpleDbQuery(QString);
    QString eraser(const QString &);
    void getDataFromDB();
    void makeRequest(QString);
    void gatherData(QString, QString);
    void getStatFromDB(QString);
};

#endif // DATABASE_H
