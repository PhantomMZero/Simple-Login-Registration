#ifndef BLOGMAINWINDOW_H
#define BLOGMAINWINDOW_H

#include <QWidget>

namespace Ui {
class BlogMainWindow;
}

class BlogMainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit BlogMainWindow(QWidget *parent = nullptr);
    ~BlogMainWindow();

private:
    Ui::BlogMainWindow *ui;
};

#endif // BLOGMAINWINDOW_H
