//
// Created by jakub on 2025-12-05.
//

#include "ManagerController.h"
//
// Created by jakub on 2025-12-05.
//

#include "ReceivingWorkerController.h"
#include <QSqlQuery>
#include <iostream>
#include <QDateTime>
#include <QSqlError>
#include <QDebug>


ManagerController::ManagerController(MainWindow* mainWindow, QSqlDatabase& db, int employeeId, int jobId):
    mainWindow(mainWindow), db(db), employeeId(employeeId), jobId(jobId) {
    //Setup views
    mainMenuView = new ManagerMainMenuView();
    logsView = new ManagerLogsView();
    vehiclesView = new ManagerVehiclesView();
    employeesView = new ManagerEmployeesView();

    mainWindow->addView(mainMenuView);
    mainWindow->addView(logsView);
    mainWindow->addView(vehiclesView);
    mainWindow->addView(employeesView);
    connectButtons();
}

void ManagerController::start() {
    QSqlQuery query(db);
    query.prepare("SELECT first_name, last_name, job, vehicle_id FROM view_employees_info WHERE employee_id = ?");
    query.addBindValue(employeeId);
    if (query.exec() && query.next()) {
        mainMenuView->setEmployeeInfo(employeeId, query.value("first_name").toString(), query.value("last_name").toString(), query.value("job").toString());
        QVariant vehicleVar = query.value("vehicle_id");
    }

    mainWindow->showView(mainMenuView);
}

void ManagerController::connectButtons() {
    //MainView
    connect(mainMenuView, &ManagerMainMenuView::logoutRequest, this, &ManagerController::handleLogout); // pass signal to AppController
    connect(mainMenuView, &ManagerMainMenuView::viewEmployees, this, &ManagerController::handleViewEmployees);
    connect(mainMenuView, &ManagerMainMenuView::viewVehicles, this, &ManagerController::handleViewVehicles);
    connect(mainMenuView, &ManagerMainMenuView::viewEventLog, this, &ManagerController::handleViewEventLog);
    connect(mainMenuView, &ManagerMainMenuView::viewOrders, this, &ManagerController::handleViewOrders);

    //LogsView
    connect(logsView, &ManagerLogsView::backToMainMenu, this, &ManagerController::handleBackButton);

    //VehiclesView
    connect(vehiclesView, &ManagerVehiclesView::backToMainMenu, this, &ManagerController::handleBackButton);
    connect(vehiclesView, &ManagerVehiclesView::saveVehicle, this, &ManagerController::handleSaveVehicle);

    //EmployeesView
    connect(employeesView, &ManagerEmployeesView::backToMainMenu, this, &ManagerController::handleBackButton);
    connect(employeesView, &ManagerEmployeesView::fireEmployee, this, &ManagerController::handleFireEmployee);
    connect(employeesView, &ManagerEmployeesView::modifyEmployee, this, &ManagerController::handleModifyEmployee);
    connect(employeesView, &ManagerEmployeesView::addEmployee, this, &ManagerController::handleAddEmployee);
}

void ManagerController::handleLogout() {
    emit logoutRequest();
}

void ManagerController::handleViewEmployees() {
    employeesView->clearEmployees();
    QMap<int, QString> jobMap;
    QSqlQuery jobQuery(db);
    jobQuery.prepare("SELECT job_id, job_name FROM jobs");
    if (!jobQuery.exec()) {
        mainMenuView->viewError("Nie udało się pobrać stanowisk");
        return;
    }
    while (jobQuery.next()) {
        int id = jobQuery.value("job_id").toInt();
        QString name = jobQuery.value("job_name").toString();
        jobMap[id] = name;
    }
    employeesView->setJobMap(jobMap);
    QSqlQuery query(db);
    query.prepare("SELECT employee_id, login, first_name, last_name, job_id, job, employed FROM view_employees");
    if (!query.exec()) {
        mainMenuView->viewError("Nie udało się pobrać listy pracowników");
        return;
    }

    while (query.next()) {
        int employeeId = query.value("employee_id").toInt();
        QString login = query.value("login").toString();
        QString firstName = query.value("first_name").toString();
        QString lastName = query.value("last_name").toString();
        int jobId = query.value("job_id").toInt();
        QString jobName = query.value("job").toString();
        bool employed = query.value("employed").toBool();

        employeesView->addEmployeeRow(employeeId, login, firstName, lastName, jobId, jobName, employed);
    }

    mainWindow->showView(employeesView);
}


void ManagerController::handleViewOrders() {
    //TODO
}

