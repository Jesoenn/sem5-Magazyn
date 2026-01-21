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

    // Aktualizacja liczby produktow w zamowieniu
    // Zwolnienie zajmowanego pojazdu
    WarehouseWorkerFunctionalTest tests(db);
    QTest::qExec(&tests, argc, argv);

    // Test potwierdzenia oddanego zamowienia
    // Usuniecie przedmiotu z dostawy
    // Dodanie przedmiotu do dostawy (do tabeli items tez sie doda)
    // Potwierdzenie zmian w dostawie
    ReceivingWorkerFunctionalTest tests2(db);
    QTest::qExec(&tests2, argc, argv);

    // Zmiana przypisania pojazdu
    // Zatrudnienie pracownika
    // Przypisanie do zamowienia
    // Utworzenie zamowienia z przypisaniem do pracownika
    // Dodanie przedmiotu do zamowienia
    ManagerFunctionalTest tests3(db);
    QTest::qExec(&tests3, argc, argv);


    return 0;
}
