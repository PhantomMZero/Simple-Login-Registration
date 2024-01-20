#include "registration.h"
#include "ui_registration.h"


Registration::Registration(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Registration)
{
    ui->setupUi(this);
    m_pTcpSocket = new QTcpSocket(this);
    m_pTcpSocket->connectToHost("localhost", 2323); //Ввести ip сервера
    connect(m_pTcpSocket, SIGNAL(connected()), SLOT(slotConnected()));
    connect(m_pTcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(m_pTcpSocket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this, SLOT(slotErrorOccured(QAbstractSocket::SocketError)));
    connect(ui->RegistrationButton, SIGNAL(clicked()), SLOT(slotSendToServer()));
    connect(ui->BackButton, SIGNAL(clicked()), SLOT(BackPushButtonClicked()));
}


Registration::~Registration()
{
    delete ui;
}



void Registration::slotReadyRead()
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
        in >> flagsIn;
        if (flagsIn==0) {
            QMessageBox msgb;
            msgb.setText("Данная почта уже зарегистрирована");
            msgb.exec();
            return;
        }

        if (flagsIn==4) {
            QMessageBox msgb;
            msgb.setText("Данный логин уже зарегистрирован");
            msgb.exec();
            return;
        }

        if (flagsIn==2) {
            //Дописать подтверждение по Email(Ввести код подтверждения)
            authWindow.show();
            close();
        }
        m_nNextBlockSize=0;
    }
}


void Registration::slotErrorOccured(QAbstractSocket::SocketError err)
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

void Registration::slotSendToServer()
{


    QString email=ui->EmaillineEdit->text();
    QString login=ui->LoginlineEdit->text();
    QString password=ui->PasswordlineEdit->text();
    QString passwordCheck=ui->PasswordChecklineEdit->text();
    static QRegularExpression regexEmail("^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,6}$");
    static QRegularExpression regexLogin("^[A-Za-z0-9]{2,10}$");
    static QRegularExpression regexPassword("^[A-Za-z0-9]{6,20}$");

    if (!regexEmail.match(email).hasMatch()){
        QMessageBox msgb;
        msgb.setText("Неверный Email");
        msgb.exec();
        return;
    }
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
    if(password!=passwordCheck){
        QMessageBox msgb;
        msgb.setText("Пароли не совпадают");
        msgb.exec();
        return;
    }


    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_4);
   //Добавить на сервере отправку подтверждения на Email.
   //Добавить проверку на наличие Email, логина
   //На будущее нужно добавить шифрование данных
    out << quint16(0) << flagOut << email << login << password;
    out.device()->seek(0);
    out << quint16(arrBlock.size()-sizeof(quint16));
    m_pTcpSocket->write(arrBlock);
}

void Registration::slotConnected()
{
    QMessageBox msgb;
    msgb.setText("connected");
    msgb.exec();
}
void Registration::BackPushButtonClicked()
{
    authWindow.show();
    close();
}

