#ifndef SMARTHOMESERVER_H
#define SMARTHOMESERVER_H
//-------------------------------------------------------------------------

#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <QUuid>
//-------------------------------------------------------------------------

struct PropHouse
{
    QString name;
    QString address;
    QString id;

    PropHouse()
    {
        name = "Дом";
    }
};
//------------------------------------------------------------------------------------

struct PropRoom
{
    QString name;
    double square;
    int countWindow;
    QString id;

    PropRoom()
    {
        square = 0;
        name = "Комната";
    }
};
//------------------------------------------------------------------------------------

struct PropSensor
{
    QString name;
    int typeSensor;
    QString id;

    PropSensor()
    {
        name = "Датчик";
    }
};
//------------------------------------------------------------------------------------

class SmartHomeServer : public QTcpServer
{
    Q_OBJECT

public:
    SmartHomeServer();
    QTcpSocket* socket;

private:
    quint16 nextBlockSize;
    QByteArray data;
    QVector <QTcpSocket*> sockets;

    void sendToClient(QString str);

    QVector<PropHouse*> vectorHouse;
    QVector<PropRoom*>  vectorRoom;
    QVector<PropSensor*>vectorSensor;

public slots:
    void incomingConnection(qintptr socketDescriptor);
    void slotReadyRead();
};
#endif // SMARTHOMESERVER_H
