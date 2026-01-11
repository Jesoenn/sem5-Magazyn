//
// Created by jakub on 2026-01-10.
//

#include "ManagerDatabase.h"

ManagerDatabase::ManagerDatabase(QSqlDatabase db): db(db) {

}

std::vector<QString> ManagerDatabase::getEmployeeInfo(int employeeId) {
    QSqlQuery query(db);
    query.prepare("SELECT first_name, last_name, job, vehicle_id FROM view_employees_info WHERE employee_id = ?");
    query.addBindValue(employeeId);

    std::vector<QString> employeeInfo;

    if (query.exec() && query.next()) {
        employeeInfo.push_back(QString::number(employeeId));
        employeeInfo.push_back(query.value("first_name").toString());
        employeeInfo.push_back(query.value("last_name").toString());
        employeeInfo.push_back(query.value("job").toString());
    } else {
        for (int i = 0; i < 5; i++) {
            employeeInfo.push_back("Unknown");
        }
    }
    return employeeInfo;
}

std::vector<std::vector<QString>> ManagerDatabase::getJobs() {
    QSqlQuery query(db);
    query.prepare("SELECT job_id, job_name FROM jobs");
    if (!query.exec()) {
        throw std::runtime_error("Błąd przy pobieraniu listy stanowisk");
    }

    std::vector<std::vector<QString>> jobs;
    while (query.next()) {
        std::vector<QString> job;
        job.push_back(query.value("job_id").toString());
        job.push_back(query.value("job_name").toString());
        jobs.push_back(job);
    }
    return jobs;
}

std::vector<std::vector<QString>> ManagerDatabase::getEmployees() {
    QSqlQuery query(db);
    query.prepare("SELECT employee_id, login, first_name, last_name, job_id, job, employed FROM view_employees");
    if (!query.exec()) {
        throw std::runtime_error("Błąd przy pobieraniu listy pracowników");
    }

    std::vector<std::vector<QString>> employees;
    while (query.next()) {
        std::vector<QString> employee;
        employee.push_back(query.value("employee_id").toString());
        employee.push_back(query.value("login").toString());
        employee.push_back(query.value("first_name").toString());
        employee.push_back(query.value("last_name").toString());
        employee.push_back(query.value("job_id").toString());
        employee.push_back(query.value("job").toString());
        employee.push_back(query.value("employed").toString());
        employees.push_back(employee);
    }
    return employees;
}

std::vector<std::vector<QString>> ManagerDatabase::getEmployeesIdToName() {
    QSqlQuery query(db);
    query.prepare("SELECT employee_id, CONCAT(first_name, ' ', last_name) AS full_name FROM employees WHERE employed = TRUE");
    if (!query.exec()) {
        throw std::runtime_error("Błąd przy pobieraniu listy pracowników");
    }

    std::vector<std::vector<QString>> employees;
    while (query.next()) {
        std::vector<QString> employee;
        employee.push_back(query.value("employee_id").toString());
        employee.push_back(query.value("full_name").toString());
        employees.push_back(employee);
    }
    return employees;
}

std::vector<std::vector<QString>> ManagerDatabase::getOrdersInfo() {
    QSqlQuery query(db);
    query.prepare("SELECT order_id, creation_date, created_by_name, assigned_employee_id FROM view_orders ORDER BY order_id DESC");
    if (!query.exec()) {
        throw std::runtime_error("Błąd przy pobieraniu listy zamówień");
    }

    std::vector<std::vector<QString>> orders;
    while (query.next()) {
        std::vector<QString> order;
        order.push_back(query.value("order_id").toString());

        QDateTime creationDate = query.value("creation_date").toDateTime();
        order.push_back(creationDate.toString("dd.MM.yyyy HH:mm"));

        order.push_back(query.value("created_by_name").toString());
        QVariant assignedEmployeeVar = query.value("assigned_employee_id");
        if (assignedEmployeeVar.isNull()) {
            order.push_back("null");
        } else {
            order.push_back(assignedEmployeeVar.toString());
        }

        orders.push_back(order);
    }
    return orders;
}

