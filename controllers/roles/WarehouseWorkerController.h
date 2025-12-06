//
// Created by jakub on 2025-12-05.
//

#ifndef MAGAZYN_WAREHOUSEWORKERCONTROLLER_H
#define MAGAZYN_WAREHOUSEWORKERCONTROLLER_H


#include <QSqlDatabase>
#include "../../widgets/MainWindow.h"

class WarehouseWorkerController: public QObject {
private:
    int employeeId, jobId;
    QSqlDatabase& db;
    MainWindow* mainWindow;

public:
    WarehouseWorkerController(MainWindow* mainWindow, QSqlDatabase& db, int employeeId, int jobId);
    void start();

private slots:


};


#endif //MAGAZYN_WAREHOUSEWORKERCONTROLLER_H
