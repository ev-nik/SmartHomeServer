#include "SmartHomeServer.h"
//-------------------------------------------------------------------------

#include <QDataStream>
#include <QDebug>
#include <ctime>
//-------------------------------------------------------------------------

enum HouseObject
{
    House  = 1,
    Room   = 2,
    Sensor = 3
};

enum TypeSensor
{
    TemperatureDetector = 0,
    HumiditiDetector    = 1,
    SmokeDetector       = 2

};


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

    m_Timer = new QTimer(this);

    connect(m_Timer, SIGNAL(timeout()), this, SLOT(genValue()));
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
    foreach(PropHouse* propHouse, vectorHouse)
    {
        delete propHouse;
    }
    vectorHouse.clear();

    foreach(PropRoom* propRoom, vectorRoom)
    {
        delete propRoom;
    }
    vectorRoom.clear();

    foreach(PropSensor* propSensor, vectorSensor)
    {
        delete propSensor;
    }
    vectorSensor.clear();

    socket = (QTcpSocket*)sender();
    qDebug() << "----------";
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
            }

            if(socket->bytesAvailable() < nextBlockSize)
            {
                qDebug() << "data not full";
                break;
            }

            quint8 tmp;
            in >> tmp;

            switch(tmp)
            {
                case House:
                {
                    PropHouse* propHouse = new PropHouse();
                    qDebug() << "1socket->bytesAvailable() = " << socket->bytesAvailable();
                    in >> propHouse->name >> propHouse->address >> propHouse->id;
                    nextBlockSize = 0;

                    vectorHouse.append(propHouse);

                    qDebug() << "name " << propHouse->name;
                    qDebug() << "address" << propHouse->address;
                    qDebug() << "id" << propHouse->id;


                    break;
                }
                case Room:
                {
                    PropRoom* propRoom = new PropRoom();
                    qDebug() << "1socket->bytesAvailable() = " << socket->bytesAvailable();
                    in >> propRoom->name >> propRoom->square >> propRoom->countWindow >> propRoom->id >> propRoom->idHouse;

                    vectorRoom.append(propRoom);

                    nextBlockSize = 0;
                    qDebug() << "name " << propRoom->name;
                    qDebug() << "square" << propRoom->square;
                    qDebug() << "count window" << propRoom->countWindow;
                    qDebug() << "id" << propRoom->id;
                    qDebug() << "idHouse" << propRoom->idHouse;
                    break;
                }
                case Sensor:
                {
                    PropSensor* propSensor = new PropSensor();
                    qDebug() << "1socket->bytesAvailable() = " << socket->bytesAvailable();
                    in >> propSensor->name >> propSensor->typeSensor >> propSensor->id >> propSensor->idRoom;
                    nextBlockSize = 0;

                    vectorSensor.append(propSensor);

                    qDebug() << "name " << propSensor->name;
                    qDebug() << "type sensor" << propSensor->typeSensor;
                    qDebug() << "id" << propSensor->id;
                    qDebug() << "idRoom" << propSensor->idRoom;


                    break;
                }
                default : break;
            }
        }

        m_Timer->start(1000);

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

void SmartHomeServer::genValue()
{
    srand(time(NULL));
    foreach(PropSensor* propSensor, vectorSensor)
    {
        switch(propSensor->typeSensor)
        {
            case TemperatureDetector:
            {
//                int randTemp = rand() % (60 - (-50 + 1)) + (-50);
                int randTemp = rand() % 60 + (-60);
                qDebug() << "Temperature Detector " << propSensor->name << propSensor->typeSensor << randTemp;

                break;
            }
            case HumiditiDetector:
            {
//                int randHum = rand() % (100 - 0 + 1) + 0;
                int randHum = rand() % 100 + 0;
                qDebug() << "Humiditi Detector " << propSensor->name << propSensor->typeSensor << randHum;

                break;
            }
            case SmokeDetector:
            {
//                int randSmoke = rand() % (100 - 0 + 1) + 0;
                int randSmoke = rand() % 100 + 0;
                qDebug() << "Smoke Detector " << propSensor->name << propSensor->typeSensor << randSmoke;

                break;
            }
            default:break;
        }
    }
}


