std::vector<std::vector<QString>> ManagerDatabase::getEmployeesCanHaveVehicle() {
    QSqlQuery query(db);
    query.prepare("SELECT employee_id, CONCAT(first_name, ' ', last_name) AS full_name FROM employees WHERE job_id IN (1, 2) AND employed = TRUE");
    if (!query.exec()) {
        throw std::runtime_error("Błąd przy pobieraniu listy pracowników mogących mieć pojazd");
    }

    std::vector<std::vector<QString>> employees;
    while (query.next()) {
        std::vector<QString> employee;
        employee.push_back(query.value("employee_id").toString());
        employee.push_back(query.value("full_name").toString());
        employees.push_back(employee);
    }
    return employees;
}

std::vector<std::vector<QString>> ManagerDatabase::getVehiclesInfo() {
    QSqlQuery query(db);
    query.prepare("SELECT vehicle_id, status, employee_id, employee_name FROM view_vehicles");
    if (!query.exec()) {
        throw std::runtime_error("Błąd przy pobieraniu listy pojazdów");
    }

    std::vector<std::vector<QString>> vehicles;
    while (query.next()) {
        std::vector<QString> vehicle;
        vehicle.push_back(query.value("vehicle_id").toString());
        vehicle.push_back(query.value("status").toString());

        QVariant employeeVar = query.value("employee_id");
        if (employeeVar.isNull()) {
            vehicle.push_back("null");
        } else {
            vehicle.push_back(employeeVar.toString());
        }

        QVariant employeeNameVar = query.value("employee_name");
        if (employeeNameVar.isNull()) {
            vehicle.push_back("null");
        } else {
            vehicle.push_back(employeeNameVar.toString());
        }

        vehicles.push_back(vehicle);
    }
    return vehicles;
}

std::vector<std::vector<QString>> ManagerDatabase::getEventLog() {
    QSqlQuery query(db);
    query.prepare("SELECT event_type, employee_name FROM view_logs ORDER BY created_at DESC");
    if (!query.exec()) {
        throw std::runtime_error("Błąd przy pobieraniu wpisów dziennika");
    }

    std::vector<std::vector<QString>> logs;
    while (query.next()) {
        std::vector<QString> log;
        log.push_back(query.value("event_type").toString());

        QVariant employeeNameVar = query.value("employee_name");
        if (employeeNameVar.isNull()) {
            log.push_back("Brak");
        } else {
            log.push_back(employeeNameVar.toString());
        }

        logs.push_back(log);
    }
    return logs;
}

bool ManagerDatabase::isEmployeeAssignedToVehicle(int employeeId, int vehicleId) {
    QSqlQuery query(db);
    query.prepare("SELECT vehicle_id FROM vehicles WHERE employee_id = :employeeId AND vehicle_id != :vehicleId");
    query.addBindValue(employeeId);
    query.addBindValue(vehicleId);
    if (!query.exec()) {
        throw std::runtime_error("Błąd przy sprawdzaniu przypisania pojazdu");
    }
    return query.next(); // If >0 result, employee is assigned to a vehicle
}

void ManagerDatabase::assignVehicleToEmployee(int vehicleId, int employeeId) {
    QSqlQuery query(db);
    query.prepare("UPDATE vehicles SET employee_id = :employeeId, status = 'zajety' WHERE vehicle_id = :vehicleId");
    if (employeeId != -1) {
        query.bindValue(":employeeId", employeeId);
    } else {
        query.bindValue(":employeeId", QVariant()); // NULL
    }
    query.bindValue(":vehicleId", vehicleId);
    if (!query.exec()) {
        throw std::runtime_error("Błąd przy przypisywaniu pojazdu do pracownika");
    }
}

void ManagerDatabase::fireEmployee(int employeeId) {
    QSqlQuery query(db);
    query.prepare("UPDATE vehicles SET employee_id = NULL, status = 'wolny' WHERE employee_id = :employeeId");
    query.bindValue(":employeeId", employeeId);
    if (!query.exec()) {
        throw std::runtime_error("Błąd przy usuwaniu przypisania do pojazdu");
    }

    query.prepare("UPDATE employees SET employed = false WHERE employee_id = :employeeId");
    query.bindValue(":employeeId", employeeId);
    if (!query.exec()) {
        throw std::runtime_error("Błąd przy zwalnianiu pracownika");
    }
}

