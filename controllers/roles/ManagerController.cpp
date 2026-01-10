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
    ordersListView = new ManagerOrdersListView();
    orderView = new ManagerOrderView();

    mainWindow->addView(mainMenuView);
    mainWindow->addView(logsView);
    mainWindow->addView(vehiclesView);
    mainWindow->addView(employeesView);
    mainWindow->addView(ordersListView);
    mainWindow->addView(orderView);
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

    //OrdersListView
    connect(ordersListView, &ManagerOrdersListView::backToMainMenu, this, &ManagerController::handleBackButton);
    connect(ordersListView, &ManagerOrdersListView::assignEmployee, this, &ManagerController::handleAssignEmployee);
    connect(ordersListView, &ManagerOrdersListView::createOrder, this, &ManagerController::handleCreateOrder);
    connect(ordersListView, &ManagerOrdersListView::deleteOrder, this, &ManagerController::handleDeleteOrder);
    connect(ordersListView, &ManagerOrdersListView::modifyOrder, this, &ManagerController::handleModifyOrder);

    //OrderView
    connect(orderView, &ManagerOrderView::backToOrdersList, this, &ManagerController::handleBackButton);
}

void ManagerController::handleLogout() {
    mainWindow->removeView(mainMenuView);
    mainWindow->removeView(logsView);
    mainWindow->removeView(vehiclesView);
    mainWindow->removeView(employeesView);
    mainWindow->removeView(ordersListView);
    mainWindow->removeView(orderView);

    delete mainMenuView;
    delete logsView;
    delete vehiclesView;
    delete employeesView;
    delete ordersListView;
    delete orderView;

    mainMenuView = nullptr;
    logsView = nullptr;
    vehiclesView = nullptr;
    employeesView = nullptr;
    ordersListView = nullptr;
    orderView = nullptr;

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


void ManagerController::handleViewOrders()
{
    ordersListView->clearOrders();
    QMap<int, QString> employeeMap;
    QSqlQuery empQuery(db);
    empQuery.prepare("SELECT employee_id, CONCAT(first_name, ' ', last_name) AS full_name FROM employees WHERE job_id = 1 AND employed = TRUE");
    if (!empQuery.exec()) {
        mainMenuView->viewError("Błąd przy pobieraniu listy pracowników");
        return;
    }

    while (empQuery.next()) {
        int id = empQuery.value("employee_id").toInt();
        QString fullName = empQuery.value("full_name").toString();
        employeeMap[id] = fullName;
    }
    ordersListView->setEmployeeMap(employeeMap);

    QSqlQuery orderQuery(db);
    orderQuery.prepare("SELECT order_id, creation_date, created_by_name, "
                       "assigned_employee_id "
                       "FROM view_orders ORDER BY order_id DESC");

    if (!orderQuery.exec()) {
        mainMenuView->viewError("Błąd przy pobieraniu zamówień");
        return;
    }
    while (orderQuery.next()) {
        int orderId = orderQuery.value("order_id").toInt();
        QDateTime createdAtDate = orderQuery.value("creation_date").toDateTime();
        QString createdAt = createdAtDate.toString("dd.MM.yyyy HH:mm");
        QString creatorName = orderQuery.value("created_by_name").toString();

        int assignedEmployeeId = orderQuery.value("assigned_employee_id").isNull() ? -1: orderQuery.value("assigned_employee_id").toInt();

        ordersListView->addOrderRow(orderId, assignedEmployeeId, creatorName, createdAt);
    }
    mainWindow->showView(ordersListView);
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

void ManagerController::handleAssignEmployee(int orderId, int employeeId) {
    QSqlQuery query(db);
    query.prepare("UPDATE orders SET assigned_employee_id = :employeeId WHERE order_id = :orderId");
    query.bindValue(":orderId", orderId);
    query.bindValue(":employeeId", employeeId);
    if (!query.exec()) {
        mainMenuView->viewError("Nie udało się przypisać pracownika");
        return;
    }
    handleViewOrders();
}

void ManagerController::handleModifyOrder(int orderId) {
    //TODO ZROBIC TO OD NOWA
    // Wyczyść poprzednie pozycje
    orderView->clearItems();

    // --- Pobranie informacji o zamówieniu ---
    QSqlQuery orderQuery(db);
    orderQuery.prepare(R"(SELECT o.order_id, o.created_by_id, CONCAT(c.first_name, ' ', c.last_name) AS created_by_name,
                                o.creation_date
                         FROM orders o
                         JOIN employees c ON o.created_by_id = c.employee_id
                         WHERE o.order_id = :orderId)");
    orderQuery.bindValue(":orderId", orderId);

    if (!orderQuery.exec() || !orderQuery.next()) {
        mainMenuView->viewError("Nie udało się pobrać danych zamówienia");
        return;
    }

    QString creatorName = orderQuery.value("created_by_name").toString();
    QString createdAt = orderQuery.value("creation_date").toDateTime().toString("yyyy-MM-dd HH:mm");

    orderView->setOrderInfo(orderId, creatorName, createdAt);

    // --- Pobranie pozycji zamówienia ---
    QSqlQuery itemsQuery(db);
    itemsQuery.prepare(R"(SELECT oi.order_item_id, oi.item_id, i.name AS item_name, oi.quantity
                         FROM order_items oi
                         JOIN items i ON oi.item_id = i.item_id
                         WHERE oi.order_id = :orderId)");
    itemsQuery.bindValue(":orderId", orderId);

    if (!itemsQuery.exec()) {
        mainMenuView->viewError("Nie udało się pobrać pozycji zamówienia");
        return;
    }

    QMap<int, QString> availableItems; // item_id -> item_name
    QSqlQuery itemsMapQuery(db);
    itemsMapQuery.prepare("SELECT item_id, name FROM items");
    if (itemsMapQuery.exec()) {
        while (itemsMapQuery.next()) {
            int itemId = itemsMapQuery.value("item_id").toInt();
            QString name = itemsMapQuery.value("name").toString();
            availableItems[itemId] = name;
        }
    }

    orderView->setAvailableItems(availableItems);

    while (itemsQuery.next()) {
        int orderItemId = itemsQuery.value("order_item_id").toInt();
        int itemId = itemsQuery.value("item_id").toInt();
        QString itemName = itemsQuery.value("item_name").toString();
        int quantity = itemsQuery.value("quantity").toInt();

        orderView->addOrderItemRow(orderItemId, itemId, itemName, quantity);
    }

    mainWindow->showView(orderView);
}


void ManagerController::handleDeleteOrder(int orderId) {
    QSqlQuery query(db);
    query.prepare("SELECT status FROM orders WHERE order_id = :orderId");
    query.bindValue(":orderId", orderId);
    if (!query.exec()) {
        mainMenuView->viewError("Nie udało się sprawdzić statusu zamówienia");
        return;
    }
    query.next();
    QString status = query.value("status").toString();
    if (status != "nowe") {
        mainMenuView->viewError("Można usuwać tylko zamówienia o statusie 'nowe'");
        return;
    }
    query.prepare("DELETE FROM order_items WHERE order_id = :orderId");
    query.bindValue(":orderId", orderId);
    if (!query.exec()) {
        mainMenuView->viewError("Nie udało się usunąć pozycji zamówienia");
        return;
    }
    query.prepare("DELETE FROM orders WHERE order_id = :orderId");
    query.bindValue(":orderId", orderId);
    if (!query.exec()) {
        mainMenuView->viewError("Nie udało się usunąć zamówienia");
        return;
    }
    handleViewOrders();
}

void ManagerController::handleCreateOrder(int employeeId) {
    QSqlQuery query(db);
    query.prepare(R"(INSERT INTO orders (created_by_id, assigned_employee_id)
        VALUES (:creator, :assigned))");

    query.bindValue(":creator", this->employeeId);
    query.bindValue(":assigned", employeeId);
    if (!query.exec()) {
        mainMenuView->viewError("Nie udało się utworzyć zamówienia");
        return;
    }
    handleViewOrders();
}




