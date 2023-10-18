#ifndef STATWINDOW_H
#define STATWINDOW_H

#include <QDialog>

namespace Ui {
class Statwindow;
}

class Statwindow : public QDialog
{
    Q_OBJECT

public:
    explicit Statwindow(QWidget *parent = nullptr);
    ~Statwindow();

private:
    Ui::Statwindow *ui;
};

#endif // STATWINDOW_H
