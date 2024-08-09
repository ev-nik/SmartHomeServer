#include <QCoreApplication>

#include "SmartHomeServer.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    SmartHomeServer sHS;

    return a.exec();
}
