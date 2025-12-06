//
// Created by jakub on 2025-12-06.
//

#ifndef MAGAZYN_WAREHOUSEWORKERORDERVIEW_H
#define MAGAZYN_WAREHOUSEWORKERORDERVIEW_H


#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>


class WarehouseWorkerOrderView : public QWidget {
    Q_OBJECT

public:
    WarehouseWorkerOrderView(QWidget* parent = nullptr);
    void setOrderInfo(int orderId, const QString& creationDate, const QString& createdBy);
    void addOrderItem(int itemId, const QString& itemName, int quantity, int pickedQuantity, int availableQuantity);

    void viewError(const QString& message);

    void handleSubmitOrder();
    void handleBackButton();
    void clearOrderItems();

signals:
    void backToMainMenu();
    void submitOrder(int orderId, QMap<int,int> pickedQuantities); // Map -> item_id / picked_quantity

private:
    int orderId;
    QVBoxLayout* mainLayout;

    //Top row
    QLabel* orderInfoLabel;
    QPushButton* backButton;

    //Scroll
    QScrollArea* scrollArea;
    QWidget* scrollWidget;
    QVBoxLayout* itemsLayout;

    QPushButton* submitButton;

    struct ItemRow {
        QLabel* itemNameLabel;
        QLabel* quantityLabel;
        QLineEdit* pickedLineEdit;
        QLabel* availableLabel;
    };
    QMap<int, ItemRow> itemRows; // Map -> item_id / ItemRow

    void setUpCaption();
    void setUpScrollArea();
    void connectButtons();
};



#endif //MAGAZYN_WAREHOUSEWORKERORDERVIEW_H
