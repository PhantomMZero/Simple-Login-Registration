#pragma once

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTextEdit>
#include <QSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

//for secure need to use QSslSocket

namespace Ui {
class Server;
}

class Server : public QWidget
{
    Q_OBJECT

public:
    explicit Server(int nPort, QWidget *parent=nullptr);
    QSqlDatabase db;
    quint16 flagsOut;// 0-неудачно, 1-удачно, 2-удачная регистрация, 4 неверный логин

    ~Server();

public slots:
       virtual void slotNewConnection();
               void slotReadClient();


private:
    QTcpServer *m_ptcpServer;//Ввести ip сервера
    QTextEdit *m_ptext;
    quint16 m_nNextBlockSize=0;//служит для хранения длины следующего полученного от сокета блока.
    void sendToClient (QTcpSocket* pSocket, quint16 sendingFlag);


    Ui::Server *ui;
};


