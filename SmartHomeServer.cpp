#include "SmartHomeServer.h"
//-------------------------------------------------------------------------

#include <QDataStream>
#include <QDebug>
//-------------------------------------------------------------------------

SmartHomeServer::SmartHomeServer()
{
    if(listen(QHostAddress::Any, 3333))
    {
        qDebug() << "start";
    }
    else
    {
        qDebug() << "not start";
    }

    nextBlockSize = 0;
}
//-------------------------------------------------------------------------

void SmartHomeServer::incomingConnection(qintptr socketDescriptor)
{
    socket = new QTcpSocket;
    socket->setSocketDescriptor(socketDescriptor);

    connect(socket, &QTcpSocket::readyRead, this, &SmartHomeServer::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);

    sockets.push_back(socket);
    qDebug() << "client conected" << socketDescriptor;
}
//-------------------------------------------------------------------------

void SmartHomeServer::slotReadyRead()
{
    socket = (QTcpSocket*)sender();

    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_15);

    if(in.status() == QDataStream::Ok)
    {
        for(;;)
        {
            if(nextBlockSize == 0)
            {
                qDebug() << "nextBlockSize = 0";

                if(socket->bytesAvailable() < 2)
                {
                    qDebug() << "data < 2";
                    break;
                }

                in >> nextBlockSize;
                qDebug() << "nextBlockSize = " << nextBlockSize;

                if(socket->bytesAvailable() < nextBlockSize)
                {
                    qDebug() << "data not full";
                    break;
                }

//                QVector<PropHouse*> vectorHouse;
                PropHouse  propHouse;
                in >> propHouse.name >> propHouse.address >> propHouse.id;
                nextBlockSize = 0;
                qDebug() << "nsme " << propHouse.name;
                qDebug() << "address" << propHouse.address;
                qDebug() << "id" << propHouse.id;
                break;
            }
        }
    }
    else
    {
        qDebug() << "QDataStream ERROR";
    }
}
//-------------------------------------------------------------------------

void SmartHomeServer::sendToClient(QString str)
{
    data.clear();

    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    out << quint16(0) << str;
    out.device()->seek(0);
    out << quint16(data.size() - sizeof(quint16));

    for(int i = 0; i < sockets.size(); i++)
    {
        sockets[i]->write(data);
    }
}
//-------------------------------------------------------------------------





















