//
// Created by jakub on 2025-12-06.
//

#include <QMessageBox>
#include <QIntValidator>
#include "WarehouseWorkerOrderView.h"

WarehouseWorkerOrderView::WarehouseWorkerOrderView(QWidget* parent) : QWidget(parent) {
    mainLayout = new QVBoxLayout(this);

    setUpCaption();     // Top row
    setUpScrollArea();  // scroll area

    submitButton = new QPushButton("Zatwierdź", this);
    mainLayout->addWidget(submitButton);

    connectButtons();
}


void WarehouseWorkerOrderView::setOrderInfo(int orderId, const QString& creationDate, const QString& createdBy) {
    this->orderId = orderId;
    orderInfoLabel->setText(QString("Zamówienie id: %1\nUtworzone: %2\nPrzez: %3").arg(orderId).arg(creationDate).arg(createdBy));
}


void WarehouseWorkerOrderView::addOrderItem(int itemId, const QString& itemName, int quantity, int pickedQuantity, int availableQuantity) {
    QWidget* rowWidget = new QWidget(scrollWidget);
    QHBoxLayout* rowLayout = new QHBoxLayout(rowWidget);

    QLabel* nameLabel = new QLabel(itemName, rowWidget);
    QLabel* qtyLabel = new QLabel(QString::number(quantity), rowWidget);
    QLineEdit* pickedEdit = new QLineEdit(QString::number(pickedQuantity), rowWidget);
    QLabel* availableLabel = new QLabel(QString::number(availableQuantity), rowWidget);

    QIntValidator* validator = new QIntValidator(0, availableQuantity, pickedEdit);
    pickedEdit->setValidator(validator);
    // Auto validate
    connect(pickedEdit, &QLineEdit::textChanged, [pickedEdit, quantity, availableQuantity](const QString &text){
        if (text.isEmpty()) return;
        int val = text.toInt();
        int maxAllowed = qMin(quantity, availableQuantity); // Max allowed input
        if (val > maxAllowed) {
            pickedEdit->setText(QString::number(maxAllowed));
        } else if (val < 0) {
            pickedEdit->setText("0");
        }
    });

    rowLayout->addWidget(nameLabel, 3);
    rowLayout->addWidget(qtyLabel, 1, Qt::AlignCenter);
    rowLayout->addWidget(pickedEdit, 1, Qt::AlignCenter);
    rowLayout->addWidget(availableLabel, 1, Qt::AlignCenter);

    rowWidget->setLayout(rowLayout);
    itemsLayout->addWidget(rowWidget);

    itemRows[itemId] = {nameLabel, qtyLabel, pickedEdit, availableLabel};
}

void WarehouseWorkerOrderView::setUpScrollArea() {
    scrollArea = new QScrollArea(this);
    scrollWidget = new QWidget(scrollArea);
    itemsLayout = new QVBoxLayout(scrollWidget);

    // Nagłówek kolumn
    QWidget* headerWidget = new QWidget(scrollWidget);
    QHBoxLayout* headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->addWidget(new QLabel("Nazwa towaru", headerWidget), 3);
    headerLayout->addWidget(new QLabel("Do zebrania", headerWidget), 1, Qt::AlignCenter);
    headerLayout->addWidget(new QLabel("Zebrane", headerWidget), 1, Qt::AlignCenter);
    headerLayout->addWidget(new QLabel("Dostępne", headerWidget), 1, Qt::AlignCenter);
    headerWidget->setLayout(headerLayout);

    itemsLayout->addWidget(headerWidget);

    scrollWidget->setLayout(itemsLayout);
    scrollArea->setWidget(scrollWidget);
    scrollArea->setWidgetResizable(true);

    mainLayout->addWidget(scrollArea);
}

void WarehouseWorkerOrderView::setUpCaption() {
    QWidget* topRowWidget = new QWidget(this);
    QHBoxLayout* topRowLayout = new QHBoxLayout(topRowWidget);

    orderInfoLabel = new QLabel(topRowWidget);
    backButton = new QPushButton("Powrót", topRowWidget);

    topRowLayout->addWidget(orderInfoLabel, 1);
    topRowLayout->addWidget(backButton, 0, Qt::AlignRight);
    topRowWidget->setLayout(topRowLayout);

    mainLayout->addWidget(topRowWidget);
}

void WarehouseWorkerOrderView::connectButtons() {
    connect(backButton, &QPushButton::clicked, this, &WarehouseWorkerOrderView::handleBackButton);
    connect(submitButton, &QPushButton::clicked, this, &WarehouseWorkerOrderView::handleSubmitOrder);
}

void WarehouseWorkerOrderView::handleBackButton() {
    emit backToMainMenu();
}

void WarehouseWorkerOrderView::handleSubmitOrder() {
    QMap<int,int> pickedMap;
    for (auto it = itemRows.begin(); it != itemRows.end(); it++) {
        pickedMap[it.key()] = it.value().pickedLineEdit->text().toInt();
    }
    emit submitOrder(orderId, pickedMap);
}

void WarehouseWorkerOrderView::clearOrderItems() {
    //Clear all without first row (caption)
    while (itemsLayout->count() > 1) {
        QLayoutItem* item = itemsLayout->takeAt(1);
        if (item) {
            QWidget* widget = item->widget();
            if (widget) {
                widget->deleteLater();
            }
            delete item;
        }
    }
    itemRows.clear();
}

void WarehouseWorkerOrderView::viewError(const QString& message) {
    QMessageBox::critical(this, "Błąd", message);
}
