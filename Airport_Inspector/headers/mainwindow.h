#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QMovie>
#include <QMessageBox>
#include <QMap>
#include <QVector>
#include <QSqlError>
#include <QtConcurrent>

#include "../headers/about.h"
#include "../headers/database.h"
#include "../headers/statwindow.h"
#include "enumerations.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void connectToDB(QMap<connection_info, QString>);
    void disconnectFromDB(QMap<connection_info, QString>);
    void requestAirportList();
    void buildTable(QString);

private slots:

    void on_action_about_triggered();
    void on_action_close_triggered();
    void on_pb_close_clicked();
    void on_pb_search_clicked();
    void on_pb_reconnect_clicked();
    void on_pb_staticstics_clicked();
    void on_action_load_triggered();
    void on_action_getports_triggered();
    void on_cb_airport_currentIndexChanged(int index);
    void on_tableViewSelection(const QItemSelection &, const QItemSelection &);
    void on_pb_clear_clicked();

signals:

    void signal_connectToDBase(QMap<connection_info, QString>);
    void signal_disconnectFromDBase(QMap<connection_info, QString>);
    void signal_requestAirportList();
    void signal_buildTable(QString);

public slots:

    void recieve_connectionStatus(QSqlError);
    void recieve_sendAirportList(QVector<QString>);
    void recieve_returnError(QSqlError);
    void recieve_paintTable(QSqlQueryModel *);

private:

    Ui::MainWindow *ui;
    QWidget * widget;
    QMovie * mov_halt;
    QMovie * mov_go;
    QPixmap * pix;
    QString portSelection = "";

    About * about;
    QMessageBox * msg;
    Statwindow * stats;
    Database * database;

    QMap<connection_info, QString> connection_data;
};
#endif // MAINWINDOW_H
