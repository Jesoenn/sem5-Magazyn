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

class ReceivingWorkerController: public QObject{
    Q_OBJECT
private:
    int employeeId, jobId;
    QSqlDatabase& db;
    MainWindow* mainWindow;

    ReceivingWorkerMainMenuView* mainMenuView;
    ReceivingWorkerOrderListView* orderListView;
    WarehouseWorkerOrderView* orderView;

    void connectButtons();
public:
    ReceivingWorkerController(MainWindow* mainWindow, QSqlDatabase& db, int employeeId, int jobId);
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

    //All
    void handleBackButton();

};


#endif //MAGAZYN_RECEIVINGWORKERCONTROLLER_H
