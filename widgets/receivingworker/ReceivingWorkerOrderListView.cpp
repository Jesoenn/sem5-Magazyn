//
// Created by jakub on 2025-12-07.
//
#include "ReceivingWorkerOrderListView.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QDateTime>
#include <QMessageBox>

ReceivingWorkerOrderListView::ReceivingWorkerOrderListView(QWidget* parent)
        : QWidget(parent)
{
    mainLayout = new QVBoxLayout(this);
    backButton = new QPushButton("Powrót", this);
    mainLayout->addWidget(backButton, 0, Qt::AlignRight);
    connect(backButton, &QPushButton::clicked, this, &ReceivingWorkerOrderListView::handleBackButton);

    scrollArea = new QScrollArea(this);
    scrollWidget = new QWidget(scrollArea);
    ordersLayout = new QVBoxLayout(scrollWidget);

    setUpHeader();
    setUpScrollArea();
}

void ReceivingWorkerOrderListView::addOrderInfo(int orderId, const QString& employeeName, const QDateTime& creationDate)
{
    QWidget* rowWidget = new QWidget(scrollWidget);
    QHBoxLayout* rowLayout = new QHBoxLayout(rowWidget);

    QLabel* idLabel = new QLabel(QString::number(orderId), rowWidget);
    QLabel* employeeLabel = new QLabel(employeeName, rowWidget);
    QLabel* dateLabel = new QLabel(creationDate.toString("dd.MM.yyyy HH:mm"), rowWidget);

    QPushButton* checkButton = new QPushButton("Sprawdź", rowWidget);
    QPushButton* confirmButton = new QPushButton("Potwierdź", rowWidget);

    rowLayout->addWidget(idLabel, 1, Qt::AlignCenter);
    rowLayout->addWidget(employeeLabel, 2, Qt::AlignCenter);
    rowLayout->addWidget(dateLabel, 2, Qt::AlignCenter);
    rowLayout->addWidget(checkButton, 1, Qt::AlignCenter);
    rowLayout->addWidget(confirmButton, 1, Qt::AlignCenter);

    rowWidget->setLayout(rowLayout);
    ordersLayout->addWidget(rowWidget);

    connect(checkButton, &QPushButton::clicked, [this, orderId]() {
        emit checkOrder(orderId);
    });
    connect(confirmButton, &QPushButton::clicked, [this, orderId]() {
        emit confirmOrder(orderId);
    });
}

void ReceivingWorkerOrderListView::clearOrders()
{
    while (ordersLayout->count() > 1) {
        QLayoutItem* item = ordersLayout->takeAt(1);
        if (item) {
            QWidget* widget = item->widget();
            if (widget) {
                widget->deleteLater();
            }
            delete item;
        }
    }
}

void ReceivingWorkerOrderListView::handleBackButton()
{
    emit backToMainMenu();
}

void ReceivingWorkerOrderListView::setUpScrollArea() {
    scrollWidget->setLayout(ordersLayout);
    scrollArea->setWidget(scrollWidget);
    scrollArea->setWidgetResizable(true);
    mainLayout->addWidget(scrollArea);
}

void ReceivingWorkerOrderListView::setUpHeader() {
    QWidget* header = new QWidget(scrollWidget);
    QHBoxLayout* headerLayout = new QHBoxLayout(header);
    headerLayout->addWidget(new QLabel("ID Zamówienia", header), 1);
    headerLayout->addWidget(new QLabel("Pracownik", header), 2);
    headerLayout->addWidget(new QLabel("Data utworzenia", header), 2);
    headerLayout->addWidget(new QLabel("      ", header), 2);
    header->setLayout(headerLayout);
    ordersLayout->addWidget(header);
}
