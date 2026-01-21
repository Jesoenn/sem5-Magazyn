//
// Created by jakub on 2025-12-05.
//

#ifndef MAGAZYN_ADMINCONTROLLER_H
#define MAGAZYN_ADMINCONTROLLER_H


#include <QObject>
#include <QSqlDatabase>
#include "../../widgets/MainWindow.h"
#include "../databases/AdminDatabase.h"
#include "../../widgets/admin/AdminMainMenuView.h"
#include "WarehouseWorkerController.h"
#include "ReceivingWorkerController.h"
#include "ManagerController.h"
#include "../databases/WarehouseWorkerDatabase.h"
#include "../databases/ReceivingWorkerDatabase.h"
#include "../databases/ManagerDatabase.h"

class AdminController: public QObject{
    Q_OBJECT

private:
    int employeeId, jobId;
    QSqlDatabase& db;
    AdminDatabase& adminDb;
    MainWindow* mainWindow;

    AdminMainMenuView* mainMenuView;
    ManagerEmployeesView* employeesView;

    //Jobs
    WarehouseWorkerController* warehouseWorkerController;
    ReceivingWorkerController* receivingWorkerController;
    ManagerController* managerController;


    WarehouseWorkerDatabase* warehouseWorkerDB;
    ReceivingWorkerDatabase* receivingWorkerDB;
    ManagerDatabase* managerDB;

    void connectButtons();
public:
    ~AdminController();
    AdminController(MainWindow* mainWindow, QSqlDatabase& db, AdminDatabase& managerDb, int employeeId, int jobId);
    void start();

signals:
    void logoutRequest();   // Logout signal emitted to AppController

private slots:
    void handleBackFromOtherController();
    void handleBackButton();

    //MainMenuView
    void handleLogout();
    void handleViewEmployees();
    void handleViewEventLog();
    void handleViewManagerView();
    void handleViewReceivingWorkerView();
    void handleViewWarehouseWorkerView();

    //EmployeesView
    void handleFireEmployee(int employeeId);
    void handleModifyEmployee(int employeeId, const QString& login, const QString& firstName, const QString& lastName, bool employed);
    void handleAddEmployee(const QString& login, const QString& firstName, const QString& lastName, int jobId, const QString& password);

//
//    //VehiclesView
//    void handleSaveVehicle(int vehicleId, const QString& status, int employeeId);
//
//    //EmployeesView
//    void handleFireEmployee(int employeeId);
//    void handleModifyEmployee(int employeeId, const QString& login, const QString& firstName, const QString& lastName, bool employed);
//    void handleAddEmployee(const QString& login, const QString& firstName, const QString& lastName, int jobId, const QString& password);
//
//    //OrdersListView
//    void handleAssignEmployee(int orderId, int employeeId);
//    void handleModifyOrder(int orderId);
//    void handleDeleteOrder(int orderId);
//    void handleCreateOrder(int employeeId);
//
//    //OrderView
//    void handleModifyOrderItem(int orderId, int orderItemId, int newQuantity);
//    void handleDeleteOrderItem(int orderId, int orderItemId);
//    void handleAddOrderItem(int orderId, int itemId, int quantity);
//
//    //All
//    void handleBackButton();
};


#endif //MAGAZYN_ADMINCONTROLLER_H
