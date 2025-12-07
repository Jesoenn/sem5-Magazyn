//
// Created by jakub on 2025-12-07.
//

#ifndef MAGAZYN_RECEIVINGWORKERDELIVERYLISTVIEW_H
#define MAGAZYN_RECEIVINGWORKERDELIVERYLISTVIEW_H



#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QLabel>
#include <QMap>
#include <QDateTime>

class ReceivingWorkerDeliveryListView : public QWidget {
Q_OBJECT
private:
    struct DeliveryRowWidgets {
        QLabel* deliveryIdLabel;
        QLabel* dateLabel;
        QPushButton* fillButton;
    };

    QVBoxLayout* mainLayout;
    QPushButton* backButton;
    QVBoxLayout* deliveriesLayout;

    QScrollArea* scrollArea;
    QWidget* scrollWidget;

    QMap<int, DeliveryRowWidgets> deliveryRows; // delivery_id -> row widgets

    void setUpScrollArea();
    void setUpHeader();

public:
    ReceivingWorkerDeliveryListView(QWidget* parent = nullptr);

    void addDeliveryInfo(int deliveryId, const QDateTime& deliveryDate);
    void clearDeliveries();

signals:
    void backToMainMenu();
    void fillDelivery(int deliveryId);

private slots:
    void handleBackButton();
};

#endif //MAGAZYN_RECEIVINGWORKERDELIVERYLISTVIEW_H
