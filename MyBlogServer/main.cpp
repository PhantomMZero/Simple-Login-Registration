#include <QtWidgets>
#include "server.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Server serv(2323);
    serv.show();

    return a.exec();
}
