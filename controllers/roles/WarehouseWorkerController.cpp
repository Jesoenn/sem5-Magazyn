//
// Created by jakub on 2025-12-05.
//

#include <QSqlQuery>
#include <iostream>
#include <QDateTime>
#include <QSqlError>
#include "WarehouseWorkerController.h"


WarehouseWorkerController::WarehouseWorkerController(MainWindow* mainWindow, WarehouseWorkerDatabase& warehouseWorkerDb, int employeeId, int jobId):
    mainWindow(mainWindow), employeeId(employeeId), jobId(jobId), warehouseWorkerDb(warehouseWorkerDb) {
    //Setup views
    mainMenuView = new WarehouseWorkerMainMenuView();
    orderView = new WarehouseWorkerOrderView();

    mainWindow->addView(mainMenuView);
    mainWindow->addView(orderView);
    connectButtons();
}

void WarehouseWorkerController::start() {
    std::vector<QString> employeeInfo = warehouseWorkerDb.getEmployeeInfo(employeeId);
    mainMenuView->setEmployeeInfo(employeeId, employeeInfo[1], employeeInfo[2], employeeInfo[3]);
    if (employeeInfo[4] == "null") {
        mainMenuView->setAssignedVehicle("Brak przypisanego pojazdu");
        mainMenuView->showAssignVehicleButton(true);
        mainMenuView->showFreeVehicleButton(false);
    } else {
        mainMenuView->setAssignedVehicle(employeeInfo[4]);
        mainMenuView->showAssignVehicleButton(false);
        mainMenuView->showFreeVehicleButton(true);
    }

    int activeOrders = warehouseWorkerDb.getActiveOrderCount(employeeId);
    if (activeOrders > 0) {
        mainMenuView->showNewOrderButton(false);
        mainMenuView->showSubmitOrderButton(true);
        mainMenuView->showViewOrderButton(true);
    } else {
        mainMenuView->showNewOrderButton(true);
        mainMenuView->showSubmitOrderButton(false);
        mainMenuView->showViewOrderButton(false);
    }

    mainWindow->showView(mainMenuView);
}

void WarehouseWorkerController::connectButtons() {
    connect(mainMenuView, &WarehouseWorkerMainMenuView::logoutRequest, this, &WarehouseWorkerController::handleLogout); // pass signal to AppController
    connect(mainMenuView, &WarehouseWorkerMainMenuView::getNewOrder, this, &WarehouseWorkerController::handleNewOrder); // pass signal to AppController
    connect(mainMenuView, &WarehouseWorkerMainMenuView::viewOrder, this, &WarehouseWorkerController::handleCurrentOrder);
    connect(mainMenuView, &WarehouseWorkerMainMenuView::assignVehicle, this, &WarehouseWorkerController::handleAssignVehicle);
    connect(mainMenuView, &WarehouseWorkerMainMenuView::freeVehicle, this, &WarehouseWorkerController::handleFreeVehicle);
    connect(mainMenuView, &WarehouseWorkerMainMenuView::submitOrder, this, &WarehouseWorkerController::handleSubmitNewOrder);

    connect(orderView, &WarehouseWorkerOrderView::backToMainMenu, this, &WarehouseWorkerController::handleBackButton);
    qRegisterMetaType<QMap<int,int>>();
    connect(orderView, &WarehouseWorkerOrderView::submitOrder,this, &WarehouseWorkerController::handleOrderUpdate);
}

void WarehouseWorkerController::handleLogout() {
    mainWindow->removeView(mainMenuView);
    mainWindow->removeView(orderView);

    delete mainMenuView;
    delete orderView;
    mainMenuView = nullptr;
    orderView = nullptr;

    emit logoutRequest();
}

void WarehouseWorkerController::handleNewOrder() {
    int activeOrders = warehouseWorkerDb.getActiveOrderCount(employeeId);
    if (activeOrders>0){
        mainMenuView->viewError("Masz już aktywne zamówienie do realizacji");
        return;
    } else if (activeOrders == -1){
        mainMenuView->viewError("Błąd przy sprawdzaniu aktywnych zamówień");
        return;
    }

    int newOrderId = warehouseWorkerDb.getFreeOrder(employeeId);
    if (newOrderId == -1) {
        mainMenuView->viewError("Brak dostępnych nowych zamówień");
        return;
    }

    // Assign order to employee
    int ans = warehouseWorkerDb.assignOrder(newOrderId, employeeId);
    if (ans == -1) {
        mainMenuView->viewError("Błąd przy przypisywaniu nowego zamówienia");
        return;
    }
    start();
}


void WarehouseWorkerController::handleCurrentOrder() {
    std::vector<QString> orderInfo = warehouseWorkerDb.getCurrentOrderInfo(employeeId);
    int orderId = orderInfo[0].toInt();
    QString creationDate = orderInfo[1];
    QString createdByName = orderInfo[2];
    orderView->setOrderInfo(orderId, creationDate, createdByName);

    // Items in order
    std::vector<std::vector<QString>> items = warehouseWorkerDb.getOrderItems(orderId);
    for (const auto& item : items) {
        int itemId = item[0].toInt();
        QString itemName = item[1];
        int quantity = item[2].toInt();
        int pickedQuantity = item[3].toInt();
        int availableQuantity = item[4].toInt();

        orderView->addOrderItem(itemId, itemName, quantity, pickedQuantity, availableQuantity);
    }
    mainWindow->showView(orderView);
}


void WarehouseWorkerController::handleBackButton() {
    orderView->clearOrderItems();
    start();
}

void WarehouseWorkerController::handleOrderUpdate(int orderId, const QMap<int,int>& pickedMap) {
    try {
        for (auto it = pickedMap.begin(); it != pickedMap.end(); it++) {
            warehouseWorkerDb.orderUpdate(orderId, it.key(), it.value());
        }
    } catch (const std::runtime_error& e) {
        orderView->viewError(QString::fromStdString(e.what()));
    }
    handleBackButton(); // back to main menu
}

void WarehouseWorkerController::handleSubmitNewOrder() {
    try{
        warehouseWorkerDb.submitOrder(employeeId);
    } catch (const std::runtime_error& e) {
        mainMenuView->viewError(QString::fromStdString(e.what()));
        return;
    }

    start();
}

void WarehouseWorkerController::handleAssignVehicle() {
    try {
        warehouseWorkerDb.assignVehicle(employeeId);
    } catch (const std::runtime_error& e) {
        mainMenuView->viewError(QString::fromStdString(e.what()));
        return;
    }

    start(); // refresh main menu gui
}


void WarehouseWorkerController::handleFreeVehicle() {
    try{
        warehouseWorkerDb.freeVehicle(employeeId);
    } catch (const std::runtime_error& e) {
        mainMenuView->viewError(QString::fromStdString(e.what()));
        return;
    }

    start(); // refresh main menu gui
}

