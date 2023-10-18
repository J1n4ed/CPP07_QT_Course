#include "../headers/statwindow.h"
#include "ui_statwindow.h"

Statwindow::Statwindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Statwindow)
{
    ui->setupUi(this);

    this->setWindowTitle("Статистика загруженности");
}

Statwindow::~Statwindow()
{
    delete ui;
}
