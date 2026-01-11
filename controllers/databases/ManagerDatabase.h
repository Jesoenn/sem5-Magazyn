//
// Created by jakub on 2026-01-10.
//

#ifndef MAGAZYN_MANAGERDATABASE_H
#define MAGAZYN_MANAGERDATABASE_H
#include <iostream>
#include <vector>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDateTime>

class ManagerDatabase {
private:
    QSqlDatabase db;
public:
    ManagerDatabase(QSqlDatabase db);
    std::vector<QString> getEmployeeInfo(int employeeId);
    std::vector<std::vector<QString>> getJobs();
    std::vector<std::vector<QString>> getEmployees();
    std::vector<std::vector<QString>> getEmployeesIdToName();
    std::vector<std::vector<QString>> getOrdersInfo();
    std::vector<std::vector<QString>> getEmployeesCanHaveVehicle();
    std::vector<std::vector<QString>> getVehiclesInfo();
    std::vector<std::vector<QString>> getEventLog();
    bool isEmployeeAssignedToVehicle(int employeeId, int vehicleId);
    void assignVehicleToEmployee(int vehicleId, int employeeId);
    void fireEmployee(int employeeId);
    void modifyEmployee(int employeeId, const QString login, const QString firstName, const QString lastName, bool employed);
    void addEmployee(const QString &login, const QString &firstName, const QString &lastName, int jobId, const QString &password);
    void assignEmployeeToOrder(int employeeId, int orderId);
    void deleteOrder(int orderId);
    void createOrder(int creatorId, int assignedEmployeeId);
    void modifyOrderItem(int orderId, int orderItemId, int newQuantity);
    void deleteOrderItem(int orderId, int orderItemId);
    void returnAllPickedQuantities(int orderId, int orderItemId);
    void addOrderItem(int orderId, int itemId, int quantity);
    std::vector<QString> getOrderInfo(int orderId);
    std::vector<std::vector<QString>> getOrderItems(int orderId);
    std::vector<std::vector<QString>> getItems();

};


#endif //MAGAZYN_MANAGERDATABASE_H
