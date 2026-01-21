#ifndef MAGAZYN_ADMINMAINMENUVIEW_H
#define MAGAZYN_ADMINMAINMENUVIEW_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>

class AdminMainMenuView : public QWidget {
    Q_OBJECT
private:
    QLabel* employeeIdLabel;
    QLabel* nameLabel;
    QLabel* jobLabel;

    QPushButton* viewEmployeesButton;
    QPushButton* viewEventLogButton;
    QPushButton* logoutButton;
    QPushButton* viewManagerViewButton;
    QPushButton* viewReceivingWorkerViewButton;
    QPushButton* viewWarehouseWorkerViewButton;

    void setUp();

    // Button handlers
    void handleViewEmployees();
    void handleViewEventLog();
    void handleLogout();
    void handleViewManagerView();
    void handleViewReceivingWorkerView();
    void handleViewWarehouseWorkerView();

public:
    explicit AdminMainMenuView(QWidget* parent = nullptr);
    void setEmployeeInfo(int employeeId, const QString& firstName, const QString& lastName, const QString& job);
    void viewError(const QString& message);

    signals:
    void viewEmployees();
    void viewEventLog();
    void logoutRequest();
    void viewManagerView();
    void viewReceivingWorkerView();
    void viewWarehouseWorkerView();
};

#endif //MAGAZYN_ADMINMAINMENUVIEW_H
