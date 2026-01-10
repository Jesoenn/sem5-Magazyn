//
// Created by jakub on 2025-12-08.
//

#ifndef MANAGERORDERSLISTVIEW_H
#define MANAGERORDERSLISTVIEW_H

#include <QWidget>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QMap>

class ManagerOrdersListView : public QWidget {
Q_OBJECT

private:
    QPushButton* backButton;
    QLabel* headerLabel;
    QVBoxLayout* mainLayout;

    QScrollArea* scrollArea;
    QWidget* scrollWidget;
    QVBoxLayout* ordersLayout;

    // Sekcja tworzenia zamówienia
    QWidget* createOrderWidget;
    QComboBox* createEmployeeCombo;
    QPushButton* createOrderButton;

    struct OrderRow {
        QLabel* idLabel;
        QComboBox* assignedEmployeeCombo;
        QLabel* creatorLabel;
        QLabel* dateLabel;
        QPushButton* modifyButton;
        QPushButton* deleteButton;
    };

    QMap<int, OrderRow> orderRows; // order_id -> row
    QMap<int, QString> employeeMap; // employee_id -> "Imię Nazwisko"

    void setUpHeader();
    void setUpScrollArea();
    void addColumnLabels();
    void setUpCreateOrderWidget();

public:
    explicit ManagerOrdersListView(QWidget* parent = nullptr);

    void setEmployeeMap(const QMap<int, QString>& map);
    void addOrderRow(int orderId, int assignedEmployeeId, const QString& creatorName, const QString& createdAt);
    void clearOrders();

signals:
    void backToMainMenu();
    void assignEmployee(int orderId, int employeeId);
    void modifyOrder(int orderId);
    void deleteOrder(int orderId);
    void createOrder(int employeeId);

private slots:
    void handleBackButton();
};

#endif
