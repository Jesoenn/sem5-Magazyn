//
// Created by jakub on 2025-12-06.
//

#ifndef MAGAZYN_WAREHOUSEWORKERMAINMENUVIEW_H
#define MAGAZYN_WAREHOUSEWORKERMAINMENUVIEW_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

class WarehouseWorkerMainMenuView : public QWidget {
Q_OBJECT
private:
    QLabel* employeeIdLabel;
    QLabel* nameLabel;
    QLabel* jobLabel;
    QLabel* vehicleLabel;

    QPushButton* newOrderButton;
    QPushButton* viewOrderButton;
    QPushButton* logoutButton;
    QPushButton* assignVehicleButton;
    QPushButton* submitOrderButton;
    QPushButton* freeVehicleButton;

    void setUp();

    // Button handlers
    void handleLogout();
    void handleNewOrder();
    void handleViewOrder();
    void handleAssignVehicle();
    void handleSubmitOrder();
    void handleFreeVehicle();

public:
    WarehouseWorkerMainMenuView(QWidget* parent = nullptr);
    void setEmployeeInfo(int employeeId, const QString& firstName, const QString& lastName, const QString& job);
    void setAssignedVehicle(const QString& vehicleId);
    void viewError(const QString& message);

    //Buttons show modifiers
    void showAssignVehicleButton(bool show);
    void showFreeVehicleButton(bool show);
    void showViewOrderButton(bool show);
    void showSubmitOrderButton(bool show);
    void showNewOrderButton(bool show);

signals:
    void viewOrder();
    void getNewOrder();
    void logoutRequest();
    void assignVehicle();
    void freeVehicle();
    void submitOrder();
};


#endif //MAGAZYN_WAREHOUSEWORKERMAINMENUVIEW_H
