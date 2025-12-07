//
// Created by jakub on 2025-12-05.
//

#include "ReceivingWorkerController.h"
#include <QSqlQuery>
#include <iostream>
#include <QDateTime>
#include <QSqlError>
#include <QDebug>


ReceivingWorkerController::ReceivingWorkerController(MainWindow* mainWindow, QSqlDatabase& db, int employeeId, int jobId):
        mainWindow(mainWindow), db(db), employeeId(employeeId), jobId(jobId) {
    //Setup views
    mainMenuView = new ReceivingWorkerMainMenuView();
    orderListView = new ReceivingWorkerOrderListView();
    orderView = new WarehouseWorkerOrderView();
    deliveryListView = new ReceivingWorkerDeliveryListView();

    mainWindow->addView(mainMenuView);
    mainWindow->addView(orderListView);
    mainWindow->addView(orderView);
    mainWindow->addView(deliveryListView);
    connectButtons();
}

void ReceivingWorkerController::start() {
    QSqlQuery query(db);
    query.prepare("SELECT first_name, last_name, job, vehicle_id FROM view_employees_info WHERE employee_id = ?");
    query.addBindValue(employeeId);
    if (query.exec() && query.next()) {
        mainMenuView->setEmployeeInfo(employeeId, query.value("first_name").toString(), query.value("last_name").toString(), query.value("job").toString());
        QVariant vehicleVar = query.value("vehicle_id");
        if (vehicleVar.isNull()) {
            mainMenuView->setAssignedVehicle("Brak przypisanego pojazdu");
            mainMenuView->showAssignVehicleButton(true);
            mainMenuView->showFreeVehicleButton(false);
        } else {
            mainMenuView->setAssignedVehicle(vehicleVar.toString());
            mainMenuView->showAssignVehicleButton(false);
            mainMenuView->showFreeVehicleButton(true);
        }
    }

//    query.prepare("SELECT COUNT(*) FROM orders WHERE assigned_employee_id = ? AND status = 'realizacja'");
//    query.addBindValue(employeeId);
//    if (query.exec() && query.next()) {
//        int activeOrders = query.value(0).toInt();
//        if (activeOrders > 0) {
//            //mainMenuView->showNewOrderButton(false);
//            //mainMenuView->showSubmitOrderButton(true);
//            //mainMenuView->showViewOrderButton(true);
//        } else {
//            //mainMenuView->showNewOrderButton(true);
//            //mainMenuView->showSubmitOrderButton(false);
//            //mainMenuView->showViewOrderButton(false);
//        }
//    }

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
    connect(deliveryListView, &ReceivingWorkerDeliveryListView::fillDelivery, this, &ReceivingWorkerController::fillDelivery);



}

void ReceivingWorkerController::handleLogout() {
    emit logoutRequest();
}

void ReceivingWorkerController::handleNewDelivery() {
    QSqlQuery query(db);
    query.prepare("INSERT INTO deliveries (employee_id) VALUES (?)");
    query.addBindValue(employeeId);
    if (!query.exec()) {
        mainMenuView->viewError("Błąd przy tworzeniu nowej dostawy: " + query.lastError().text());
        return;
    }
    handleDeliveries(); //View all deliveries
}


void ReceivingWorkerController::handleDeliveries() {
    deliveryListView->clearDeliveries();

    QSqlQuery query(db);
    query.prepare("SELECT delivery_id, delivery_date FROM deliveries ORDER BY delivery_id DESC");
    if (!query.exec()) {
        mainMenuView->viewError("Błąd przy pobieraniu dostaw");
        return;
    }

    while (query.next()) {
        int deliveryId = query.value("delivery_id").toInt();
        QDateTime deliveryDate = query.value("delivery_date").toDateTime();
        deliveryListView->addDeliveryInfo(deliveryId, deliveryDate);
    }

    mainWindow->showView(deliveryListView);
}

void ReceivingWorkerController::handleBackButton() {
    start();
}

void ReceivingWorkerController::handleAssignVehicle() {
    QSqlQuery query(db);
    query.prepare("UPDATE vehicles SET employee_id = ?, status = 'zajety' WHERE status = 'wolny' and employee_id is NULL LIMIT 1");
    query.addBindValue(employeeId);

    if (!query.exec()) {
        mainMenuView->viewError("Błąd przy przypisywaniu pojazdu");
        return;
    }
    start(); // refresh main menu gui
}


void ReceivingWorkerController::handleFreeVehicle() {
    QSqlQuery query(db);
    query.prepare("UPDATE vehicles SET status = 'wolny', employee_id = NULL WHERE employee_id = ?");
    query.addBindValue(employeeId);
    if (!query.exec()) {
        mainMenuView->viewError("Błąd przy zwalnianiu pojazdu");
        return;
    }
    start(); // refresh main menu gui
}

void ReceivingWorkerController::handleVerifyOrders() {
    orderListView->clearOrders();   // clear old orders

    //Get orders marked as finished ("gotowe")
    QSqlQuery query(db);
    query.prepare("SELECT order_id, creation_date, assigned_employee_name FROM view_orders WHERE status = 'gotowe'");
    if (!query.exec()) {
        mainMenuView->viewError("Błąd przy pobieraniu listy zamówień");
        return;
    }

    //Add each order to orderListView
    while (query.next()) {
        int orderId = query.value("order_id").toInt();
        QString employeeName = query.value("assigned_employee_name").toString();
        QDateTime creationDate = query.value("creation_date").toDateTime();
        orderListView->addOrderInfo(orderId, employeeName, creationDate);
    }

    mainWindow->showView(orderListView);
}

void ReceivingWorkerController::handleConfirmOrder(int orderId) {
    QSqlQuery query(db);
    query.prepare("UPDATE orders SET status = 'wyslane' WHERE order_id = ?");
    query.addBindValue(orderId);
    if (!query.exec()) {
        mainMenuView->viewError("Błąd przy potwierdzaniu zamówienia");
        return;
    }
    handleVerifyOrders(); //Refresh
}

void ReceivingWorkerController::handleCheckOrder(int orderId) {
    QSqlQuery query(db);
    query.prepare("SELECT * FROM view_orders WHERE order_id = ? AND status = \"gotowe\"");
    query.addBindValue(orderId);

    if (!query.exec()) {
        mainMenuView->viewError("Błąd przy pobieraniu zamówienia");
        return;
    } else if (query.exec() && !query.next()) {
        QString message = "Brak zamowienia";
        mainMenuView->viewError(message);
        return;
    }

    // Order information
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

void ReceivingWorkerController::handleOrderUpdate(int orderId, const QMap<int,int>& pickedMap) {
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

    handleVerifyOrders(); // back to viewing all orders
}

void ReceivingWorkerController::fillDelivery(int orderId) {
    //TODO DOKONCZYC
    qDebug()<<"Chce uzupelnic "<<orderId;

}
