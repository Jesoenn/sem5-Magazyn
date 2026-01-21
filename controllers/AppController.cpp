//
// Created by jakub on 2025-12-05.
//
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include "AppController.h"
AppController::AppController():
    warehouseWorkerController(nullptr), receivingWorkerController(nullptr), managerController(nullptr) {
    connectToDatabase();
    mainWindow = new MainWindow();
    mainWindow -> loadStyle();
    loginView = new LoginView();

    // Add views to mainWindow
    mainWindow->addView(loginView);

    // connect login signal
    connect(loginView, &LoginView::loginRequested,this, &AppController::handleLogin);
}

void AppController::start() {
    mainWindow->showView(loginView);
    mainWindow->show();
}

void AppController::handleLogin(QString user, QString pass) {
    QSqlQuery query;
    //TODO: hashowanie SHA256

    // ready SQL statement
    query.prepare("SELECT employee_id, employed, job_id FROM employees WHERE login = :user AND password = :pass");
    query.bindValue(":user", user);
    query.bindValue(":pass", pass);

    if (!query.exec()) {
        qDebug() << "ERROR:" << query.lastError().text();
        return;
    }

    if (query.next()) {
        int employeeId = query.value("employee_id").toInt();
        bool employed = query.value("employed").toBool();
        int jobId = query.value("job_id").toInt();

        if (jobId == 1 && employed){
            warehouseWorkerDB = new WarehouseWorkerDatabase(db);
            warehouseWorkerController = new WarehouseWorkerController(mainWindow, *warehouseWorkerDB, employeeId, jobId);
            warehouseWorkerController->start();
            //Logout signal
            connect(warehouseWorkerController, &WarehouseWorkerController::logoutRequest, this, &AppController::handleLogout);
        } else if (jobId == 2 && employed){
            receivingWorkerDB = new ReceivingWorkerDatabase(db);
            receivingWorkerController = new ReceivingWorkerController(mainWindow, *receivingWorkerDB, employeeId, jobId);
            receivingWorkerController->start();
            //Logout signal
            connect(receivingWorkerController, &ReceivingWorkerController::logoutRequest, this, &AppController::handleLogout);
        } else if (jobId == 3 && employed){
            managerDB = new ManagerDatabase(db);
            managerController = new ManagerController(mainWindow, db, *managerDB, employeeId, jobId);
            managerController->start();
            //Logout signal
            connect(managerController, &ManagerController::logoutRequest, this, &AppController::handleLogout);
        } else if (jobId == 4 && employed){
            adminDB = new AdminDatabase(db);
            adminController = new AdminController(mainWindow, db, *adminDB, employeeId, jobId);
            adminController->start();
            //Logout signal
            connect(adminController, &AdminController::logoutRequest, this, &AppController::handleLogout);
        } else {
            loginView->loginFailed();
        }

    } else {
        loginView->loginFailed();
    }
}

void AppController::connectToDatabase() {
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setPort(3306);
    db.setUserName("root");
    db.setPassword("root");
    db.setDatabaseName("magazyn");

    if (!db.open()) {
        throw std::invalid_argument("Blad polaczenia z baza danych");
    }

    qDebug() << "Polaczono baze danych";
}

AppController::~AppController() {
    delete warehouseWorkerController;
    delete receivingWorkerController;
    delete managerController;
    delete adminController;
}

void AppController::handleLogout() {
    if(warehouseWorkerController != nullptr){
        delete warehouseWorkerController;
        warehouseWorkerController=nullptr;
    }
    else if (receivingWorkerController != nullptr){
        delete receivingWorkerController;
        receivingWorkerController=nullptr;
    }
    else if (managerController != nullptr){
        delete managerController;
        managerController = nullptr;
    }
    else if (adminController != nullptr){
        delete adminController;
        adminController = nullptr;
    }

    mainWindow->showView(loginView);
}
