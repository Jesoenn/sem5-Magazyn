//
// Created by jakub on 2026-01-12.
//

#ifndef MAGAZYN_MANAGERFUNCTIONALTEST_H
#define MAGAZYN_MANAGERFUNCTIONALTEST_H

#include <QtTest>
#include <QSqlDatabase>

class ManagerFunctionalTest : public QObject {
Q_OBJECT

public:
    explicit ManagerFunctionalTest(QSqlDatabase& db);

private slots:
    void testHandleSaveVehicle(); // vehicleId, status, employeeId -> zmiana pojazdu
    void testHandleAddEmployee(); // login,  firstName,   lastName,  jobId,   password -> zatrudnienie pracownika
    void testHandleAssignEmployee(); // int orderId, int employeeId -> przypisanie pracownika do zamowienia
    void testHandleCreateOrder(); //  employeeId -> utworzenie zamowienia z przypisaniem do pracownika
    void testHandleAddOrderItem(); // orderId,  itemId, quantity -> dodanie przedmiotu do zamowienia

private:
    QSqlDatabase& db;
};



#endif //MAGAZYN_MANAGERFUNCTIONALTEST_H
