//
// Created by jakub on 2026-01-10.
//

#include "WarehouseWorkerDatabase.h"
#include <QDateTime>

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

std::vector<QString> WarehouseWorkerDatabase::getCurrentOrderInfo(int employeeId) {
    QSqlQuery query(db);
    query.prepare("SELECT order_id, creation_date, created_by_name FROM view_orders WHERE assigned_employee_id = ? AND status = \"realizacja\"");
    query.addBindValue(employeeId);

    if (!query.exec()) {
        throw std::runtime_error("Database error");
    }

    if (!query.next()) {
        throw std::invalid_argument("No active order");
    }

    std::vector<QString> orderInfo;
    orderInfo.push_back(query.value("order_id").toString());
    orderInfo.push_back(query.value("creation_date").toDateTime().toString("dd.MM.yyyy HH:mm"));
    orderInfo.push_back(query.value("created_by_name").toString());

    return orderInfo;
}

std::vector<std::vector<QString>> WarehouseWorkerDatabase::getOrderItems(int orderId) {
    QSqlQuery query(db);
    query.prepare("SELECT * FROM view_order_items WHERE order_id = ?");
    query.addBindValue(orderId);

    if (!query.exec()) {
        throw std::runtime_error("Error while getting order items");
    }

    std::vector<std::vector<QString>> items;

    while (query.next()) {
        std::vector<QString> item;
        item.push_back(query.value("item_id").toString());
        item.push_back(query.value("item_name").toString());
        item.push_back(query.value("quantity").toString());
        item.push_back(query.value("picked_quantity").toString());
        item.push_back(query.value("available_quantity").toString());
        items.push_back(item);
    }

    return items;
}

void WarehouseWorkerDatabase::orderUpdate(int orderId, int itemId, int quantity) {
    QSqlQuery query(db);

    query.prepare("UPDATE order_items SET picked_quantity = :picked WHERE item_id = :item_id and order_id = :order_id");
    query.bindValue(":picked", quantity);
    query.bindValue(":item_id", itemId);
    query.bindValue(":order_id", orderId);

    if (!query.exec()) {
        QString message = "Błąd dla item_id: " + QString::number(itemId);
        throw std::runtime_error(message.toStdString());
    }
}

void WarehouseWorkerDatabase::submitOrder(int employeeId) {
    QSqlQuery query(db);
    query.prepare("UPDATE orders SET status = 'gotowe' WHERE assigned_employee_id = ? AND status = 'realizacja'");
    query.addBindValue(employeeId);
    if (!query.exec()) {
        throw std::runtime_error("Błąd przy zatwierdzaniu zamówienia");
    }
}

void WarehouseWorkerDatabase::assignVehicle(int employeeId) {
    QSqlQuery query(db);
    query.prepare("UPDATE vehicles SET employee_id = ?, status = 'zajety' WHERE status = 'wolny' and employee_id is NULL LIMIT 1");
    query.addBindValue(employeeId);

    if (!query.exec()) {
        throw std::runtime_error("Błąd przy przypisywaniu pojazdu");
    }
}

void WarehouseWorkerDatabase::freeVehicle(int employeeId) {
    QSqlQuery query(db);
    query.prepare("UPDATE vehicles SET status = 'wolny', employee_id = NULL WHERE employee_id = ?");
    query.addBindValue(employeeId);
    if (!query.exec()) {
        throw std::runtime_error("Błąd przy zwalnianiu pojazdu");
    }
}


