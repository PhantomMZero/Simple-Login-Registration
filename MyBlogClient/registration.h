#ifndef REGISTRATION_H
#define REGISTRATION_H

#include <QWidget>
#include "authorization.h"
#include <QMessageBox>

namespace Ui {
class Registration;
}

class Registration : public QWidget
{
    Q_OBJECT

public:
    explicit Registration(QWidget *parent = nullptr);
    quint16 flagOut=2;//2-Регистрация
    quint16 flagsIn;
    QTcpSocket *m_pTcpSocket;
    ~Registration();

private slots:
    void slotReadyRead();
    void slotErrorOccured(QAbstractSocket::SocketError);
    void slotSendToServer();
    void slotConnected();
    void BackPushButtonClicked();


private:
    quint16 m_nNextBlockSize=0;//служит для хранения длины следующего полученного от сокета блока.
    Ui::Registration *ui;
    Authorization authWindow;

};

#endif // REGISTRATION_H
