//
// Created by jakub on 2025-12-07.
//

#ifndef MAGAZYN_RECEIVINGWORKERDELIVERYVIEW_H
#define MAGAZYN_RECEIVINGWORKERDELIVERYVIEW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QLabel>
#include <QHBoxLayout>
#include <QMap>
#include <QLineEdit>
#include <QDateTime>
#include <QComboBox>

class ReceivingWorkerDeliveryView : public QWidget {
Q_OBJECT
private:
    struct DeliveryItemRow {
        QLabel* itemIdLabel;
        QLabel* itemNameLabel;
        QLineEdit* quantityLineEdit;
        QPushButton* removeButton;
    };

    int deliveryId = -1;
    QVBoxLayout* mainLayout;
    QLabel* deliveryInfoLabel;
    QPushButton* backButton;
    QPushButton* submitButton;

    QScrollArea* scrollArea;
    QWidget* scrollWidget;
    QVBoxLayout* itemsLayout;

    QMap<int, DeliveryItemRow> itemRows; // item_id -> row widgets

    QComboBox* itemComboBox;
    QPushButton* addItemButton;

    // Prywatne funkcje
    void setUpHeader();
    void setUpScrollArea();

public:
    explicit ReceivingWorkerDeliveryView(QWidget* parent = nullptr);

    void setDeliveryInfo(int deliveryId, const QDateTime& creationDate);
    void addDeliveryItem(int itemId, const QString& itemName, int quantity);
    void clearDeliveryItems();
    void addAvailableItem(int itemId, const QString& name);
    void setDeliveryId(int id);
    int getDeliveryId();

signals:
    void backToMainMenu();
    void removeItem(int deliveryId, int itemId);
    void addItem(int deliveryId, int itemId);
    void submitDelivery(int deliveryId, const QMap<int,int>& quantities);

private slots:
    void handleBackButton();
    void handleAddItem();
    void handleSubmitDelivery();
};

#endif //MAGAZYN_RECEIVINGWORKERDELIVERYVIEW_H
