//
// Created by jakub on 2025-12-07.
//

#ifndef MAGAZYN_RECEIVINGWORKERMAINMENUVIEW_H
#define MAGAZYN_RECEIVINGWORKERMAINMENUVIEW_H


#include <QWidget>
#include <QLabel>
#include <QPushButton>

class ReceivingWorkerMainMenuView : public QWidget {
    Q_OBJECT
private:
    QLabel* employeeIdLabel;
    QLabel* nameLabel;
    QLabel* jobLabel;
    QLabel* vehicleLabel;

    QPushButton* newDeliveryButton;
    QPushButton* viewDeliveryButton;
    QPushButton* logoutButton;
    QPushButton* assignVehicleButton;
    QPushButton* freeVehicleButton;
    QPushButton* verifyOrdersButton;

    void setUp();

    // Button handlers
    void handleLogout();
    void handleNewDelivery();
    void handleViewDelivery();
    void handleAssignVehicle();
    void handleFreeVehicle();
    void handleVerifyOrders();

public:
    ReceivingWorkerMainMenuView(QWidget* parent = nullptr);
    void setEmployeeInfo(int employeeId, const QString& firstName, const QString& lastName, const QString& job);
    void setAssignedVehicle(const QString& vehicleId);
    void viewError(const QString& message);

    //Buttons show modifiers
    void showAssignVehicleButton(bool show);
    void showFreeVehicleButton(bool show);
    void showViewOrderButton(bool show);
    void showNewOrderButton(bool show);
    void showVerifyOrders(bool show);

    signals:
    void viewDelivery();
    void getNewDelivery();
    void logoutRequest();
    void assignVehicle();
    void freeVehicle();
    void verifyOrders();
};


#endif //MAGAZYN_RECEIVINGWORKERMAINMENUVIEW_H
