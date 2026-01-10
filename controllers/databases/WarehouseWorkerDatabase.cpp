//
// Created by jakub on 2026-01-10.
//

#include "WarehouseWorkerDatabase.h"

std::vector<QString> WarehouseWorkerDatabase::getEmployeeInfo(int employeeId) {
    QSqlQuery query(db);
    query.prepare("SELECT first_name, last_name, job, vehicle_id FROM view_employees_info WHERE employee_id = ?");
    query.addBindValue(employeeId);

    std::vector<QString> employeeInfo;

    if (query.exec() && query.next()) {
        employeeInfo.push_back(QString::number(employeeId));
        employeeInfo.push_back(query.value("first_name").toString());
        employeeInfo.push_back(query.value("last_name").toString());
        employeeInfo.push_back(query.value("job").toString());
        QVariant vehicleVar = query.value("vehicle_id");
        if (vehicleVar.isNull()) {
            employeeInfo.push_back("null");
        } else {
            employeeInfo.push_back(vehicleVar.toString());
        }
    } else {
        for (int i = 0; i < 5; i++) {
            employeeInfo.push_back("Unknown");
        }
    }


    return employeeInfo;
}

WarehouseWorkerDatabase::WarehouseWorkerDatabase(QSqlDatabase db): db(db) {

}

int WarehouseWorkerDatabase::getActiveOrderCount(int employeeId) {
    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM orders WHERE assigned_employee_id = ? AND status = 'realizacja'");
    query.addBindValue(employeeId);
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    } else {
        return -1;
    }
}

int WarehouseWorkerDatabase::getFreeOrder(int employeeId) {
    QSqlQuery query(db);
    // Get one order with "nowe" status to employee
    query.prepare("SELECT order_id FROM orders WHERE assigned_employee_id = ? AND status = 'nowe' LIMIT 1");
    query.addBindValue(employeeId);
    if (query.exec() && query.next()) {
        return query.value("order_id").toInt();
    }
    return -1;
}

int WarehouseWorkerDatabase::assignOrder(int orderId, int employeeId) {
    QSqlQuery updateQuery(db);
    updateQuery.prepare("UPDATE orders SET assigned_employee_id = ?, status = 'realizacja' WHERE order_id = ?");
    updateQuery.addBindValue(employeeId);
    updateQuery.addBindValue(orderId);

    if (!updateQuery.exec()) {
        return -1;
    }
    return 0;
}
