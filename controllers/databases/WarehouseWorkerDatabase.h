//
// Created by jakub on 2026-01-10.
//

#ifndef MAGAZYN_WAREHOUSEWORKERDATABASE_H
#define MAGAZYN_WAREHOUSEWORKERDATABASE_H
#include <iostream>
#include <vector>
#include <QSqlDatabase>
#include <QSqlQuery>

class WarehouseWorkerDatabase {
private:
    QSqlDatabase db;
public:
    WarehouseWorkerDatabase(QSqlDatabase db);
    std::vector<QString> getEmployeeInfo(int employeeId);
    int getActiveOrderCount(int employeeId);
    int getFreeOrder(int employeeId);
    int assignOrder(int orderId, int employeeId);

};


#endif //MAGAZYN_WAREHOUSEWORKERDATABASE_H
