#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>
#include <QWidget>
#include <QMovie>

namespace Ui {
class About;
}

class About : public QDialog
{
    Q_OBJECT

public:

    explicit About(QWidget *parent = nullptr);
    ~About();

private slots:

    void on_pb_close_about_clicked();

private:

    Ui::About *ui;
    QMovie mo_about;
};

#endif // ABOUT_H
