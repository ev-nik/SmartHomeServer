#include "SmartHomeServer.h"
//-------------------------------------------------------------------------

#include <QDataStream>
#include <QDebug>
#include <ctime>
#include <QUuid>
#include <QDateTime>
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
    if(!listen(QHostAddress::Any, 3333))
    {
        qWarning() << "[x] Error connect Host";
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

    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_15);

    if(in.status() == QDataStream::Ok)
    {
        for(;;)
        {
            if(nextBlockSize == 0)
            {
                if(socket->bytesAvailable() < 2)
                {
                    break;
                }

                in >> nextBlockSize;
            }

            if(socket->bytesAvailable() < nextBlockSize)
            {
                qWarning() << "Data not full";
                break;
            }

            quint8 typeObject;
            in >> typeObject;

            switch(typeObject)
            {
                case House:
                {
                    PropHouse* propHouse = new PropHouse();
                    in >> propHouse->name >> propHouse->address >> propHouse->id;
                    nextBlockSize = 0;

                    vectorHouse.append(propHouse);
                    break;
                }
                case Room:
                {
                    PropRoom* propRoom = new PropRoom();
                    in >> propRoom->name >> propRoom->square >> propRoom->countWindow >> propRoom->id >> propRoom->idHouse;

                    vectorRoom.append(propRoom);

                    nextBlockSize = 0;
                    break;
                }
                case Sensor:
                {
                    PropSensor* propSensor = new PropSensor();
                    in >> propSensor->name >> propSensor->typeSensor >> propSensor->id >> propSensor->idRoom;
                    nextBlockSize = 0;

                    vectorSensor.append(propSensor);
                    break;
                }
                default : break;
            }
        }

        m_Timer->start(1000);
    }
    else
    {
        qWarning() << "QDataStream ERROR";
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
    // псевдо случайные значения
    srand(time(NULL));

    foreach(PropSensor* propSensor, vectorSensor)
    {
        switch(propSensor->typeSensor)
        {
            case TemperatureDetector:
            {
                int randTemp = rand() % 60 + (-60);

                qint64 dateTime = QDateTime::currentDateTime().toSecsSinceEpoch();
                qDebug() << dateTime;

                if(!insertValuesTable(propSensor, dateTime, randTemp))
                {
                    return;
                }
                break;
            }
            case HumiditiDetector:
            {
                int randHum = rand() % 100 + 0;

                qint64 dateTime = QDateTime::currentDateTime().toSecsSinceEpoch();
                qDebug() << dateTime;

                if(!insertValuesTable(propSensor, dateTime, randHum))
                {
                    return;
                }
                break;
            }
            case SmokeDetector:
            {
                int randSmoke = rand() % 100 + 0;

                qint64 dateTime = QDateTime::currentDateTime().toSecsSinceEpoch();
                qDebug() << dateTime;

                if(!insertValuesTable(propSensor, dateTime, randSmoke))
                {
                    return;
                }
                break;
            }
            default:break;
        }
    }
}
//-------------------------------------------------------------------------

bool SmartHomeServer::insertValuesTable(PropSensor* propsensor, qint64& dateTime, int& value)
{
    QSqlQuery query = QSqlQuery(*dbase);

    QString insertValuesSQL = QString("INSERT INTO Values (id_, id_sensor, date_time, value) VALUES (:id_, :id_sensor, :date_time, :value);");
    query.prepare(insertValuesSQL);
    query.bindValue(":id_"      , QUuid::createUuid().toString());
    query.bindValue(":id_sensor", propsensor->id);
    query.bindValue(":date_time", dateTime);
    query.bindValue(":value"    , value);

    if(!query.exec())
    {
        qWarning() << "[x] Error insert value in DB" << query.lastError().text();
        return false;
    }

    return true;
}
//------------------------------------------------------------------------------------

void SmartHomeServer::init(QSqlDatabase* dbase)
{
    this->dbase = dbase;
}














