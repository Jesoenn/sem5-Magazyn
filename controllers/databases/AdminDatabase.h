//
// Created by jakub on 2026-01-21.
//

#ifndef MAGAZYN_ADMINDATABASE_H
#define MAGAZYN_ADMINDATABASE_H
#include <iostream>
#include <vector>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDateTime>

class AdminDatabase {
private:
    QSqlDatabase db;
public:
    explicit AdminDatabase(QSqlDatabase db);
    std::vector<QString> getEmployeeInfo(int employeeId);
    std::vector<std::vector<QString>> getJobs();
    std::vector<std::vector<QString>> getEmployees();

};


#endif //MAGAZYN_ADMINDATABASE_H
