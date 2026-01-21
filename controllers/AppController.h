//
// Created by jakub on 2025-12-05.
//

#ifndef MAGAZYN_APPCONTROLLER_H
#define MAGAZYN_APPCONTROLLER_H


#include <QObject>
#include <QSqlDatabase>
#include "../widgets/MainWindow.h"
#include "../widgets/LoginView.h"
#include "roles/WarehouseWorkerController.h"
#include "roles/ReceivingWorkerController.h"
#include "roles/ManagerController.h"
#include "databases/ReceivingWorkerDatabase.h"
#include "roles/AdminController.h"

class AppController: public QObject {
private:
    QSqlDatabase db;
    MainWindow* mainWindow;
    LoginView* loginView;

    //Jobs
    WarehouseWorkerController* warehouseWorkerController;
    ReceivingWorkerController* receivingWorkerController;
    ManagerController* managerController;
    AdminController* adminController;

    WarehouseWorkerDatabase* warehouseWorkerDB;
    ReceivingWorkerDatabase* receivingWorkerDB;
    ManagerDatabase* managerDB;
    AdminDatabase* adminDB;

public:
    ~AppController();
    AppController();
    void start();
    void connectToDatabase();

private slots:
    void handleLogin(QString user, QString pass);
    void handleLogout();
};

#endif //MAGAZYN_APPCONTROLLER_H
