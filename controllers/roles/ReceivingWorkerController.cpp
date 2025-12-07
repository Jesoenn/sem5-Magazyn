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
    deliveryView = new ReceivingWorkerDeliveryView();

    mainWindow->addView(mainMenuView);
    mainWindow->addView(orderListView);
    mainWindow->addView(orderView);
    mainWindow->addView(deliveryListView);
    mainWindow->addView(deliveryView);
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
    query.prepare("SELECT delivery_id, delivery_date FROM deliveries WHERE employee_id = ? ORDER BY delivery_id DESC");
    query.addBindValue(employeeId);
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

void ReceivingWorkerController::handleFillDelivery(int deliveryId) {
    deliveryView->clearDeliveryItems();
    deliveryView->setDeliveryId(deliveryId);

    // Get delivery date
    QSqlQuery deliveryQuery(db);
    deliveryQuery.prepare("SELECT delivery_date FROM deliveries WHERE delivery_id = ?");
    deliveryQuery.addBindValue(deliveryId);
    if (!deliveryQuery.exec() || !deliveryQuery.next()) {
        mainMenuView->viewError("Nie udało się pobrać informacji o dostawie");
        return;
    }

    QDateTime deliveryDate = deliveryQuery.value("delivery_date").toDateTime();
    deliveryView->setDeliveryInfo(deliveryId, deliveryDate);

    // Get delivery items
    QSqlQuery itemsQuery(db);
    itemsQuery.prepare("SELECT item_id, item_name, quantity FROM view_delivery_items WHERE delivery_id = ?");
    itemsQuery.addBindValue(deliveryId);
    if (!itemsQuery.exec()) {
        mainMenuView->viewError("Nie udało się pobrać przedmiotów dostawy");
        return;
    }
    QSet<int> deliveryItemIds; // Items id in this delivery

    // Add each delivery item to view
    while (itemsQuery.next()) {
        int itemId = itemsQuery.value("item_id").toInt();
        QString itemName = itemsQuery.value("item_name").toString();
        int quantity = itemsQuery.value("quantity").toInt();

        deliveryView->addDeliveryItem(itemId, itemName, quantity);
        deliveryItemIds.insert(itemId);
    }

    // Get all items in warehouse
    QSqlQuery allItemsQuery(db);
    allItemsQuery.prepare("SELECT item_id, name FROM items ORDER BY name ASC");
    if (!allItemsQuery.exec()) {
        mainMenuView->viewError("Nie udało się pobrać listy wszystkich przedmiotów");
        return;
    }

    while (allItemsQuery.next()) {
        int itemId = allItemsQuery.value("item_id").toInt();
        QString itemName = allItemsQuery.value("name").toString();

        if (!deliveryItemIds.contains(itemId)) {
            deliveryView->addAvailableItem(itemId, itemName);  // <--- pojedynczo
        }
    }
    mainWindow->showView(deliveryView);
}

void ReceivingWorkerController::handleRemoveItem(int deliveryId, int itemId)
{
    QSqlQuery query(db);
    query.prepare("DELETE FROM delivery_items WHERE delivery_id = ? AND item_id = ?");
    query.addBindValue(deliveryId);
    query.addBindValue(itemId);

    if (!query.exec()) {
        mainMenuView->viewError("Nie udało się usunąć przedmiotu");
        return;
    }

    handleFillDelivery(deliveryId);
}

void ReceivingWorkerController::handleAddItem(int deliveryId, int itemId) {
    QSqlQuery query(db);
    query.prepare("INSERT INTO delivery_items (delivery_id, item_id, quantity) VALUES (?, ?, 0)");
    query.addBindValue(deliveryId);
    query.addBindValue(itemId);

    if (!query.exec()) {
        mainMenuView->viewError("Nie udało się dodać przedmiotu");
        return;
    }
    handleFillDelivery(deliveryId);
}

void ReceivingWorkerController::handleSubmitDelivery(int deliveryId, const QMap<int, int> &quantities) {
    QSqlQuery query(db);
    for (auto it = quantities.begin(); it != quantities.end(); ++it) {
        int itemId = it.key();
        int quantity = it.value();
        query.prepare("UPDATE delivery_items SET quantity = ? WHERE delivery_id = ? AND item_id = ?");
        query.addBindValue(quantity);
        query.addBindValue(deliveryId);
        query.addBindValue(itemId);
        if (!query.exec()) {
            mainMenuView->viewError("Błąd aktualizacji item_id " + QString::number(itemId));
            return;
        }
    }
}
