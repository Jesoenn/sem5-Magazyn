//
// Created by jakub on 2025-12-07.
//

#include "ReceivingWorkerDeliveryListView.h"
#include <QHBoxLayout>

ReceivingWorkerDeliveryListView::ReceivingWorkerDeliveryListView(QWidget* parent)
        : QWidget(parent)
{
    mainLayout = new QVBoxLayout(this);
    backButton = new QPushButton("Powrót", this);
    mainLayout->addWidget(backButton, 0, Qt::AlignRight);
    connect(backButton, &QPushButton::clicked, this, &ReceivingWorkerDeliveryListView::handleBackButton);

    scrollArea = new QScrollArea(this);
    scrollWidget = new QWidget(scrollArea);
    deliveriesLayout = new QVBoxLayout(scrollWidget);

    setUpHeader();
    setUpScrollArea();
}

void ReceivingWorkerDeliveryListView::addDeliveryInfo(int deliveryId, const QDateTime& deliveryDate)
{
    QWidget* rowWidget = new QWidget(scrollWidget);
    QHBoxLayout* rowLayout = new QHBoxLayout(rowWidget);

    QLabel* idLabel = new QLabel(QString::number(deliveryId), rowWidget);
    QLabel* dateLabel = new QLabel(deliveryDate.toString("dd.MM.yyyy HH:mm"), rowWidget);
    QPushButton* fillButton = new QPushButton("Uzupełnij dostawę", rowWidget);

    rowLayout->addWidget(idLabel, 1, Qt::AlignLeft);
    rowLayout->addWidget(dateLabel, 2, Qt::AlignLeft);
    rowLayout->addWidget(fillButton, 1, Qt::AlignLeft);

    rowWidget->setLayout(rowLayout);
    deliveriesLayout->addWidget(rowWidget);

    deliveryRows[deliveryId] = {idLabel, dateLabel, fillButton};

    connect(fillButton, &QPushButton::clicked, [this, deliveryId]() {
        emit fillDelivery(deliveryId);
    });
}

void ReceivingWorkerDeliveryListView::clearDeliveries()
{
    while (deliveriesLayout->count() > 1) { // zostawiamy header
        QLayoutItem* item = deliveriesLayout->takeAt(1);
        if (item) {
            QWidget* widget = item->widget();
            if (widget) {
                widget->deleteLater();
            }
            delete item;
        }
    }
    deliveryRows.clear();
}

void ReceivingWorkerDeliveryListView::handleBackButton()
{
    emit backToMainMenu();
}

void ReceivingWorkerDeliveryListView::setUpScrollArea() {
    scrollWidget->setLayout(deliveriesLayout);
    scrollArea->setWidget(scrollWidget);
    scrollArea->setWidgetResizable(true);
    mainLayout->addWidget(scrollArea);
}

void ReceivingWorkerDeliveryListView::setUpHeader() {
    QWidget* header = new QWidget(scrollWidget);
    QHBoxLayout* headerLayout = new QHBoxLayout(header);
    headerLayout->addWidget(new QLabel("ID Dostawy", header), 1);
    headerLayout->addWidget(new QLabel("Data Dostawy", header), 2);
    headerLayout->addWidget(new QLabel("     ", header), 1);
    header->setLayout(headerLayout);
    deliveriesLayout->addWidget(header);
}
