//
// Created by jakub on 2025-12-07.
//

#include "ManagerMainMenuView.h"
#include <QVBoxLayout>
#include <QMessageBox>

ManagerMainMenuView::ManagerMainMenuView(QWidget* parent)
        : QWidget(parent)
{
    setUp();
}

void ManagerMainMenuView::setUp()
{
    employeeIdLabel = new QLabel("ID: ", this);
    nameLabel       = new QLabel("Imię i nazwisko: ", this);
    jobLabel        = new QLabel("Stanowisko: ", this);

    QVBoxLayout* infoLayout = new QVBoxLayout();
    infoLayout->addWidget(employeeIdLabel);
    infoLayout->addWidget(nameLabel);
    infoLayout->addWidget(jobLabel);
    infoLayout->addSpacing(10);

    viewEmployeesButton = new QPushButton("Zobacz pracowników", this);
    viewVehiclesButton = new QPushButton("Zobacz pojazdy", this);
    viewOrdersButton = new QPushButton("Zobacz zamówienia", this);
    viewEventLogButton = new QPushButton("Wyświetl dziennik zdarzeń", this);
    logoutButton = new QPushButton("Wyloguj", this);

    QVBoxLayout* buttonsLayout = new QVBoxLayout();
    buttonsLayout->addWidget(viewEmployeesButton);
    buttonsLayout->addWidget(viewVehiclesButton);
    buttonsLayout->addWidget(viewOrdersButton);
    buttonsLayout->addWidget(viewEventLogButton);
    buttonsLayout->addSpacing(10);
    buttonsLayout->addWidget(logoutButton);
    buttonsLayout->addStretch();

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(infoLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(buttonsLayout);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);
    setLayout(mainLayout);

    connect(viewEmployeesButton, &QPushButton::clicked, this, &ManagerMainMenuView::handleViewEmployees);
    connect(viewVehiclesButton, &QPushButton::clicked, this, &ManagerMainMenuView::handleViewVehicles);
    connect(viewOrdersButton, &QPushButton::clicked, this, &ManagerMainMenuView::handleViewOrders);
    connect(viewEventLogButton, &QPushButton::clicked,this, &ManagerMainMenuView::handleViewEventLog);
    connect(logoutButton, &QPushButton::clicked, this, &ManagerMainMenuView::handleLogout);
}

void ManagerMainMenuView::setEmployeeInfo(int employeeId, const QString& firstName, const QString& lastName, const QString& job)
{
    employeeIdLabel->setText(QString("Identyfikator: %1").arg(employeeId));
    nameLabel->setText(QString("Zalogowany jako: %1 %2").arg(firstName, lastName));
    jobLabel->setText(QString("Stanowisko: %1").arg(job));
}

void ManagerMainMenuView::viewError(const QString& message)
{
    QMessageBox::critical(this, "Błąd", message);
}

void ManagerMainMenuView::handleViewEmployees() {
    emit viewEmployees();
}
void ManagerMainMenuView::handleViewVehicles() {
    emit viewVehicles();
}
void ManagerMainMenuView::handleViewOrders() {
    emit viewOrders();
}
void ManagerMainMenuView::handleViewEventLog() {
    emit viewEventLog();
}
void ManagerMainMenuView::handleLogout() {
    emit logoutRequest();
}
