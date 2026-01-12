#include <QApplication>
#include <QtTest>
#include <QSqlDatabase>
#include <QDebug>

#include "WarehouseWorkerFunctionalTest.h"
#include "ReceivingWorkerFunctionalTest.h"
#include "ManagerFunctionalTest.h"

QSqlDatabase db;

void connectToDatabase() {
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setPort(3306);
    db.setUserName("root");
    db.setPassword("root");
    db.setDatabaseName("magazyn_testy");

    if (!db.open()) {
        qFatal("Blad polaczenia z baza danych");
    }
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    connectToDatabase();

    WarehouseWorkerFunctionalTest tests(db);
    QTest::qExec(&tests, argc, argv);

    ReceivingWorkerFunctionalTest tests2(db);
    QTest::qExec(&tests2, argc, argv);

    ManagerFunctionalTest tests3(db);
    QTest::qExec(&tests3, argc, argv);


    return 0;
}
