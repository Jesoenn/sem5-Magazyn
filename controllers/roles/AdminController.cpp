//
// Created by jakub on 2025-12-05.
//

#include "AdminController.h"

//TODO: EVENLOG MODYFIKACJA + weryfikacja czy wszystko dziala

AdminController::AdminController(MainWindow* mainWindow, QSqlDatabase& db, AdminDatabase& adminDb, int employeeId, int jobId):
        mainWindow(mainWindow), db(db), adminDb(adminDb), employeeId(employeeId), jobId(jobId),
        warehouseWorkerController(nullptr), receivingWorkerController(nullptr), managerController(nullptr),
        warehouseWorkerDB(nullptr), receivingWorkerDB(nullptr), managerDB(nullptr) {
    //Setup views
    mainMenuView = new AdminMainMenuView();
    employeesView = new ManagerEmployeesView();

    mainWindow->addView(mainMenuView);
    mainWindow->addView(employeesView);
    connectButtons();
}

AdminController::~AdminController() {
    delete warehouseWorkerController;
    delete receivingWorkerController;
    delete managerController;
}

void AdminController::start() {
    std::vector<QString> employeeInfo = adminDb.getEmployeeInfo(employeeId);
    mainMenuView->setEmployeeInfo(employeeId, employeeInfo[1], employeeInfo[2], employeeInfo[3]);
    mainWindow->showView(mainMenuView);
}

void AdminController::connectButtons() {
    //MainView
    connect(mainMenuView, &AdminMainMenuView::logoutRequest, this, &AdminController::handleLogout); // pass signal to AppController
    connect(mainMenuView, &AdminMainMenuView::viewEmployees, this, &AdminController::handleViewEmployees);
    connect(mainMenuView, &AdminMainMenuView::viewEventLog, this, &AdminController::handleViewEventLog);
    connect(mainMenuView, &AdminMainMenuView::viewManagerView, this, &AdminController::handleViewManagerView);
    connect(mainMenuView, &AdminMainMenuView::viewReceivingWorkerView, this, &AdminController::handleViewReceivingWorkerView);
    connect(mainMenuView, &AdminMainMenuView::viewWarehouseWorkerView, this, &AdminController::handleViewWarehouseWorkerView);

    //EmployeesView
    connect(employeesView, &ManagerEmployeesView::backToMainMenu, this, &AdminController::handleBackButton);
    connect(employeesView, &ManagerEmployeesView::fireEmployee, this, &AdminController::handleFireEmployee);
    connect(employeesView, &ManagerEmployeesView::modifyEmployee, this, &AdminController::handleModifyEmployee);
    connect(employeesView, &ManagerEmployeesView::addEmployee, this, &AdminController::handleAddEmployee);
}

void AdminController::handleLogout() {
    mainWindow->removeView(mainMenuView);
    mainWindow->removeView(employeesView);

    delete mainMenuView;
    delete employeesView;

    mainMenuView = nullptr;
    employeesView = nullptr;

    emit logoutRequest();
}

void AdminController::handleViewEmployees() {
    employeesView->clearEmployees();
    employeesView->setAdmin(true);
    QMap<int, QString> jobMap;

    try{
        std::vector<std::vector<QString>> jobs = adminDb.getJobs();
        for (const auto& job : jobs) {
            int id = job[0].toInt();
            QString name = job[1];
            jobMap[id] = name;
        }
        employeesView->setJobMap(jobMap);
    } catch (const std::runtime_error& e) {
        mainMenuView->viewError(QString::fromStdString(e.what()));
        return;
    }

    try{
        std::vector<std::vector<QString>> employees = adminDb.getEmployees();
        for (const auto& emp : employees) {
            int employeeId = emp[0].toInt();
            QString login = emp[1];
            QString firstName = emp[2];
            QString lastName = emp[3];
            int jobId = emp[4].toInt();
            QString jobName = emp[5];
            bool employed = (emp[6] == "1" || emp[6].toLower() == "true");

            employeesView->addEmployeeRow(employeeId, login, firstName, lastName, jobId, jobName, employed);
        }
    } catch (const std::runtime_error& e) {
        mainMenuView->viewError(QString::fromStdString(e.what()));
        return;
    }

    mainWindow->showView(employeesView);
}

void AdminController::handleViewEventLog() {
    mainMenuView->viewError("handleViewEventLog");
}

void AdminController::handleViewManagerView() {
    managerDB = new ManagerDatabase(db);
    managerController = new ManagerController(mainWindow, db, *managerDB, employeeId, jobId);
    managerController->start();
    //Logout signal
    connect(managerController, &ManagerController::logoutRequest, this, &AdminController::handleBackFromOtherController);
}

void AdminController::handleViewReceivingWorkerView() {
    receivingWorkerDB = new ReceivingWorkerDatabase(db);
    receivingWorkerController = new ReceivingWorkerController(mainWindow, *receivingWorkerDB, employeeId, jobId);
    receivingWorkerController->start();
    //Logout signal
    connect(receivingWorkerController, &ReceivingWorkerController::logoutRequest, this, &AdminController::handleBackFromOtherController);
}

void AdminController::handleViewWarehouseWorkerView() {
    warehouseWorkerDB = new WarehouseWorkerDatabase(db);
    warehouseWorkerController = new WarehouseWorkerController(mainWindow, *warehouseWorkerDB, employeeId, jobId);
    warehouseWorkerController->start();
    //Logout signal
    connect(warehouseWorkerController, &WarehouseWorkerController::logoutRequest, this, &AdminController::handleBackFromOtherController);
}

void AdminController::handleBackFromOtherController() {
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

    start();
}

void AdminController::handleFireEmployee(int employeeId) {

}

void AdminController::handleModifyEmployee(int employeeId, const QString &login, const QString &firstName,const QString &lastName, bool employed) {

}

void
AdminController::handleAddEmployee(const QString &login, const QString &firstName, const QString &lastName, int jobId, const QString &password) {

}

void AdminController::handleBackButton() {
    start();
}


