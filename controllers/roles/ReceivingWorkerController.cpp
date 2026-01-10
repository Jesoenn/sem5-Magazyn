//
// Created by jakub on 2025-12-05.
//

#include "ReceivingWorkerController.h"
#include <QSqlQuery>
#include <iostream>
#include <QDateTime>
#include <QSqlError>
#include <QDebug>


ReceivingWorkerController::ReceivingWorkerController(MainWindow* mainWindow, ReceivingWorkerDatabase& receivingWorkerDb, int employeeId, int jobId):
        mainWindow(mainWindow), receivingWorkerDb(receivingWorkerDb), employeeId(employeeId), jobId(jobId) {
    //Setup views
    mainMenuView = new ReceivingWorkerMainMenuView();
    orderListView = new ReceivingWorkerOrderListView();
    orderView = new WarehouseWorkerOrderView();
    deliveryListView = new ReceivingWorkerDeliveryListView();
    deliveryView = new ReceivingWorkerDeliveryView();

    mainWindow->addView(mainMenuView);
    mainWindow->addView(orderListView);
    mainWindow->addView(orderView);
    mainWindow->addView(deliveryListView);
    mainWindow->addView(deliveryView);
    connectButtons();
}

void ReceivingWorkerController::start() {
    std::vector<QString> employeeInfo = receivingWorkerDb.getEmployeeInfo(employeeId);
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

    mainWindow->showView(mainMenuView);
}

void ReceivingWorkerController::connectButtons() {
    //MainView
    connect(mainMenuView, &ReceivingWorkerMainMenuView::logoutRequest, this, &ReceivingWorkerController::handleLogout); // pass signal to AppController
    connect(mainMenuView, &ReceivingWorkerMainMenuView::assignVehicle, this, &ReceivingWorkerController::handleAssignVehicle);
    connect(mainMenuView, &ReceivingWorkerMainMenuView::freeVehicle, this, &ReceivingWorkerController::handleFreeVehicle);
    connect(mainMenuView, &ReceivingWorkerMainMenuView::viewDelivery, this, &ReceivingWorkerController::handleDeliveries);
    connect(mainMenuView, &ReceivingWorkerMainMenuView::getNewDelivery, this, &ReceivingWorkerController::handleNewDelivery);
    connect(mainMenuView, &ReceivingWorkerMainMenuView::verifyOrders, this, &ReceivingWorkerController::handleVerifyOrders);

    //OrderListView
    connect(orderListView, &ReceivingWorkerOrderListView::backToMainMenu, this, &ReceivingWorkerController::handleBackButton);
    connect(orderListView, &ReceivingWorkerOrderListView::checkOrder, this, &ReceivingWorkerController::handleCheckOrder);
    connect(orderListView, &ReceivingWorkerOrderListView::confirmOrder, this, &ReceivingWorkerController::handleConfirmOrder);

    //OrderView
    connect(orderView, &WarehouseWorkerOrderView::backToMainMenu, this, &ReceivingWorkerController::handleBackButton);
    qRegisterMetaType<QMap<int,int>>();
    connect(orderView, &WarehouseWorkerOrderView::submitOrder,this, &ReceivingWorkerController::handleOrderUpdate);

    //DeliveryListView
    connect(deliveryListView, &ReceivingWorkerDeliveryListView::backToMainMenu, this, &ReceivingWorkerController::handleBackButton);
    connect(deliveryListView, &ReceivingWorkerDeliveryListView::fillDelivery, this, &ReceivingWorkerController::handleFillDelivery);

    //DeliveryView
    connect(deliveryView, &ReceivingWorkerDeliveryView::backToMainMenu, this, &ReceivingWorkerController::handleBackButton);
    connect(deliveryView, &ReceivingWorkerDeliveryView::removeItem, this, &ReceivingWorkerController::handleRemoveItem);
    connect(deliveryView, &ReceivingWorkerDeliveryView::addItem, this, &ReceivingWorkerController::handleAddItem);
    connect(deliveryView, &ReceivingWorkerDeliveryView::submitDelivery, this, &ReceivingWorkerController::handleSubmitDelivery);


}