void ManagerController::handleViewVehicles() {
    vehiclesView->clearVehicles();
    QMap<int, QString> employeeMap; // employee_id -> "Imię Nazwisko"
    QSqlQuery empQuery(db);
    empQuery.prepare("SELECT employee_id, CONCAT(first_name, ' ', last_name) AS full_name "
                     "FROM employees WHERE job_id IN (1, 2)");
    if (!empQuery.exec()) {
        mainMenuView->viewError("Błąd przy pobieraniu pracowników: " + empQuery.lastError().text());
        return;
    }
    while (empQuery.next()) {
        int id = empQuery.value("employee_id").toInt();
        QString name = empQuery.value("full_name").toString();
        employeeMap[id] = name;
    }
    vehiclesView->setEmployeeMap(employeeMap);

    QSqlQuery vehQuery(db);
    vehQuery.prepare("SELECT vehicle_id, status, employee_id, employee_name FROM view_vehicles");
    if (!vehQuery.exec()) {
        mainMenuView->viewError("Błąd przy pobieraniu pojazdów: " + vehQuery.lastError().text());
        return;
    }
    while (vehQuery.next()) {
        int vehicleId = vehQuery.value("vehicle_id").toInt();
        QString status = vehQuery.value("status").toString();
        int assignedEmployeeId = vehQuery.value("employee_id").isNull() ? -1 :
                                 vehQuery.value("employee_id").toInt();

        vehiclesView->addVehicleRow(vehicleId, status, assignedEmployeeId);
    }

    mainWindow->showView(vehiclesView);
}


void ManagerController::handleViewEventLog() {
    logsView->clearLogs();

    QSqlQuery query(db);
    query.prepare(R"(
        SELECT event_type, employee_name
        FROM view_logs
        ORDER BY created_at DESC
    )");
    if (!query.exec()) {
        mainMenuView->viewError("Nie udało się pobrać wpisów dziennika");
        return;
    }
    while (query.next()) {
        QString eventType = query.value("event_type").toString();
        QString creator = query.value("employee_name").toString();
        if (creator.isEmpty()) creator = "Brak"; //if null
        logsView->addLogEntry(eventType, creator);
    }
    mainWindow->showView(logsView);
}


void ManagerController::handleBackButton() {
    start();
}

void ManagerController::handleSaveVehicle(int vehicleId, const QString &status, int employeeId) {
    QSqlQuery query(db);
    QString newStatus = status;
    // -1 is NULL
    if (employeeId != -1) {
        query.prepare("SELECT vehicle_id FROM vehicles WHERE employee_id = :employeeId AND vehicle_id != :vehicleId");
        query.bindValue(":employeeId", employeeId);
        query.bindValue(":vehicleId", vehicleId);
        if (!query.exec()) {
            mainMenuView->viewError("Błąd przy sprawdzaniu przypisania pracownika");
            return;
        }
        if (query.next()) { // There is already assigned vehicle
            mainMenuView->viewError("Wybrany pracownik jest już przypisany do innego pojazdu");
            return;
        }

        newStatus = "zajety";
    }
    //Update vehicle
    query.prepare("UPDATE vehicles SET status = :status, employee_id = :employeeId WHERE vehicle_id = :vehicleId");
    query.bindValue(":status", newStatus);
    if (employeeId != -1) {
        query.bindValue(":employeeId", employeeId);
    } else {
        query.bindValue(":employeeId", QVariant()); // NULL
    }
    query.bindValue(":vehicleId", vehicleId);
    if (!query.exec()) {
        mainMenuView->viewError("Nie udało się zapisać pojazdu.");
        return;
    }
    handleViewVehicles();
}

void ManagerController::handleFireEmployee(int employeeId) {
    QSqlQuery query(db);
    // Remove assigned vehicle
    query.prepare("UPDATE vehicles SET employee_id = NULL, status = 'wolny' WHERE employee_id = :employeeId");
    query.bindValue(":employeeId", employeeId);
    if (!query.exec()) {
        mainMenuView->viewError("Nie udało się usunąć przypisania do pojazdu");
        return;
    }

    query.prepare("UPDATE employees SET employed = false WHERE employee_id = :employeeId");
    query.bindValue(":employeeId", employeeId);
    if (!query.exec()) {
        mainMenuView->viewError("Nie udało się zwolnić pracownika");
        return;
    }

    handleViewEmployees();
}


void ManagerController::handleModifyEmployee(int employeeId, const QString &login, const QString &firstName, const QString &lastName, bool employed) {
    QSqlQuery query(db);
    query.prepare(R"( UPDATE employees SET login = :login, first_name = :firstName,
            last_name = :lastName, employed = :employed WHERE employee_id = :employeeId )");
    query.bindValue(":login", login);
    query.bindValue(":firstName", firstName);
    query.bindValue(":lastName", lastName);
    query.bindValue(":employed", employed);
    query.bindValue(":employeeId", employeeId);

    if (!query.exec()) {
        mainMenuView->viewError("Nie udało się zmodyfikować danych pracownika");
        return;
    }

    handleViewEmployees();
}

void ManagerController::handleAddEmployee(const QString &login, const QString &firstName, const QString &lastName, int jobId, const QString &password) {
    if (login.isEmpty() || firstName.isEmpty() || lastName.isEmpty() || password.isEmpty()) {
        mainMenuView->viewError("Wpisana wartosc jest pusta");
        return;
    }

    QSqlQuery query(db);
    query.prepare(R"( INSERT INTO employees (login, first_name, last_name, job_id, employed, password)
        VALUES (:login, :firstName, :lastName, :jobId, TRUE, :password))");
    query.bindValue(":login", login);
    query.bindValue(":firstName", firstName);
    query.bindValue(":lastName", lastName);
    query.bindValue(":jobId", jobId);
    query.bindValue(":password", password);

    if (!query.exec()) {
        mainMenuView->viewError("Nie udało się dodać pracownika");
        return;
    }
    handleViewEmployees();
}



