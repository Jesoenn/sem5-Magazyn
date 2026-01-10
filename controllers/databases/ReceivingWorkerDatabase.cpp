//
// Created by jakub on 2026-01-10.
//

#include "ReceivingWorkerDatabase.h"

ReceivingWorkerDatabase::ReceivingWorkerDatabase(QSqlDatabase db): db(db) {

}

std::vector<QString> ReceivingWorkerDatabase::getEmployeeInfo(int employeeId) {
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

void ReceivingWorkerDatabase::newDelivery(int employeeId) {
    QSqlQuery query(db);
    query.prepare("INSERT INTO deliveries (employee_id) VALUES (?)");
    query.addBindValue(employeeId);
    if (!query.exec()) {
        throw std::runtime_error("Błąd przy tworzeniu nowej dostawy");
    }
}

std::vector<std::vector<QString>> ReceivingWorkerDatabase::getDeliveries(int employeeId) {
    QSqlQuery query(db);
    query.prepare("SELECT delivery_id, delivery_date FROM deliveries WHERE employee_id = ? ORDER BY delivery_id DESC");
    query.addBindValue(employeeId);
    if (!query.exec()) {
        throw std::runtime_error("Błąd przy pobieraniu dostaw");
    }

    std::vector<std::vector<QString>> deliveries;
    while (query.next()) {
        std::vector<QString> delivery;
        delivery.push_back(query.value("delivery_id").toString());

        QDateTime deliveryDate = query.value("delivery_date").toDateTime();
        delivery.push_back(deliveryDate.toString("dd.MM.yyyy HH:mm"));

        deliveries.push_back(delivery);
    }

    return deliveries;
}

void ReceivingWorkerDatabase::assignVehicle(int employeeId) {
    QSqlQuery query(db);
    query.prepare("UPDATE vehicles SET employee_id = ?, status = 'zajety' WHERE status = 'wolny' and employee_id is NULL LIMIT 1");
    query.addBindValue(employeeId);

    if (!query.exec()) {
        throw std::runtime_error("Błąd przy przypisywaniu pojazdu");
    }
}

void ReceivingWorkerDatabase::freeVehicle(int employeeId) {
    QSqlQuery query(db);
    query.prepare("UPDATE vehicles SET status = 'wolny', employee_id = NULL WHERE employee_id = ?");
    query.addBindValue(employeeId);
    if (!query.exec()) {
        throw std::runtime_error("Błąd przy zwalnianiu pojazdu");
    }
}

std::vector<std::vector<QString>> ReceivingWorkerDatabase::verifyOrders() {
    QSqlQuery query(db);
    query.prepare("SELECT order_id, creation_date, assigned_employee_name FROM view_orders WHERE status = 'gotowe'");

    if (!query.exec()) {
        throw std::runtime_error("Błąd przy pobieraniu listy zamówień");
    }

    std::vector<std::vector<QString>> orders;

    while (query.next()) {
        std::vector<QString> order;
        order.push_back(query.value("order_id").toString());
        order.push_back(query.value("assigned_employee_name").toString());

        QDateTime creationDate = query.value("creation_date").toDateTime();
        order.push_back(creationDate.toString("dd.MM.yyyy HH:mm"));

        orders.push_back(order);
    }

    return orders;
}

void ReceivingWorkerDatabase::confirmOrder(int orderId) {
    QSqlQuery query(db);
    query.prepare("UPDATE orders SET status = 'wyslane' WHERE order_id = ?");
    query.addBindValue(orderId);
    if (!query.exec()) {
        throw std::runtime_error("Błąd przy potwierdzaniu zamówienia");
    }
}

std::vector<QString> ReceivingWorkerDatabase::getOrderInfo(int orderId) {
    QSqlQuery query(db);
    query.prepare("SELECT creation_date, created_by_name FROM view_orders WHERE order_id = ? AND status = \"gotowe\"");
    query.addBindValue(orderId);
    std::vector<QString> orderInfo;

    if (query.exec() && query.next()) {
        QDateTime creationDate = query.value("creation_date").toDateTime();
        orderInfo.push_back(creationDate.toString("dd.MM.yyyy HH:mm"));
        orderInfo.push_back(query.value("created_by_name").toString());
    } else {
        throw std::runtime_error("Błąd przy pobieraniu informacji o zamówieniu");
    }

    return orderInfo;
}

std::vector<std::vector<QString>> ReceivingWorkerDatabase::getOrderItems(int orderId) {
    std::vector<std::vector<QString>> orderItems;

    QSqlQuery query(db);
    query.prepare("SELECT * FROM view_order_items WHERE order_id = ?");
    query.addBindValue(orderId);
    if (!query.exec()) {
        throw std::runtime_error("Błąd przy pobieraniu pozycji zamówienia");
    }

    while (query.next()) {
        std::vector<QString> item;
        item.push_back(query.value("item_id").toString());
        item.push_back(query.value("item_name").toString());
        item.push_back(query.value("quantity").toString());
        item.push_back(query.value("picked_quantity").toString());
        item.push_back(query.value("available_quantity").toString());

        orderItems.push_back(item);
    }

    return orderItems;
}

void ReceivingWorkerDatabase::updateOrder(int orderId, int itemId, int quantity) {
    QSqlQuery query(db);
    query.prepare("UPDATE order_items SET picked_quantity = :picked WHERE item_id = :item_id and order_id = :order_id");
    query.bindValue(":picked", quantity);
    query.bindValue(":item_id", itemId);
    query.bindValue(":order_id", orderId);

    if (!query.exec()) {
        throw std::runtime_error("Błąd dla item_id: " + QString::number(itemId).toStdString());
    }
}

QString ReceivingWorkerDatabase::getDeliveryDate(int deliveryId) {
    QSqlQuery query(db);
    query.prepare("SELECT delivery_date FROM deliveries WHERE delivery_id = ?");
    query.addBindValue(deliveryId);

    if (!query.exec() || !query.next()) {
        throw std::runtime_error("Błąd przy pobieraniu daty dostawy");
    }

    QDateTime deliveryDate = query.value("delivery_date").toDateTime();
    return deliveryDate.toString("dd.MM.yyyy HH:mm");
}

std::vector<std::vector<QString>> ReceivingWorkerDatabase::getDeliveryItems(int deliveryId) {
    std::vector<std::vector<QString>> deliveryItems;

    QSqlQuery query(db);
    query.prepare("SELECT item_id, item_name, quantity FROM view_delivery_items WHERE delivery_id = ?");
    query.addBindValue(deliveryId);
    if (!query.exec()) {
        throw std::runtime_error("Błąd przy pobieraniu przedmiotów dostawy");
    }

    while (query.next()) {
        std::vector<QString> item;
        item.push_back(query.value("item_id").toString());
        item.push_back(query.value("item_name").toString());
        item.push_back(query.value("quantity").toString());

        deliveryItems.push_back(item);
    }

    return deliveryItems;
}

std::vector<std::vector<QString>> ReceivingWorkerDatabase::getWarehouseItems() {
    std::vector<std::vector<QString>> warehouseItems;

    QSqlQuery query(db);
    query.prepare("SELECT item_id, name FROM items ORDER BY name ASC");
    if (!query.exec()) {
        throw std::runtime_error("Błąd przy pobieraniu listy wszystkich przedmiotów");
    }

    while (query.next()) {
        std::vector<QString> item;
        item.push_back(query.value("item_id").toString());
        item.push_back(query.value("name").toString());

        warehouseItems.push_back(item);
    }

    return warehouseItems;
}

void ReceivingWorkerDatabase::removeItemFromDelivery(int deliveryId, int itemId) {
    QSqlQuery query(db);
    query.prepare("DELETE FROM delivery_items WHERE delivery_id = ? AND item_id = ?");
    query.addBindValue(deliveryId);
    query.addBindValue(itemId);

    if (!query.exec()) {
        throw std::runtime_error("Błąd przy usuwaniu przedmiotu z dostawy");
    }
}

void ReceivingWorkerDatabase::addItemToDelivery(int deliveryId, int itemId) {
    QSqlQuery query(db);
    query.prepare("INSERT INTO delivery_items (delivery_id, item_id, quantity) VALUES (?, ?, 0)");
    query.addBindValue(deliveryId);
    query.addBindValue(itemId);

    if (!query.exec()) {
        throw std::runtime_error("Błąd przy dodawaniu przedmiotu do dostawy");
    }
}

void ReceivingWorkerDatabase::submitDeliveryItem(int deliveryId, int itemId, int quantity) {
    QSqlQuery query(db);
    query.prepare("UPDATE delivery_items SET quantity = ? WHERE delivery_id = ? AND item_id = ?");
    query.addBindValue(quantity);
    query.addBindValue(deliveryId);
    query.addBindValue(itemId);

    if (!query.exec()) {
        throw std::runtime_error("Błąd przy zatwierdzaniu przedmiotu dostawy");
    }
}