void ReceivingWorkerController::handleLogout() {
    mainWindow->removeView(mainMenuView);
    mainWindow->removeView(orderListView);
    mainWindow->removeView(orderView);
    mainWindow->removeView(deliveryListView);
    mainWindow->removeView(deliveryView);

    delete mainMenuView;
    delete orderListView;
    delete orderView;
    delete deliveryListView;
    delete deliveryView;

    mainMenuView = nullptr;
    orderListView = nullptr;
    orderView = nullptr;
    deliveryListView = nullptr;
    deliveryView = nullptr;
    emit logoutRequest();
}

void ReceivingWorkerController::handleNewDelivery() {
    try{
        receivingWorkerDb.newDelivery(employeeId);
    } catch (const std::runtime_error& e) {
        mainMenuView->viewError(QString::fromStdString(e.what()));
        return;
    }

    handleDeliveries(); //View all deliveries
}


void ReceivingWorkerController::handleDeliveries() {
    deliveryListView->clearDeliveries();
    std::vector<std::vector<QString>> deliveries;
    try{
        deliveries = receivingWorkerDb.getDeliveries(employeeId);
    } catch (const std::runtime_error& e) {
        mainMenuView->viewError(QString::fromStdString(e.what()));
        return;
    }
    for (const auto& delivery : deliveries) {
        int deliveryId = delivery[0].toInt();
        QString deliveryDate = delivery[1];
        deliveryListView->addDeliveryInfo(deliveryId, QDateTime::fromString(deliveryDate, "dd.MM.yyyy HH:mm"));
    }

    mainWindow->showView(deliveryListView);
}

void ReceivingWorkerController::handleBackButton() {
    start();
}

void ReceivingWorkerController::handleAssignVehicle() {
    try{
        receivingWorkerDb.assignVehicle(employeeId);
    }catch (const std::runtime_error& e) {
        mainMenuView->viewError(QString::fromStdString(e.what()));
        return;
    }
    start(); // refresh main menu gui
}


void ReceivingWorkerController::handleFreeVehicle() {
    try{
        receivingWorkerDb.freeVehicle(employeeId);
    }catch (const std::runtime_error& e) {
        mainMenuView->viewError(QString::fromStdString(e.what()));
        return;
    }
    start(); // refresh main menu gui
}

void ReceivingWorkerController::handleVerifyOrders() {
    orderListView->clearOrders();   // clear old orders

    //Get orders marked as finished ("gotowe")
    std::vector<std::vector<QString>> orders;
    try{
        orders = receivingWorkerDb.verifyOrders();
    }catch (const std::runtime_error& e) {
        mainMenuView->viewError(QString::fromStdString(e.what()));
        return;
    }

    //Add each order to orderListView
    for (const auto& order : orders) {
        int orderId = order[0].toInt();
        QString employeeName = order[1];
        QDateTime creationDate = QDateTime::fromString(order[2], "dd.MM.yyyy HH:mm");
        orderListView->addOrderInfo(orderId, employeeName, creationDate);
    }

    mainWindow->showView(orderListView);
}

void ReceivingWorkerController::handleConfirmOrder(int orderId) {
    try{
        receivingWorkerDb.confirmOrder(orderId);
    } catch (const std::runtime_error& e) {
        mainMenuView->viewError(QString::fromStdString(e.what()));
        return;
    }
    handleVerifyOrders(); //Refresh
}

