//
// Created by jakub on 2025-12-07.
//

#ifndef MAGAZYN_RECEIVINGWORKERORDERLISTVIEW_H
#define MAGAZYN_RECEIVINGWORKERORDERLISTVIEW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QLabel>
#include <QMap>

struct DeliveryRowWidgets {
    QLabel* orderIdLabel;
    QLabel* employeeLabel;
    QLabel* dateLabel;
    QPushButton* checkButton;
    QPushButton* confirmButton;
};

class ReceivingWorkerOrderListView : public QWidget {
    Q_OBJECT
private:
    QVBoxLayout* mainLayout;
    QPushButton* backButton;
    QVBoxLayout* ordersLayout;

    QScrollArea* scrollArea;
    QWidget* scrollWidget;
    QVBoxLayout* rowsLayout;

    QMap<int, DeliveryRowWidgets> orderRows;    // order_id -> DeliveryRowWidget

    void setUpScrollArea();
    void setUpHeader();

public:
    ReceivingWorkerOrderListView(QWidget* parent = nullptr);

    void addOrderInfo(int orderId, const QString& employeeName, const QDateTime& creationDate);
    void clearOrders();

signals:
    void backToMainMenu();
    void checkOrder(int orderId);
    void confirmOrder(int orderId);

private slots:
    void handleBackButton();
};


#endif //MAGAZYN_RECEIVINGWORKERORDERLISTVIEW_H