void ManagerDatabase::modifyEmployee(int employeeId, const QString login, const QString firstName, const QString lastName, bool employed) {
    QSqlQuery query(db);
    query.prepare(R"( UPDATE employees SET login = :login, first_name = :firstName,
            last_name = :lastName, employed = :employed WHERE employee_id = :employeeId )");
    query.bindValue(":login", login);
    query.bindValue(":firstName", firstName);
    query.bindValue(":lastName", lastName);
    query.bindValue(":employed", employed);
    query.bindValue(":employeeId", employeeId);

    if (!query.exec()) {
        throw std::runtime_error("Błąd przy modyfikowaniu danych pracownika");
    }
}

void ManagerDatabase::addEmployee(const QString &login, const QString &firstName, const QString &lastName, int jobId, const QString &password) {
    QSqlQuery query(db);
    query.prepare(R"( INSERT INTO employees (login, first_name, last_name, job_id, employed, password)
        VALUES (:login, :firstName, :lastName, :jobId, TRUE, :password))");
    query.bindValue(":login", login);
    query.bindValue(":firstName", firstName);
    query.bindValue(":lastName", lastName);
    query.bindValue(":jobId", jobId);
    query.bindValue(":password", password);

    if (!query.exec()) {
        throw std::runtime_error("Błąd przy dodawaniu nowego pracownika");
    }
}

void ManagerDatabase::assignEmployeeToOrder(int employeeId, int orderId) {
    QSqlQuery query(db);
    query.prepare("UPDATE orders SET assigned_employee_id = :employeeId WHERE order_id = :orderId");
    query.bindValue(":orderId", orderId);
    query.bindValue(":employeeId", employeeId);
    if (!query.exec()) {
        throw std::runtime_error("Błąd przy przypisywaniu pracownika do zamówienia");
    }
}

void ManagerDatabase::deleteOrder(int orderId) {
    QSqlQuery query(db);
    query.prepare("SELECT status FROM orders WHERE order_id = :orderId");
    query.bindValue(":orderId", orderId);
    if (!query.exec()) {
        throw std::runtime_error("Błąd przy sprawdzaniu statusu zamówienia");
    }
    query.next();
    QString status = query.value("status").toString();
    if (status != "nowe") {
        throw std::runtime_error("Można usuwać tylko zamówienia o statusie 'nowe'");
    }
    query.prepare("DELETE FROM order_items WHERE order_id = :orderId");
    query.bindValue(":orderId", orderId);
    if (!query.exec()) {
        throw std::runtime_error("Błąd przy usuwaniu pozycji zamówienia");
    }
    query.prepare("DELETE FROM orders WHERE order_id = :orderId");
    query.bindValue(":orderId", orderId);
    if (!query.exec()) {
        throw std::runtime_error("Błąd przy usuwaniu zamówienia");
    }
}

void ManagerDatabase::createOrder(int creatorId, int assignedEmployeeId) {
    QSqlQuery query(db);
    query.prepare(R"(INSERT INTO orders (created_by_id, assigned_employee_id)
        VALUES (:creator, :assigned))");

    query.bindValue(":creator", creatorId);
    query.bindValue(":assigned", assignedEmployeeId);
    if (!query.exec()) {
        throw std::runtime_error("Błąd przy tworzeniu zamówienia");
    }
}

void ManagerDatabase::modifyOrderItem(int orderId, int orderItemId, int newQuantity) {
    QSqlQuery query(db);

    query.prepare("UPDATE order_items SET quantity = :quantity WHERE order_id = :order_id AND order_item_id = :order_item_id");
    query.bindValue(":quantity", newQuantity);
    query.bindValue(":order_id", orderId);
    query.bindValue(":order_item_id", orderItemId);

    if (!query.exec()) {
        throw std::runtime_error("Błąd modyfikacji pozycji zamówienia");
    }
}

