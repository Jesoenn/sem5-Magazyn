//
// Created by jakub on 2025-12-08.
//

#include "ManagerOrderView.h"
#include <QIntValidator>
#include <QMessageBox>

ManagerOrderView::ManagerOrderView(QWidget* parent) : QWidget(parent) {
    mainLayout = new QVBoxLayout(this);

    setUpHeader();
    setUpScrollArea();
    addColumnLabels();
    setUpAddItemWidget();

    mainLayout->setContentsMargins(10,10,10,10);
    mainLayout->setSpacing(10);
    setLayout(mainLayout);
}

void ManagerOrderView::setUpHeader() {
    QHBoxLayout* topLayout = new QHBoxLayout();
    orderInfoLabel = new QLabel("Zamówienie #0", this);
    backButton = new QPushButton("Powrót", this);
    connect(backButton, &QPushButton::clicked, this, &ManagerOrderView::handleBackButton);

    topLayout->addWidget(orderInfoLabel);
    topLayout->addStretch();
    topLayout->addWidget(backButton);

    mainLayout->addLayout(topLayout);
}

void ManagerOrderView::setUpScrollArea() {
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollWidget = new QWidget(scrollArea);
    itemsLayout = new QVBoxLayout(scrollWidget);
    itemsLayout->setSpacing(5);
    itemsLayout->setContentsMargins(5,5,5,5);
    scrollWidget->setLayout(itemsLayout);
    scrollArea->setWidget(scrollWidget);

    mainLayout->addWidget(scrollArea);
}

void ManagerOrderView::addColumnLabels() {
    QWidget* headerRow = new QWidget(scrollWidget);
    QHBoxLayout* headerLayout = new QHBoxLayout(headerRow);

    headerLayout->addWidget(new QLabel("Przedmiot"));
    headerLayout->addWidget(new QLabel("Ilość do wzięcia"));
    headerLayout->addWidget(new QLabel("Modyfikuj"));
    headerLayout->addWidget(new QLabel("Usuń"));

    headerRow->setLayout(headerLayout);
    itemsLayout->addWidget(headerRow);
}

void ManagerOrderView::addOrderItemRow(int orderItemId, int itemId, const QString& itemName, int quantity) {
    QWidget* rowWidget = new QWidget(scrollWidget);
    QHBoxLayout* rowLayout = new QHBoxLayout(rowWidget);

    QLabel* itemNameLabel = new QLabel(itemName, rowWidget);
    QLineEdit* quantityEdit = new QLineEdit(QString::number(quantity), rowWidget);
    quantityEdit->setValidator(new QIntValidator(1, 1000000, quantityEdit));

    QPushButton* modifyButton = new QPushButton("Modyfikuj", rowWidget);
    QPushButton* deleteButton = new QPushButton("Usuń", rowWidget);

    rowLayout->addWidget(itemNameLabel);
    rowLayout->addWidget(quantityEdit);
    rowLayout->addWidget(modifyButton);
    rowLayout->addWidget(deleteButton);

    rowWidget->setLayout(rowLayout);
    itemsLayout->addWidget(rowWidget);

    itemRows[orderItemId] = {itemNameLabel, quantityEdit, modifyButton, deleteButton, itemId};

    connect(modifyButton, &QPushButton::clicked, [this, orderItemId, quantityEdit]() {
        int newQty = quantityEdit->text().toInt();
        if (newQty <= 0) {
            QMessageBox::warning(this, "Błąd", "Ilość musi być większa od 0");
            return;
        }
        emit modifyOrderItem(orderItemId, newQty);
    });

    connect(deleteButton, &QPushButton::clicked, [this, orderItemId]() {
        emit deleteOrderItem(orderItemId);
    });
}

void ManagerOrderView::clearItems() {
    QLayoutItem* item;
    while ((item = itemsLayout->takeAt(1)) != nullptr) { // skip header row
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }
    itemRows.clear();
}

void ManagerOrderView::setOrderInfo(int orderId, const QString& createdBy, const QString& createdAt) {
    orderInfoLabel->setText(QString("Zamówienie #%1 | Stworzone przez: %2 | Data: %3").arg(orderId).arg(createdBy).arg(createdAt));
}

void ManagerOrderView::setUpAddItemWidget() {
    addItemWidget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(addItemWidget);

    layout->addWidget(new QLabel("Towar:"));
    newItemCombo = new QComboBox(addItemWidget);
    layout->addWidget(newItemCombo);

    layout->addWidget(new QLabel("Ilość:"));
    newQuantityEdit = new QLineEdit(addItemWidget);
    newQuantityEdit->setValidator(new QIntValidator(1, 1000000, newQuantityEdit));
    layout->addWidget(newQuantityEdit);

    addItemButton = new QPushButton("Dodaj", addItemWidget);
    layout->addWidget(addItemButton);

    mainLayout->addWidget(addItemWidget);

    connect(addItemButton, &QPushButton::clicked, [this]() {
        int qty = newQuantityEdit->text().toInt();
        int itemId = newItemCombo->currentData().toInt();
        emit addOrderItem(itemId, qty);

        newQuantityEdit->clear();
        newItemCombo->setCurrentIndex(0);
    });
}

void ManagerOrderView::setAvailableItems(const QMap<int, QString>& itemsMap) {
    itemIdToName = itemsMap;
    newItemCombo->clear();
    for (auto it = itemIdToName.begin(); it != itemIdToName.end(); ++it)
        newItemCombo->addItem(it.value(), it.key());
}

void ManagerOrderView::handleBackButton() {
    emit backToOrdersList();
}
