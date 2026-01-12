//
// Created by jakub on 2026-01-12.
//

#ifndef MAGAZYN_WAREHOUSEWORKERFUNCTIONALTEST_H
#define MAGAZYN_WAREHOUSEWORKERFUNCTIONALTEST_H

#include <QtTest>
#include <QSqlDatabase>

class WarehouseWorkerFunctionalTest : public QObject {
Q_OBJECT

public:
    explicit WarehouseWorkerFunctionalTest(QSqlDatabase& db);

private slots:
    void testHandleOrderUpdate();   // Aktualizacja haribo z 0 na 1 w zamowieniu
    void testHandleFreeVehicle();   // Zwolnienie pojazdu 1 przez pracownika 1

private:
    QSqlDatabase& db;
};

#endif
