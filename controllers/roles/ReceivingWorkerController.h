//
// Created by jakub on 2025-12-05.
//

#ifndef MAGAZYN_RECEIVINGWORKERCONTROLLER_H
#define MAGAZYN_RECEIVINGWORKERCONTROLLER_H


#include <QObject>
#include <QSqlDatabase>
#include "../../widgets/MainWindow.h"
#include "../../widgets/receivingworker/ReceivingWorkerMainMenuView.h"
#include "../../widgets/receivingworker/ReceivingWorkerOrderListView.h"
#include "../../widgets/warehouseworker/WarehouseWorkerOrderView.h"
#include "../../widgets/receivingworker/ReceivingWorkerDeliveryListView.h"
#include "../../widgets/receivingworker/ReceivingWorkerDeliveryView.h"
#include "../databases/ReceivingWorkerDatabase.h"

class ReceivingWorkerController: public QObject{
    Q_OBJECT
private:
    int employeeId, jobId;
    ReceivingWorkerDatabase& receivingWorkerDb;
    MainWindow* mainWindow;

    ReceivingWorkerMainMenuView* mainMenuView;
    ReceivingWorkerOrderListView* orderListView;
    WarehouseWorkerOrderView* orderView;
    ReceivingWorkerDeliveryListView* deliveryListView;
    ReceivingWorkerDeliveryView* deliveryView;

    void connectButtons();
public:
    ReceivingWorkerController(MainWindow* mainWindow, ReceivingWorkerDatabase& receivingWorkerDb, int employeeId, int jobId);
    void start();

signals:
    void logoutRequest();   // Logout signal emitted to AppController

private slots:
    //MainMenuView
    void handleLogout();
    void handleNewDelivery();
    void handleDeliveries();
    void handleAssignVehicle();
    void handleFreeVehicle();
    void handleVerifyOrders();

    //OrderListView
    void handleConfirmOrder(int orderId);
    void handleCheckOrder(int orderId);

    //OrderView
    void handleOrderUpdate(int orderId, const QMap<int,int>& pickedMap);

    //DeliveryListView
    void handleFillDelivery(int deliveryId);

    //DeliveryView
    void handleRemoveItem(int deliveryId, int itemId);
    void handleAddItem(int deliveryId, int itemId);
    void handleSubmitDelivery(int deliveryId, const QMap<int,int>& quantities);

    //All
    void handleBackButton();

};


#endif //MAGAZYN_RECEIVINGWORKERCONTROLLER_H
