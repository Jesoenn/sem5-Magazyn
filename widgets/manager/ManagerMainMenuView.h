//
// Created by jakub on 2025-12-07.
//

//
// Created by jakub on 2025-12-07.
//

#ifndef MAGAZYN_MANAGERMAINMENUVIEW_H
#define MAGAZYN_MANAGERMAINMENUVIEW_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>

class ManagerMainMenuView : public QWidget {
Q_OBJECT
private:
    QLabel* employeeIdLabel;
    QLabel* nameLabel;
    QLabel* jobLabel;

    QPushButton* viewEmployeesButton;
    QPushButton* viewVehiclesButton;
    QPushButton* viewOrdersButton;
    QPushButton* viewEventLogButton;
    QPushButton* logoutButton;

    void setUp();

    // Button handlers
    void handleViewEmployees();
    void handleViewVehicles();
    void handleViewOrders();
    void handleViewEventLog();
    void handleLogout();

public:
    explicit ManagerMainMenuView(QWidget* parent = nullptr);
    void setEmployeeInfo(int employeeId, const QString& firstName, const QString& lastName, const QString& job);
    void viewError(const QString& message);

signals:
    void viewEmployees();
    void viewVehicles();
    void viewOrders();
    void viewEventLog();
    void logoutRequest();
};

#endif // MAGAZYN_MANAGERMAINMENUVIEW_H

