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


ManagerController::ManagerController(MainWindow* mainWindow, QSqlDatabase& db, ManagerDatabase& managerDb, int employeeId, int jobId):
    mainWindow(mainWindow), db(db), managerDb(managerDb), employeeId(employeeId), jobId(jobId) {
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
    std::vector<QString> employeeInfo = managerDb.getEmployeeInfo(employeeId);
    mainMenuView->setEmployeeInfo(employeeId, employeeInfo[1], employeeInfo[2], employeeInfo[3]);
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

    try{
        std::vector<std::vector<QString>> jobs = managerDb.getJobs();
        for (const auto& job : jobs) {
            int id = job[0].toInt();
            QString name = job[1];
            jobMap[id] = name;
        }
        employeesView->setJobMap(jobMap);
    } catch (const std::runtime_error& e) {
        mainMenuView->viewError(QString::fromStdString(e.what()));
        return;
    }

    try{
        std::vector<std::vector<QString>> employees = managerDb.getEmployees();
        for (const auto& emp : employees) {
            int employeeId = emp[0].toInt();
            QString login = emp[1];
            QString firstName = emp[2];
            QString lastName = emp[3];
            int jobId = emp[4].toInt();
            QString jobName = emp[5];
            bool employed = (emp[6] == "1" || emp[6].toLower() == "true");

            employeesView->addEmployeeRow(employeeId, login, firstName, lastName, jobId, jobName, employed);
        }
    } catch (const std::runtime_error& e) {
        mainMenuView->viewError(QString::fromStdString(e.what()));
        return;
    }

    mainWindow->showView(employeesView);
}


void ManagerController::handleViewOrders(){
    ordersListView->clearOrders();
    QMap<int, QString> employeeMap;

    try{
        std::vector<std::vector<QString>> employees = managerDb.getEmployeesIdToName();
        for (const auto& employee : employees) {
            int id = employee[0].toInt();
            QString name = employee[1];
            employeeMap[id] = name;
        }
    } catch (const std::runtime_error& e) {
        mainMenuView->viewError(QString::fromStdString(e.what()));
        return;
    }
    ordersListView->setEmployeeMap(employeeMap);

    try{
        std::vector<std::vector<QString>> ordersInfo = managerDb.getOrdersInfo();
        for (const auto& order : ordersInfo) {
            int orderId = order[0].toInt();
            QString creatorName = order[1];
            QString createdAt = order[2];
            int assignedEmployeeId = order[3].isEmpty() ? -1 : order[3].toInt();

            ordersListView->addOrderRow(orderId, assignedEmployeeId, creatorName, createdAt);
        }
    } catch (const std::runtime_error& e) {
        mainMenuView->viewError(QString::fromStdString(e.what()));
        return;
    }
    mainWindow->showView(ordersListView);
}


void ManagerController::handleViewVehicles() {
    vehiclesView->clearVehicles();
    QMap<int, QString> employeeMap; // employee_id -> "Imię Nazwisko"

    try{
        std::vector<std::vector<QString>> employees = managerDb.getEmployeesCanHaveVehicle();
        for (const auto& employee : employees) {
            int id = employee[0].toInt();
            QString name = employee[1];
            employeeMap[id] = name;
        }
    } catch (const std::runtime_error& e) {
        mainMenuView->viewError(QString::fromStdString(e.what()));
        return;
    }
    vehiclesView->setEmployeeMap(employeeMap);

    try{
        std::vector<std::vector<QString>> vehiclesInfo = managerDb.getVehiclesInfo();
        for (const auto& vehicle : vehiclesInfo) {
            int vehicleId = vehicle[0].toInt();
            QString status = vehicle[1];
            int assignedEmployeeId = vehicle[2]=="null" ? -1 : vehicle[2].toInt();

            vehiclesView->addVehicleRow(vehicleId, status, assignedEmployeeId);
        }
    } catch (const std::runtime_error& e) {
        mainMenuView->viewError(QString::fromStdString(e.what()));
        return;
    }
    mainWindow->showView(vehiclesView);
}


void ManagerController::handleViewEventLog() {
    logsView->clearLogs();

    try{
        std::vector<std::vector<QString>> eventLog = managerDb.getEventLog();
        for (const auto& logEntry : eventLog) {
            QString eventType = logEntry[0];
            QString creator = logEntry[1];
            logsView->addLogEntry(eventType, creator);
        }
    } catch (const std::runtime_error& e) {
        mainMenuView->viewError(QString::fromStdString(e.what()));
        return;
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
    try{
        if (employeeId != -1){
            bool ans = managerDb.isEmployeeAssignedToVehicle(employeeId, vehicleId);
            if (ans) {
                mainMenuView->viewError("Wybrany pracownik jest już przypisany do pojazdu");
                return;
            }
            newStatus = "zajety";
        }
    } catch (const std::runtime_error& e) {
        mainMenuView->viewError(QString::fromStdString(e.what()));
        return;
    }

    //Update vehicle
    try{
        managerDb.assignVehicleToEmployee(vehicleId, employeeId);
    } catch (const std::runtime_error& e) {
        mainMenuView->viewError(QString::fromStdString(e.what()));
        return;
    }
    handleViewVehicles();
}

void ManagerController::handleFireEmployee(int employeeId) {
    try{
        managerDb.fireEmployee(employeeId);
    } catch (const std::runtime_error& e) {
        mainMenuView->viewError(QString::fromStdString(e.what()));
        return;
    }
    handleViewEmployees();
}


void ManagerController::handleModifyEmployee(int employeeId, const QString &login, const QString &firstName, const QString &lastName, bool employed) {
    try{
        managerDb.modifyEmployee(employeeId, login, firstName, lastName, employed);
    } catch (const std::runtime_error& e) {
        mainMenuView->viewError(QString::fromStdString(e.what()));
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




