//
// Created by jakub on 2025-12-08.
//

#include "ManagerOrdersListView.h"
#include <QHBoxLayout>

ManagerOrdersListView::ManagerOrdersListView(QWidget* parent): QWidget(parent)
{
    setUpHeader();
    setUpScrollArea();
    addColumnLabels();

    mainLayout = new QVBoxLayout(this);
    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->addWidget(headerLabel);
    topLayout->addStretch();
    topLayout->addWidget(backButton);
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(scrollArea);
    mainLayout->setContentsMargins(10,10,10,10);
    mainLayout->setSpacing(10);
    setLayout(mainLayout);
    setUpCreateOrderWidget();
}

void ManagerOrdersListView::setUpHeader() {
    headerLabel = new QLabel("Lista zamówień", this);
    backButton = new QPushButton("Powrót", this);
    connect(backButton, &QPushButton::clicked, this, &ManagerOrdersListView::handleBackButton);
}

void ManagerOrdersListView::handleBackButton() {
    emit backToMainMenu();
}

void ManagerOrdersListView::setUpScrollArea() {
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollWidget = new QWidget(scrollArea);
    ordersLayout = new QVBoxLayout(scrollWidget);
    ordersLayout->setSpacing(5);
    ordersLayout->setContentsMargins(5,5,5,5);
    scrollWidget->setLayout(ordersLayout);
    scrollArea->setWidget(scrollWidget);
}

void ManagerOrdersListView::addColumnLabels() {
    QWidget* headerRow = new QWidget(scrollWidget);
    QHBoxLayout* headerLayout = new QHBoxLayout(headerRow);
    headerLayout->addWidget(new QLabel("ID Zamówienia"));
    headerLayout->addWidget(new QLabel("Pracownik przypisany"));
    headerLayout->addWidget(new QLabel("Twórca"));
    headerLayout->addWidget(new QLabel("Data utworzenia"));
    headerRow->setLayout(headerLayout);
    ordersLayout->addWidget(headerRow);
}

void ManagerOrdersListView::setEmployeeMap(const QMap<int, QString>& map) {
    employeeMap = map;
    createEmployeeCombo->clear();
    for (auto it = employeeMap.begin(); it != employeeMap.end(); ++it)
        createEmployeeCombo->addItem(it.value(), it.key());
}

void ManagerOrdersListView::setUpCreateOrderWidget() {
    createOrderWidget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(createOrderWidget);
    layout->addWidget(new QLabel("Pracownik:"));
    createEmployeeCombo = new QComboBox(createOrderWidget);
    layout->addWidget(createEmployeeCombo);
    createOrderButton = new QPushButton("Stwórz zamówienie", createOrderWidget);
    layout->addWidget(createOrderButton);

    connect(createOrderButton, &QPushButton::clicked, [this]() {
        int employeeId = createEmployeeCombo->currentData().toInt();
        emit createOrder(employeeId);
    });

    mainLayout->addWidget(createOrderWidget);
}

void ManagerOrdersListView::addOrderRow(int orderId, int assignedEmployeeId, const QString& creatorName, const QString& createdAt)
{
    QWidget* rowWidget = new QWidget(scrollWidget);
    QHBoxLayout* rowLayout = new QHBoxLayout(rowWidget);
    QLabel* idLabel = new QLabel(QString::number(orderId), rowWidget);
    QComboBox* assignedCombo = new QComboBox(rowWidget);
    for (auto it = employeeMap.begin(); it != employeeMap.end(); ++it)
        assignedCombo->addItem(it.value(), it.key());
    assignedCombo->setCurrentIndex(assignedCombo->findData(assignedEmployeeId));

    QLabel* creatorLabel = new QLabel(creatorName, rowWidget);
    QLabel* dateLabel = new QLabel(createdAt, rowWidget);
    QPushButton* modifyButton = new QPushButton("Modyfikuj", rowWidget);
    QPushButton* deleteButton = new QPushButton("Usuń", rowWidget);

    connect(assignedCombo, &QComboBox::currentIndexChanged, [this, orderId, assignedCombo]() {
        emit assignEmployee(orderId, assignedCombo->currentData().toInt());
    });

    connect(modifyButton, &QPushButton::clicked, [this, orderId]() {
        emit modifyOrder(orderId);
    });

    connect(deleteButton, &QPushButton::clicked, [this, orderId]() {
        emit deleteOrder(orderId);
    });

    rowLayout->addWidget(idLabel);
    rowLayout->addWidget(assignedCombo);
    rowLayout->addWidget(creatorLabel);
    rowLayout->addWidget(dateLabel);
    rowLayout->addWidget(modifyButton);
    rowLayout->addWidget(deleteButton);
    rowWidget->setLayout(rowLayout);
    ordersLayout->addWidget(rowWidget);
    orderRows[orderId] = { idLabel, assignedCombo, creatorLabel, dateLabel, modifyButton, deleteButton };
}

void ManagerOrdersListView::clearOrders() {
    QLayoutItem* item;
    while ((item = ordersLayout->takeAt(1)) != nullptr) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }
    orderRows.clear();
}