void ReceivingWorkerController::handleCheckOrder(int orderId) {
    orderView->clearOrderItems();
    try{
        // Order information
        std::vector<QString> orderInfo = receivingWorkerDb.getOrderInfo(orderId);
        orderView->setOrderInfo(orderId, orderInfo[0], orderInfo[1]);

    } catch (const std::runtime_error& e) {
        mainMenuView->viewError(QString::fromStdString(e.what()));
        return;
    }

    // Items in order
    try {
        std::vector<std::vector<QString>> items = receivingWorkerDb.getOrderItems(orderId);
        for (const auto& item : items) {
            orderView->addOrderItem(item[0].toInt(), item[1], item[2].toInt(), item[3].toInt(), item[4].toInt());
        }
        mainWindow->showView(orderView);
    } catch (const std::runtime_error& e) {
        mainMenuView->viewError(e.what());
        return;
    }

    mainWindow->showView(orderView);
}

void ReceivingWorkerController::handleOrderUpdate(int orderId, const QMap<int,int>& pickedMap) {
    try {
        for (auto it = pickedMap.begin(); it != pickedMap.end(); it++) {
            receivingWorkerDb.updateOrder(orderId, it.key(), it.value());
        }
    } catch (const std::runtime_error& e) {
        orderView->viewError(QString::fromStdString(e.what()));
    }

    handleVerifyOrders(); // back to viewing all orders
}

void ReceivingWorkerController::handleFillDelivery(int deliveryId) {
    deliveryView->clearDeliveryItems();
    deliveryView->setDeliveryId(deliveryId);

    //Get delivery date
    try{
        QString date = receivingWorkerDb.getDeliveryDate(deliveryId);
        QDateTime deliveryDate = QDateTime::fromString(date, "dd.MM.yyyy HH:mm");
        deliveryView->setDeliveryInfo(deliveryId, deliveryDate);
    }catch (const std::runtime_error& e) {
        mainMenuView->viewError(QString::fromStdString(e.what()));
        return;
    }

    // Get delivery items
    QSet<int> deliveryItemIds; // Items id in this delivery
    try{
        std::vector<std::vector<QString>> deliveryItems= receivingWorkerDb.getDeliveryItems(deliveryId);
        // Add each delivery item to view
        for (const auto& item : deliveryItems) {
            int itemId = item[0].toInt();
            QString itemName = item[1];
            int quantity = item[2].toInt();

            deliveryView->addDeliveryItem(itemId, itemName, quantity);
            deliveryItemIds.insert(itemId);
        }
    } catch (const std::runtime_error& e) {
        mainMenuView->viewError(QString::fromStdString(e.what()));
        return;
    }

    // Get all items in warehouse
    try{
        std::vector<std::vector<QString>> warehouseItems = receivingWorkerDb.getWarehouseItems();
        for(const auto& item : warehouseItems) {
            int itemId = item[0].toInt();
            QString itemName = item[1];
            if (!deliveryItemIds.contains(itemId)) {
                deliveryView->addAvailableItem(itemId, itemName);  // <--- pojedynczo
            }
        }
    } catch (const std::runtime_error& e) {
        mainMenuView->viewError(QString::fromStdString(e.what()));
        return;
    }
    mainWindow->showView(deliveryView);
}

void ReceivingWorkerController::handleRemoveItem(int deliveryId, int itemId)
{
    try{
        receivingWorkerDb.removeItemFromDelivery(deliveryId, itemId);
    } catch (const std::runtime_error& e) {
        mainMenuView->viewError(QString::fromStdString(e.what()));
        return;
    }

    handleFillDelivery(deliveryId);
}

void ReceivingWorkerController::handleAddItem(int deliveryId, int itemId) {
    try{
        receivingWorkerDb.addItemToDelivery(deliveryId, itemId);
    } catch (const std::runtime_error& e) {
        mainMenuView->viewError(QString::fromStdString(e.what()));
        return;
    }
    handleFillDelivery(deliveryId);
}

void ReceivingWorkerController::handleSubmitDelivery(int deliveryId, const QMap<int, int> &quantities) {
    try{
        for (auto it = quantities.begin(); it != quantities.end(); ++it) {
            receivingWorkerDb.submitDeliveryItem(deliveryId, it.key(), it.value());
        }
    } catch (const std::runtime_error& e) {
        mainMenuView->viewError(QString::fromStdString(e.what()));
        return;
    }
}
