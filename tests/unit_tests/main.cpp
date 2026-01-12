#include <QApplication>
#include <QTest>
#include <iostream>
#include "ManagerViewTests.h"
#include "ReceivingWorkerTests.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QTest::qExec(new ManagerViewTests, argc, argv);   // test dodania pracownika / test modyfikacji przypisania pojazdu / test modyfikacji wlasnosci zamowienia
    QTest::qExec(new ReceivingWorkerTests, argc, argv); // test modyfikacji zamowienia

    return 0;
}
