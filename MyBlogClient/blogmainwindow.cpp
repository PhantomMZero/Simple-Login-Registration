#include "blogmainwindow.h"
#include "ui_blogmainwindow.h"

BlogMainWindow::BlogMainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BlogMainWindow)
{
    ui->setupUi(this);
}

BlogMainWindow::~BlogMainWindow()
{
    delete ui;
}
