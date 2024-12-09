#ifndef SMARTHOMESERVER_H
#define SMARTHOMESERVER_H
//-------------------------------------------------------------------------

#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <QUuid>
#include <QTimer>
#include <QTime>
#include <QSqlDatabase>
#include <QtSql>
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
    QString idHouse;

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
    QString idRoom;

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

QVector <QTcpSocket*> sockets;

private:
    quint16 nextBlockSize;
    QByteArray data;

    void sendToDisplay(PropSensor* propSensor, int dateTime, int val);

    QVector<PropHouse*> vectorHouse;
    QVector<PropRoom*>  vectorRoom;
    QVector<PropSensor*>vectorSensor;

    QTimer* m_Timer;

public slots:
    void incomingConnection(qintptr socketDescriptor);
    void slotReadyRead();
    void stateChangeSocket(QAbstractSocket::SocketState socketState);
    void genValue();

private:
    bool insertValuesTable(PropSensor* propsensor, qint64& dateTime, int& value);

    void readValuesFromDB();
    void send();
    void deleteSocket();

    QSqlDatabase* dbase;

public:
    void init(QSqlDatabase* dbase);
};
#endif // SMARTHOMESERVER_H
