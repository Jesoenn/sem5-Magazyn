//
// Created by jakub on 2026-01-10.
//

#ifndef MAGAZYN_RECEIVINGWORKERDATABASE_H
#define MAGAZYN_RECEIVINGWORKERDATABASE_H
#include <iostream>
#include <vector>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDateTime>

class ReceivingWorkerDatabase {
private:
    QSqlDatabase db;
public:
    ReceivingWorkerDatabase(QSqlDatabase db);

    std::vector<QString> getEmployeeInfo(int employeeId);
    void newDelivery(int employeeId);
    std::vector<std::vector<QString>> getDeliveries(int employeeId);
    void assignVehicle(int employeeId);
    void freeVehicle(int employeeId);
    std::vector<std::vector<QString>> verifyOrders();
    void confirmOrder(int orderId);
    std::vector<QString> getOrderInfo(int orderId);
    std::vector<std::vector<QString>> getOrderItems(int orderId);
    void updateOrder(int orderId, int itemId, int quantity);
    QString getDeliveryDate(int deliveryId);
    std::vector<std::vector<QString>> getDeliveryItems(int deliveryId);
    std::vector<std::vector<QString>> getWarehouseItems();
    void removeItemFromDelivery(int deliveryId, int itemId);
    void addItemToDelivery(int deliveryId, int itemId);
    void submitDeliveryItem(int deliveryId, int itemId, int quantity);
};


#endif //MAGAZYN_RECEIVINGWORKERDATABASE_H
