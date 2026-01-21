//
// Created by jakub on 2026-01-21.
//

#include "AdminDatabase.h"

AdminDatabase::AdminDatabase(QSqlDatabase db): db(db) {}

std::vector<QString> AdminDatabase::getEmployeeInfo(int employeeId) {
    QSqlQuery query(db);
    query.prepare("SELECT first_name, last_name, job, vehicle_id FROM view_employees_info WHERE employee_id = ?");
    query.addBindValue(employeeId);

    std::vector<QString> employeeInfo;

    if (query.exec() && query.next()) {
        employeeInfo.push_back(QString::number(employeeId));
        employeeInfo.push_back(query.value("first_name").toString());
        employeeInfo.push_back(query.value("last_name").toString());
        employeeInfo.push_back(query.value("job").toString());
    } else {
        for (int i = 0; i < 5; i++) {
            employeeInfo.push_back("Unknown");
        }
    }
    return employeeInfo;
}

std::vector<std::vector<QString>> AdminDatabase::getJobs() {
    QSqlQuery query(db);
    query.prepare("SELECT job_id, job_name FROM jobs");
    if (!query.exec()) {
        throw std::runtime_error("Błąd przy pobieraniu listy stanowisk");
    }

    std::vector<std::vector<QString>> jobs;
    while (query.next()) {
        std::vector<QString> job;
        job.push_back(query.value("job_id").toString());
        job.push_back(query.value("job_name").toString());
        jobs.push_back(job);
    }
    return jobs;
}

std::vector<std::vector<QString>> AdminDatabase::getEmployees() {
    QSqlQuery query(db);
    query.prepare("SELECT employee_id, login, first_name, last_name, job_id, job, employed FROM view_employees");
    if (!query.exec()) {
        throw std::runtime_error("Błąd przy pobieraniu listy pracowników");
    }

    std::vector<std::vector<QString>> employees;
    while (query.next()) {
        std::vector<QString> employee;
        employee.push_back(query.value("employee_id").toString());
        employee.push_back(query.value("login").toString());
        employee.push_back(query.value("first_name").toString());
        employee.push_back(query.value("last_name").toString());
        employee.push_back(query.value("job_id").toString());
        employee.push_back(query.value("job").toString());
        employee.push_back(query.value("employed").toString());
        employees.push_back(employee);
    }
    return employees;
}