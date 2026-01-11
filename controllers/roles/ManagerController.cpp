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
    connect(orderView, &ManagerOrderView::backToOrdersList,this, &ManagerController::handleBackButton);
    connect(orderView, &ManagerOrderView::modifyOrderItem,this, &ManagerController::handleModifyOrderItem);
    connect(orderView, &ManagerOrderView::deleteOrderItem,this, &ManagerController::handleDeleteOrderItem);
    connect(orderView, &ManagerOrderView::addOrderItem,this, &ManagerController::handleAddOrderItem);
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

    try{
        managerDb.addEmployee(login, firstName, lastName, jobId, password);
    } catch (const std::runtime_error& e) {
        mainMenuView->viewError(QString::fromStdString(e.what()));
        return;
    }

    handleViewEmployees();
}

void ManagerController::handleAssignEmployee(int orderId, int employeeId) {
    try{
        managerDb.assignEmployeeToOrder(employeeId, orderId);
    } catch (const std::runtime_error& e) {
        mainMenuView->viewError(QString::fromStdString(e.what()));
        return;
    }
    handleViewOrders();
}

void ManagerController::handleModifyOrder(int orderId) {
    orderView->clearItems();

    try{
        std::vector<QString> orderInfo = managerDb.getOrderInfo(orderId);
        orderView->setOrderInfo(orderId, orderInfo[2], orderInfo[3]);
    } catch (const std::runtime_error& e) {
        mainMenuView->viewError(QString::fromStdString(e.what()));
        return;
    }

    std::vector<std::vector<QString>> orderItems;
    try {
        orderItems = managerDb.getOrderItems(orderId);
        for( const auto& item : orderItems) {
            int orderItemId = item[0].toInt();
            int itemId = item[1].toInt();
            QString itemName = item[2];
            int quantity = item[3].toInt();

            orderView->addOrderItemRow(orderItemId, itemId, itemName, quantity);
        }
    } catch (const std::runtime_error& e) {
        mainMenuView->viewError(QString::fromStdString(e.what()));
        return;
    }

    QMap<int, QString> availableItems; // item_id -> item_name
    try {
        std::vector<std::vector<QString>> items = managerDb.getItems();
        for (const auto& item : items) {
            int id = item[0].toInt();
            QString name = item[1];
            availableItems[id] = name;
        }
    } catch (const std::runtime_error& e) {
        mainMenuView->viewError(QString::fromStdString(e.what()));
        return;
    }
    orderView->setAvailableItems(availableItems);
    mainWindow->showView(orderView);
}


void ManagerController::handleDeleteOrder(int orderId) {
    try{
        managerDb.deleteOrder(orderId);
    } catch (const std::runtime_error& e) {
        mainMenuView->viewError(QString::fromStdString(e.what()));
        return;
    }
    handleViewOrders();
}

void ManagerController::handleCreateOrder(int employeeId) {
    try{
        managerDb.createOrder(this->employeeId, employeeId);
    } catch (const std::runtime_error& e) {
        mainMenuView->viewError(QString::fromStdString(e.what()));
        return;
    }

    handleViewOrders();
}

void ManagerController::handleModifyOrderItem(int orderId, int orderItemId, int newQuantity) {
    try {
        managerDb.modifyOrderItem(orderId, orderItemId, newQuantity);
    } catch (const std::runtime_error& e) {
        mainMenuView->viewError(e.what());
    }
    handleModifyOrder(orderId);
}

void ManagerController::handleDeleteOrderItem(int orderId, int orderItemId) {
    try {
        managerDb.returnAllPickedQuantities(orderId, orderItemId);
        managerDb.deleteOrderItem(orderId, orderItemId);
    } catch (const std::runtime_error& e) {
        mainMenuView->viewError(QString::fromStdString(e.what()));
    }
    handleModifyOrder(orderId);
}

void ManagerController::handleAddOrderItem(int orderId, int itemId, int quantity) {
    try {
        managerDb.addOrderItem(orderId, itemId, quantity);
    } catch (const std::runtime_error& e) {
        mainMenuView->viewError(QString::fromStdString(e.what()));
    }
    handleModifyOrder(orderId);
}


