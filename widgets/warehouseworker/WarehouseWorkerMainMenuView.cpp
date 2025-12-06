//
// Created by jakub on 2025-12-06.
//

#include <QMessageBox>
#include "WarehouseWorkerMainMenuView.h"

WarehouseWorkerMainMenuView::WarehouseWorkerMainMenuView(QWidget* parent) : QWidget(parent) {
    setUp();

}

void WarehouseWorkerMainMenuView::setEmployeeInfo(int employeeId, const QString& firstName, const QString& lastName, const QString& job)
{
    employeeIdLabel->setText(QString("Identyfikator: %1").arg(employeeId));
    nameLabel->setText(QString("Zalogowany jako: %1 %2").arg(firstName, lastName));
    jobLabel->setText(QString("Stanowisko: %1").arg(job));
}

void WarehouseWorkerMainMenuView::setAssignedVehicle(const QString& vehicleId)
{
    vehicleLabel->setText(QString("Pojazd: %1").arg(vehicleId));
}

void WarehouseWorkerMainMenuView::setUp() {
    employeeIdLabel = new QLabel("ID: ", this);
    nameLabel       = new QLabel("Imię i nazwisko: ", this);
    jobLabel        = new QLabel("Stanowisko: ", this);
    vehicleLabel    = new QLabel("Pojazd: ", this);

    QVBoxLayout* infoLayout = new QVBoxLayout();
    infoLayout->addWidget(employeeIdLabel);
    infoLayout->addWidget(nameLabel);
    infoLayout->addWidget(jobLabel);
    infoLayout->addSpacing(10);
    infoLayout->addWidget(vehicleLabel);
    infoLayout->addStretch();

    newOrderButton = new QPushButton("Pobierz nowe zamówienie", this);
    logoutButton   = new QPushButton("Wyloguj", this);
    viewOrderButton = new QPushButton("Zobacz zamówienie", this);
    assignVehicleButton = new QPushButton("Zajmij pojazd", this);
    submitOrderButton   = new QPushButton("Złóż zamówienie", this);
    freeVehicleButton = new QPushButton("Zwolnij pojazd", this);

    QVBoxLayout* buttonsLayout = new QVBoxLayout();
    buttonsLayout->addWidget(assignVehicleButton);
    buttonsLayout->addWidget(freeVehicleButton);
    buttonsLayout->addWidget(viewOrderButton);
    buttonsLayout->addWidget(submitOrderButton);
    buttonsLayout->addWidget(newOrderButton);
    buttonsLayout->addWidget(logoutButton);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(infoLayout);
    mainLayout->addLayout(buttonsLayout);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    setLayout(mainLayout);

    connect(newOrderButton, &QPushButton::clicked, this, &WarehouseWorkerMainMenuView::handleNewOrder);
    connect(logoutButton, &QPushButton::clicked, this, &WarehouseWorkerMainMenuView::handleLogout);
    connect(viewOrderButton, &QPushButton::clicked, this, &WarehouseWorkerMainMenuView::handleViewOrder);
    connect(assignVehicleButton, &QPushButton::clicked, this, &WarehouseWorkerMainMenuView::handleAssignVehicle);
    connect(submitOrderButton, &QPushButton::clicked, this, &WarehouseWorkerMainMenuView::handleSubmitOrder);
    connect(freeVehicleButton, &QPushButton::clicked, this, &WarehouseWorkerMainMenuView::handleFreeVehicle);
}

void WarehouseWorkerMainMenuView::handleViewOrder() {
    emit viewOrder();
}

void WarehouseWorkerMainMenuView::handleLogout() {
    emit logoutRequest();
}

void WarehouseWorkerMainMenuView::handleNewOrder() {
    emit getNewOrder();
}

void WarehouseWorkerMainMenuView::viewError(const QString& message) {
    QMessageBox::critical(this, "Błąd", message);
}

void WarehouseWorkerMainMenuView::handleAssignVehicle() {
    emit assignVehicle();
}

void WarehouseWorkerMainMenuView::handleSubmitOrder() {
    emit submitOrder();
}

void WarehouseWorkerMainMenuView::showAssignVehicleButton(bool show) {
    if(assignVehicleButton)
        assignVehicleButton->setVisible(show);
}

void WarehouseWorkerMainMenuView::showViewOrderButton(bool show) {
    if(viewOrderButton)
        viewOrderButton->setVisible(show);
}

void WarehouseWorkerMainMenuView::showSubmitOrderButton(bool show) {
    if(submitOrderButton)
        submitOrderButton->setVisible(show);
}

void WarehouseWorkerMainMenuView::showNewOrderButton(bool show) {
    if(newOrderButton)
        newOrderButton->setVisible(show);
}

void WarehouseWorkerMainMenuView::showFreeVehicleButton(bool show) {
    if(freeVehicleButton)
        freeVehicleButton->setVisible(show);
}

void WarehouseWorkerMainMenuView::handleFreeVehicle() {
    emit freeVehicle();
}

