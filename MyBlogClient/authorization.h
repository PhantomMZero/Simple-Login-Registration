#ifndef AUTHORIZATION_H
#define AUTHORIZATION_H


#include <QMainWindow>
#include <QString>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTextEdit>
#include <QRegularExpression>


QT_BEGIN_NAMESPACE
namespace Ui { class Authorization; }
QT_END_NAMESPACE

class Authorization : public QMainWindow
{
    Q_OBJECT

public:
    Authorization(QWidget *parent = nullptr);
    quint16 flagAuth=1; //1-Авторизация
    quint16 flags; //0-Неудачно, 1-Авторизирован
    QTcpSocket *m_pTcpSocket;
    QPair<QString, QString> Data;
    ~Authorization();

private slots:


    void on_Registration_clicked();

    void slotReadyRead();
    void slotErrorOccured(QAbstractSocket::SocketError);
    void slotSendToServer();
    void slotConnected();
private:

    quint16 m_nNextBlockSize=0;//служит для хранения длины следующего полученного от сокета блока.
    Ui::Authorization *ui;
};
#endif // AUTHORIZATION_H
