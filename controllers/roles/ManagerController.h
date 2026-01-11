//
// Created by jakub on 2025-12-05.
//

#ifndef MAGAZYN_MANAGERCONTROLLER_H
#define MAGAZYN_MANAGERCONTROLLER_H

#include <QObject>
#include <QSqlDatabase>
#include "../../widgets/MainWindow.h"
#include "../../widgets/manager/ManagerMainMenuView.h"
#include "../../widgets/manager/ManagerLogsView.h"
#include "../../widgets/manager/ManagerVehiclesView.h"
#include "../../widgets/manager/ManagerEmployeesView.h"
#include "../../widgets/manager/ManagerOrdersListView.h"
#include "../../widgets/manager/ManagerOrderView.h"
#include "../databases/ManagerDatabase.h"

class ManagerController: public QObject{
    Q_OBJECT
private:
    int employeeId, jobId;
    QSqlDatabase& db;
    ManagerDatabase& managerDb;
    MainWindow* mainWindow;

    ManagerMainMenuView* mainMenuView;
    ManagerLogsView* logsView;
    ManagerVehiclesView* vehiclesView;
    ManagerEmployeesView* employeesView;
    ManagerOrdersListView* ordersListView;
    ManagerOrderView* orderView;

    void connectButtons();
public:
    ManagerController(MainWindow* mainWindow, QSqlDatabase& db, ManagerDatabase& managerDb, int employeeId, int jobId);
    void start();

signals:
    void logoutRequest();   // Logout signal emitted to AppController

private slots:
    //MainMenuView
    void handleLogout();
    void handleViewEmployees();
    void handleViewVehicles();
    void handleViewOrders();
    void handleViewEventLog();

    //VehiclesView
    void handleSaveVehicle(int vehicleId, const QString& status, int employeeId);

    //EmployeesView
    void handleFireEmployee(int employeeId);
    void handleModifyEmployee(int employeeId, const QString& login, const QString& firstName, const QString& lastName, bool employed);
    void handleAddEmployee(const QString& login, const QString& firstName, const QString& lastName, int jobId, const QString& password);

    //OrdersListView
    void handleAssignEmployee(int orderId, int employeeId);
    void handleModifyOrder(int orderId);
    void handleDeleteOrder(int orderId);
    void handleCreateOrder(int employeeId);

    //OrderView
    void handleModifyOrderItem(int orderId, int orderItemId, int newQuantity);
    void handleDeleteOrderItem(int orderId, int orderItemId);
    void handleAddOrderItem(int orderId, int itemId, int quantity);

    //All
    void handleBackButton();

};


#endif //MAGAZYN_MANAGERCONTROLLER_H
