#include "authorization.h"
#include "./ui_authorization.h"
#include "registration.h"
#include "blogmainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QString>
#include <QPair>

Authorization::Authorization(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Authorization)
{
    ui->setupUi(this);
    ui->LoginlineEdit->setPlaceholderText("Enter Your Login");
    ui->PasswordlineEdit->setPlaceholderText("Enter Your Password");
    m_pTcpSocket = new QTcpSocket(this);
    m_pTcpSocket->connectToHost("localhost", 2323); //Ввести ip сервера
    connect(m_pTcpSocket, SIGNAL(connected()), SLOT(slotConnected()));
    connect(m_pTcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(m_pTcpSocket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this, SLOT(slotErrorOccured(QAbstractSocket::SocketError)));
    connect(ui->LogIn, SIGNAL(clicked()), SLOT (slotSendToServer()));


}

Authorization::~Authorization()
{
    delete ui;
}

//базу данных перевести на сервер (advanced сделать хеш хранение)

void Authorization::slotReadyRead()
{
    QDataStream in(m_pTcpSocket);
    in.setVersion(QDataStream::Qt_6_4);
    for( ; ; ){
        //Получение данных либо целым блоком, либо только часть
        if(!m_nNextBlockSize){
            if(m_pTcpSocket->bytesAvailable()<sizeof(quint16)){
                break;
            }
            in>>m_nNextBlockSize;
        }
        if(m_pTcpSocket->bytesAvailable()<m_nNextBlockSize){
            break;
        }


        //получение данных из потока
        in >> flags;
        if (flags==0){
            QMessageBox msgb;
            msgb.setText("Пользователя с введенными данными не существует");
            msgb.exec();
            return;
        }
        if (flags==1){
            BlogMainWindow* BlogMainWindowW=new BlogMainWindow();
            BlogMainWindowW->show();
            hide();
        }
         m_nNextBlockSize=0;
    }
}

void Authorization::slotErrorOccured(QAbstractSocket::SocketError err)
{
    QString strError =
        "Error: " + (err == QAbstractSocket::HostNotFoundError ?
                     "The host was not found. Restart a programm" :
                     err == QAbstractSocket::RemoteHostClosedError ?
                     "The remote host is closed. Restart a programm" :
                     err == QAbstractSocket::ConnectionRefusedError ?
                     "The connection was refused. Restart a programm" :
                     QString(m_pTcpSocket->errorString())
                    );
    QMessageBox msgb;
    msgb.setText(strError);
    msgb.exec();
}

void Authorization::slotSendToServer()
{


    QString login=ui->LoginlineEdit->text();
    QString password=ui->PasswordlineEdit->text();
    static QRegularExpression regexLogin("^[A-Za-z0-9]{2,10}$");
    static QRegularExpression regexPassword("^[A-Za-z0-9]{6,20}$");
    if (!regexLogin.match(login).hasMatch()){
            QMessageBox msgb;
            msgb.setText("Неверный логин");
            msgb.exec();
            return;
    }
    if (!regexPassword.match(password).hasMatch()){
            QMessageBox msgb;
            msgb.setText("Неверный пароль");
            msgb.exec();
            return;
    }

    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_4);
    Data=qMakePair(login, password);
    //На будущее нужно добавить шифрование данных
    out << quint16(0) << flagAuth<< Data;
    out.device()->seek(0);
    out << quint16(arrBlock.size()-sizeof(quint16));
    m_pTcpSocket->write(arrBlock);
}

void Authorization::slotConnected()
{
    qDebug()<<"Connected";
}



void Authorization::on_Registration_clicked()
{
    Registration* RegistrationWindow=new Registration();
    RegistrationWindow->show();

    close();
}