void ManagerDatabase::deleteOrderItem(int orderId, int orderItemId) {
    QSqlQuery query(db);

    query.prepare("DELETE FROM order_items WHERE order_id = :order_id AND order_item_id = :order_item_id");
    query.bindValue(":order_id", orderId);
    query.bindValue(":order_item_id", orderItemId);

    if (!query.exec()) {
        throw std::runtime_error("Błąd przy usuwaniu pozycji zamówienia");
    }

    if (query.numRowsAffected() == 0) {
        throw std::runtime_error("Nie znaleziono pozycji do usunięcia");
    }
}

void ManagerDatabase::returnAllPickedQuantities(int orderId, int orderItemId) {
    QSqlQuery query(db);
    //Return order picked quantity do items
    query.prepare("UPDATE items "
                  "SET quantity = quantity + (SELECT picked_quantity FROM order_items WHERE order_id = :order_id AND order_item_id = :order_item_id) "
                  "WHERE item_id = (SELECT item_id FROM order_items WHERE order_id = :order_id AND order_item_id = :order_item_id)");

    query.bindValue(":order_id", orderId);
    query.bindValue(":order_item_id", orderItemId);

    if (!query.exec()) {
        throw std::runtime_error("Błąd przy zwracaniu ilości pozycji do magazynu");
    }
}

void ManagerDatabase::addOrderItem(int orderId, int itemId, int quantity) {
    QSqlQuery query(db);

    query.prepare("INSERT INTO order_items (order_id, item_id, quantity, picked_quantity) "
                  "VALUES (:order_id, :item_id, :quantity, 0)");

    query.bindValue(":order_id", orderId);
    query.bindValue(":item_id", itemId);
    query.bindValue(":quantity", quantity);

    if (!query.exec()) {
        throw std::runtime_error("Błąd przy dodawaniu nowej pozycji do zamówienia");
    }
}

std::vector<QString> ManagerDatabase::getOrderInfo(int orderId) {
    QSqlQuery query(db);
    query.prepare(R"(SELECT o.order_id, o.created_by_id,
                            CONCAT(c.first_name, ' ', c.last_name) AS created_by_name,
                            o.creation_date
                     FROM orders o
                     JOIN employees c ON o.created_by_id = c.employee_id
                     WHERE o.order_id = :orderId)");
    query.bindValue(":orderId", orderId);

    std::vector<QString> orderInfo;

    if (!query.exec()) {
        throw std::runtime_error("Błąd przy pobieraniu danych zamówienia");
    }

    if (query.next()) {
        orderInfo.push_back(query.value("order_id").toString());
        orderInfo.push_back(query.value("created_by_id").toString());
        orderInfo.push_back(query.value("created_by_name").toString());
        orderInfo.push_back(query.value("creation_date").toDateTime().toString("yyyy-MM-dd HH:mm"));
    } else {
        throw std::runtime_error("Nie znaleziono zamówienia o podanym ID");
    }

    return orderInfo;
}

std::vector<std::vector<QString>> ManagerDatabase::getOrderItems(int orderId) {
    std::vector<std::vector<QString>> items;

    QSqlQuery query(db);
    query.prepare(R"(SELECT oi.order_item_id, oi.item_id, i.name AS item_name, oi.quantity
                     FROM order_items oi
                     JOIN items i ON oi.item_id = i.item_id
                     WHERE oi.order_id = :orderId)");
    query.bindValue(":orderId", orderId);

    if (!query.exec()) {
        throw std::runtime_error("Nie udało się pobrać pozycji zamówienia");
    }

    while (query.next()) {
        std::vector<QString> row;
        row.push_back(query.value("order_item_id").toString());
        row.push_back(query.value("item_id").toString());
        row.push_back(query.value("item_name").toString());
        row.push_back(query.value("quantity").toString());

        items.push_back(row);
    }

    return items;
}

std::vector<std::vector<QString>> ManagerDatabase::getItems() {
    std::vector<std::vector<QString>> items;

    QSqlQuery query(db);
    query.prepare("SELECT item_id, name FROM items");

    if (!query.exec()) {
        throw std::runtime_error("Nie udało się pobrać listy towarów");
    }

    while (query.next()) {
        std::vector<QString> row;
        row.push_back(query.value("item_id").toString());
        row.push_back(query.value("name").toString());
        items.push_back(row);
    }

    return items;
}



