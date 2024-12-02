#include "SmartHomeServer.h"
#include "Arguments.h"
//----------------------------------------------------------------------

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QtSql>
#include <QMessageBox>
//----------------------------------------------------------------------

// -hn=localHost   || --hostName=localHost
// -po=5432         || --port=5432
// -un=postgres    || --username=postgres
// -pwd=19871      || --password=19871
// -dbn=SQLPractic || --databasename=SQLPractic

// -hN=localHost  -p=5432 -uN=postgres -pwd=19871 -dBN=SmartHomeConfig
//----------------------------------------------------------------------

void printHelp()
{
    qDebug() << "-h or --help\n"
             << "Example:\n"
             << "-h\n"

             << "-hN or --hostName\n"
             << "Example:\n"
             << "-hN=localhost\n"

             << "-p or --port\n"
             << "Example:\n"
             << "-p=5432\n"

             << "-uN or --userName\n"
             << "Example:\n"
             << "-uN=postgres\n"

             << "-pwd or --password\n"
             << "Example:\n"
             << "-pwd=19871\n"

             << "-dBN or --dataBaseName\n"
             << "Example:\n"
             << "-dBN=SmartHomeConfig\n";
}
//----------------------------------------------------------------------

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Arguments arguments = Arguments(argc, argv);
    if(arguments.isHelp)
    {
        printHelp();
        return 0;
    }

    QSqlDatabase dbase = QSqlDatabase::addDatabase("QPSQL");
    dbase.setHostName(arguments.hostName);
    dbase.setPort(arguments.port);
    dbase.setUserName(arguments.userName);
    dbase.setPassword(arguments.pwd);
    dbase.setDatabaseName(arguments.dataBaseName);

    if(!dbase.open())
    {
        qWarning() << "[x] Not Open DB" << dbase.lastError().text();
    }

    SmartHomeServer sHS;
    sHS.init(&dbase);

    return a.exec();
}















