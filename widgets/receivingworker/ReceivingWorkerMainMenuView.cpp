//
// Created by jakub on 2025-12-07.
//

#include "ReceivingWorkerMainMenuView.h"
#include <QMessageBox>
#include <QVBoxLayout>

ReceivingWorkerMainMenuView::ReceivingWorkerMainMenuView(QWidget* parent) : QWidget(parent) {
    setUp();

}

void ReceivingWorkerMainMenuView::setEmployeeInfo(int employeeId, const QString& firstName, const QString& lastName, const QString& job)
{
    employeeIdLabel->setText(QString("Identyfikator: %1").arg(employeeId));
    nameLabel->setText(QString("Zalogowany jako: %1 %2").arg(firstName, lastName));
    jobLabel->setText(QString("Stanowisko: %1").arg(job));
}

void ReceivingWorkerMainMenuView::setAssignedVehicle(const QString& vehicleId)
{
    vehicleLabel->setText(QString("Pojazd: %1").arg(vehicleId));
}

void ReceivingWorkerMainMenuView::setUp() {
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

    assignVehicleButton = new QPushButton("Zajmij pojazd", this);
    freeVehicleButton = new QPushButton("Zwolnij pojazd", this);
    newDeliveryButton = new QPushButton("Stwórz nową dostawę", this);
    logoutButton   = new QPushButton("Wyloguj", this);
    viewDeliveryButton = new QPushButton("Zobacz swoje dostawy", this);
    verifyOrdersButton = new QPushButton("Zweryfikuj zamówienia", this);

    QVBoxLayout* buttonsLayout = new QVBoxLayout();
    buttonsLayout->addWidget(assignVehicleButton);
    buttonsLayout->addWidget(freeVehicleButton);
    buttonsLayout->addWidget(viewDeliveryButton);
    buttonsLayout->addWidget(newDeliveryButton);
    buttonsLayout->addWidget(verifyOrdersButton);
    buttonsLayout->addWidget(logoutButton);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(infoLayout);
    mainLayout->addLayout(buttonsLayout);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    setLayout(mainLayout);

    connect(newDeliveryButton, &QPushButton::clicked, this, &ReceivingWorkerMainMenuView::handleNewDelivery);
    connect(logoutButton, &QPushButton::clicked, this, &ReceivingWorkerMainMenuView::handleLogout);
    connect(viewDeliveryButton, &QPushButton::clicked, this, &ReceivingWorkerMainMenuView::handleViewDelivery);
    connect(assignVehicleButton, &QPushButton::clicked, this, &ReceivingWorkerMainMenuView::handleAssignVehicle);
    connect(freeVehicleButton, &QPushButton::clicked, this, &ReceivingWorkerMainMenuView::handleFreeVehicle);
    connect(verifyOrdersButton, &QPushButton::clicked, this, &ReceivingWorkerMainMenuView::handleVerifyOrders);
}

void ReceivingWorkerMainMenuView::handleViewDelivery() {
    emit viewDelivery();
}

void ReceivingWorkerMainMenuView::handleLogout() {
    emit logoutRequest();
}

void ReceivingWorkerMainMenuView::handleNewDelivery() {
    emit getNewDelivery();
}

void ReceivingWorkerMainMenuView::viewError(const QString& message) {
    QMessageBox::critical(this, "Błąd", message);
}

void ReceivingWorkerMainMenuView::handleAssignVehicle() {
    emit assignVehicle();
}

void ReceivingWorkerMainMenuView::showAssignVehicleButton(bool show) {
    if(assignVehicleButton)
        assignVehicleButton->setVisible(show);
}

void ReceivingWorkerMainMenuView::showViewOrderButton(bool show) {
    if(viewDeliveryButton)
        viewDeliveryButton->setVisible(show);
}

void ReceivingWorkerMainMenuView::showNewOrderButton(bool show) {
    if(newDeliveryButton)
        newDeliveryButton->setVisible(show);
}

void ReceivingWorkerMainMenuView::showFreeVehicleButton(bool show) {
    if(freeVehicleButton)
        freeVehicleButton->setVisible(show);
}

void ReceivingWorkerMainMenuView::handleFreeVehicle() {
    emit freeVehicle();
}

void ReceivingWorkerMainMenuView::showVerifyOrders(bool show) {
    if(verifyOrdersButton)
        verifyOrdersButton->setVisible(show);
}

void ReceivingWorkerMainMenuView::handleVerifyOrders(){
    emit verifyOrders();
}

