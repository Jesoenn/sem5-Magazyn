//
// Created by jakub on 2025-12-05.
//

#ifndef MAGAZYN_WAREHOUSEWORKERCONTROLLER_H
#define MAGAZYN_WAREHOUSEWORKERCONTROLLER_H


#include <QSqlDatabase>
#include "../../widgets/MainWindow.h"
#include "../../widgets/warehouseworker/WarehouseWorkerMainMenuView.h"
#include "../../widgets/warehouseworker/WarehouseWorkerOrderView.h"
#include "../databases/WarehouseWorkerDatabase.h"

class WarehouseWorkerController: public QObject {
    Q_OBJECT
private:
    int employeeId, jobId;
    WarehouseWorkerDatabase& warehouseWorkerDb;
    MainWindow* mainWindow;

    WarehouseWorkerMainMenuView* mainMenuView;
    WarehouseWorkerOrderView* orderView;

    void connectButtons();



public:
    WarehouseWorkerController(MainWindow* mainWindow, WarehouseWorkerDatabase& warehouseWorkerDb, int employeeId, int jobId);
    void start();

signals:
    void logoutRequest();   // Logout signal emitted to AppController

private slots:
    void handleLogout();
    void handleNewOrder();
    void handleCurrentOrder();
    void handleBackButton();
    void handleOrderUpdate(int orderId, const QMap<int,int>& pickedMap);
    void handleSubmitNewOrder();
    void handleAssignVehicle();
    void handleFreeVehicle();

};


#endif //MAGAZYN_WAREHOUSEWORKERCONTROLLER_H
