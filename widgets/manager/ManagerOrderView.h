//
// Created by jakub on 2025-12-08.
//

#ifndef MAGAZYN_MANAGERORDERVIEW_H
#define MAGAZYN_MANAGERORDERVIEW_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QMap>

class ManagerOrderView : public QWidget {
Q_OBJECT
private:
    int orderId=-1;
    QPushButton* backButton;
    QLabel* orderInfoLabel;
    QVBoxLayout* mainLayout;

    QScrollArea* scrollArea;
    QWidget* scrollWidget;
    QVBoxLayout* itemsLayout;

    QWidget* addItemWidget;
    QComboBox* newItemCombo;
    QLineEdit* newQuantityEdit;
    QPushButton* addItemButton;

    struct OrderItemRow {
        QLabel* itemNameLabel;
        QLineEdit* quantityEdit;
        QPushButton* modifyButton;
        QPushButton* deleteButton;
        int itemId;
    };

    QMap<int, OrderItemRow> itemRows; // order_item_id -> row
    QMap<int, QString> itemIdToName; // item_id

    void setUpHeader();
    void setUpScrollArea();
    void addColumnLabels();
    void setUpAddItemWidget();

public:
    explicit ManagerOrderView(QWidget* parent = nullptr);

    void setOrderInfo(int orderId, const QString& createdBy, const QString& createdAt);
    void addOrderItemRow(int orderItemId, int itemId, const QString& itemName, int quantity);
    void clearItems();
    void setAvailableItems(const QMap<int, QString>& itemsMap);

signals:
    void backToOrdersList();
    void modifyOrderItem(int orderId, int orderItemId, int newQuantity);
    void deleteOrderItem(int orderId, int orderItemId);
    void addOrderItem(int orderId, int itemId, int quantity);

private slots:
    void handleBackButton();
};

#endif //MAGAZYN_MANAGERORDERVIEW_H
