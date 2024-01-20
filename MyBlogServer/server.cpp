#include "server.h"
#include "ui_server.h"
#include <QMessageBox>
#include <QDir>
#include <QPair>


Server::Server(int nPort, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Server)
{
    ui->setupUi(this);
    //создание и подключение базы данных
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setHostName("localhost");
    db.setDatabaseName(QDir::currentPath()+"/BlogUsers.sqlite");
    db.setUserName("root");
    db.setPassword("");
    m_ptext=new QTextEdit;
    //Layout setup
    ui->verticalLayout->addWidget(m_ptext);
    m_ptcpServer=new QTcpServer(this);
    if(!m_ptcpServer->listen(QHostAddress::Any, nPort)){
        QMessageBox::critical(0,"Server Error", "Unable to start the server:" + m_ptcpServer->errorString());
        m_ptcpServer->close();
        return;
    }
    m_ptext->append("server started");
    connect(m_ptcpServer, SIGNAL (newConnection()),
            this, SLOT(slotNewConnection()));
    m_ptext->append("sygnal estable");
}

void Server::slotNewConnection()
{
    m_ptext->append("new connection");
    QTcpSocket* pClientSocket = m_ptcpServer->nextPendingConnection();
    connect(pClientSocket, SIGNAL(disconnected()),
            pClientSocket, SLOT(deleteLater())
            );
    connect(pClientSocket, SIGNAL(readyRead()),
            this,          SLOT(slotReadClient())
            );
}


void Server::slotReadClient()
{
    QTcpSocket* pClientSocket = (QTcpSocket*)sender();
    QDataStream in(pClientSocket);
    in.setVersion(QDataStream::Qt_6_4);
    QPair<QString, QString> data;
    quint16 flagsIn;//1-Аутентификация, 2-Регистрация.
    for( ; ; ){
        //Получение данных либо целым блоком, либо только часть
        if(!m_nNextBlockSize){
            if(pClientSocket->bytesAvailable()<sizeof(quint16)){
                break;
            }
            in>>m_nNextBlockSize;
        }
        if(pClientSocket->bytesAvailable()<m_nNextBlockSize){
            break;
        }


        in >> flagsIn;

        //Аутентификация
        if (flagsIn==1){
            in >> data;
            //Прописать проверку из дб
            if(!db.open()){
                qDebug() << "db dont open";
            }
            else{
                QString a=data.first;
                QString b=data.second;
                QString strMessage =
                    "Client has sended - " + data.first + data.second;
                data.first.clear();
                data.second.clear();
                m_ptext->append(strMessage);
                QSqlQuery qr;
                qr.prepare("SELECT email, login, password FROM users WHERE login=:login AND password=:password");
                qr.bindValue(":login", a);
                qr.bindValue(":password", b);
                if (!qr.exec()){
                    qDebug() << qr.lastError().text();
                    qDebug() << a<<'\t'<<b;
                }
                else{
                    int count=0;
                    while (qr.next()){
                        count++;
                    }
                    if (count==1){
                        flagsOut=1;
                        sendToClient(pClientSocket, flagsOut);
                        qDebug()<<"nice";
                    }
                    if (count <1){
                        flagsOut=0;
                        sendToClient(pClientSocket, flagsOut);
                        qDebug()<<"NotFound";
                    }

                }

            }
        }


        //Регистрация
        if (flagsIn==2){
            QString email, login, password;
            in >> email >> login >> password;
            if(!db.open()){
                qDebug() << "db dont open";
            }
            else{
                QSqlQuery qr;
                if (!qr.exec("CREATE TABLE IF NOT EXISTS users("
                             "email VARCHAR(255),  "
                             "login VARCHAR(255), "
                             "password VARCHAR(255))"))
                {
                    qDebug()<<"Не удалось создать таблицу";
                }

                qr.prepare("SELECT email, login, password FROM users WHERE email=:email");
                qr.bindValue(":email", email);

                if (!qr.exec()){
                    qDebug() << qr.lastError().text();
                }
                else{
                    if (!qr.next()<1){
                        flagsOut=0;//Неудача, такой email уже существует
                        sendToClient(pClientSocket, flagsOut);
                    }
                    else{
                        qr.clear();
                        qr.prepare("SELECT email, login, password FROM users WHERE login=:login");
                        qr.bindValue(":login", login);
                        if (!qr.exec()){
                            qDebug() << "loginqrnotworking";
                        }
                        else{
                            int count=0;
                            while (qr.next()){
                                count++;
                            }
                            if (!(count <1)){
                                flagsOut=4;//Неудача, такой логин уже существует
                                sendToClient(pClientSocket, flagsOut);

                            }
                            else{
                                flagsOut=2;//успешная регистрация
                                sendToClient(pClientSocket, flagsOut);
                                //без реализации подтверждения на email.
                                qr.prepare("INSERT INTO users (email, login, password) "
                                           "VALUES (:email, :login, :password)");
                                qr.bindValue(":email", email);
                                qr.bindValue(":login", login);
                                qr.bindValue(":password", password);
                                if (!qr.exec()){
                                    qDebug()<<"Not added in db";
                                }
                            }



                        }
                    }
                }
            }

        }
         m_nNextBlockSize=0;
    }
}



void Server::sendToClient(QTcpSocket* pSocket, quint16 sendingFlag)
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_4);
    out << quint16(0) << sendingFlag;
    out.device()->seek(0);
    out << quint16(arrBlock.size()-sizeof(quint16));
    pSocket->write(arrBlock);
}


Server::~Server()
{
    db.close();
    delete ui;
}
