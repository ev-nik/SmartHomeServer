#ifndef SMARTHOMESERVER_H
#define SMARTHOMESERVER_H
//-------------------------------------------------------------------------

#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
//-------------------------------------------------------------------------

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

public slots:
    void incomingConnection(qintptr socketDescriptor);
    void slotReadyRead();
};
#endif // SMARTHOMESERVER_H
