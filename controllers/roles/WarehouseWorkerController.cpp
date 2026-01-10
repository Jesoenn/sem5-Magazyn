//
// Created by jakub on 2025-12-05.
//

#include <QSqlQuery>
#include <iostream>
#include <QDateTime>
#include <QSqlError>
#include "WarehouseWorkerController.h"


WarehouseWorkerController::WarehouseWorkerController(MainWindow* mainWindow, QSqlDatabase& db, WarehouseWorkerDatabase& warehouseWorkerDb, int employeeId, int jobId):
    mainWindow(mainWindow), db(db), employeeId(employeeId), jobId(jobId), warehouseWorkerDb(warehouseWorkerDb) {
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
    QSqlQuery query(db);

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
    QSqlQuery query(db);
    query.prepare("SELECT * FROM view_orders WHERE assigned_employee_id = ? AND status = \"realizacja\"");
    query.addBindValue(employeeId);

    if (!query.exec()) {
        mainMenuView->viewError("Błąd przy pobieraniu zamówienia");
        return;
    } else if (query.exec() && !query.next()) {
        QString message = "Brak aktywnego zamowienia";
        mainMenuView->viewError(message);
        return;
    }

    // Order information
    int orderId = query.value("order_id").toInt();
    QDateTime date = query.value("creation_date").toDateTime();
    QString creationDate = date.toString("dd.MM.yyyy HH:mm");   // Format date
    QString createdByName = query.value("created_by_name").toString();

    // Items in order
    QSqlQuery orderItemsQuery(db);
    orderItemsQuery.prepare("SELECT * FROM view_order_items WHERE order_id = ?");
    orderItemsQuery.addBindValue(orderId);

    if (!orderItemsQuery.exec()) {
        mainMenuView->viewError("Błąd przy pobieraniu zamówienia");
        return;
    }

    orderView->setOrderInfo(orderId, creationDate, createdByName);

    while (orderItemsQuery.next()) {
        int itemId = orderItemsQuery.value("item_id").toInt();
        QString itemName = orderItemsQuery.value("item_name").toString();
        int quantity = orderItemsQuery.value("quantity").toInt();
        int pickedQuantity = orderItemsQuery.value("picked_quantity").toInt();
        int availableQuantity = orderItemsQuery.value("available_quantity").toInt();
        orderView->addOrderItem(itemId, itemName, quantity, pickedQuantity, availableQuantity);
    }

    mainWindow->showView(orderView);
}

void WarehouseWorkerController::handleBackButton() {
    orderView->clearOrderItems();
    start();
}

void WarehouseWorkerController::handleOrderUpdate(int orderId, const QMap<int,int>& pickedMap) {
    QSqlQuery query(db);
    for (auto it = pickedMap.begin(); it != pickedMap.end(); it++) {
        query.prepare("UPDATE order_items SET picked_quantity = :picked WHERE item_id = :item_id and order_id = :order_id");
        query.bindValue(":picked", it.value());
        query.bindValue(":item_id", it.key());
        query.bindValue(":order_id", orderId);

        if (!query.exec()) {
            QString message = "Błąd dla item_id: " + QString::number(it.key());
            orderView->viewError(message);
        }
    }

    handleBackButton(); // back to main menu
}

void WarehouseWorkerController::handleSubmitNewOrder() {
    QSqlQuery query(db);
    query.prepare("UPDATE orders SET status = 'gotowe' WHERE assigned_employee_id = ? AND status = 'realizacja'");
    query.addBindValue(employeeId);
    if (!query.exec()) {
        mainMenuView->viewError("Błąd przy zatwierdzaniu zamówienia");
        return;
    }

    start();
}

void WarehouseWorkerController::handleAssignVehicle() {
    QSqlQuery query(db);
    query.prepare("UPDATE vehicles SET employee_id = ?, status = 'zajety' WHERE status = 'wolny' and employee_id is NULL LIMIT 1");
    query.addBindValue(employeeId);

    if (!query.exec()) {
        mainMenuView->viewError("Błąd przy przypisywaniu pojazdu");
        return;
    }
    start(); // refresh main menu gui
}


void WarehouseWorkerController::handleFreeVehicle() {
    QSqlQuery query(db);
    query.prepare("UPDATE vehicles SET status = 'wolny', employee_id = NULL WHERE employee_id = ?");
    query.addBindValue(employeeId);
    if (!query.exec()) {
        mainMenuView->viewError("Błąd przy zwalnianiu pojazdu");
        return;
    }
    start(); // refresh main menu gui
}

