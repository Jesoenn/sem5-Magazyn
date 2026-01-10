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

}
