
#include "AdminMainMenuView.h"

//
// Created by jakub on 2026-01-21.
//

#include "AdminMainMenuView.h"
#include <QVBoxLayout>
#include <QMessageBox>

AdminMainMenuView::AdminMainMenuView(QWidget* parent)
        : QWidget(parent)
{
    setUp();
}

void AdminMainMenuView::setUp() {
    employeeIdLabel = new QLabel("ID: ", this);
    nameLabel       = new QLabel("Imię i nazwisko: ", this);
    jobLabel        = new QLabel("Stanowisko: ", this);

    QVBoxLayout* infoLayout = new QVBoxLayout();
    infoLayout->addWidget(employeeIdLabel);
    infoLayout->addWidget(nameLabel);
    infoLayout->addWidget(jobLabel);
    infoLayout->addSpacing(10);

    viewEmployeesButton = new QPushButton("Zobacz pracowników", this);
    viewEventLogButton = new QPushButton("Wyświetl dziennik zdarzeń", this);
    viewManagerViewButton = new QPushButton("Widok menedżera", this);
    viewReceivingWorkerViewButton = new QPushButton("Widok pracownika przyjęć", this);
    viewWarehouseWorkerViewButton = new QPushButton("Widok magazyniera", this);
    logoutButton = new QPushButton("Wyloguj", this);

    QVBoxLayout* buttonsLayout = new QVBoxLayout();
    buttonsLayout->addWidget(viewEmployeesButton);
    buttonsLayout->addWidget(viewEventLogButton);
    buttonsLayout->addSpacing(10);
    buttonsLayout->addWidget(viewManagerViewButton);
    buttonsLayout->addWidget(viewReceivingWorkerViewButton);
    buttonsLayout->addWidget(viewWarehouseWorkerViewButton);
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

    connect(viewEmployeesButton, &QPushButton::clicked,
            this, &AdminMainMenuView::handleViewEmployees);
    connect(viewEventLogButton, &QPushButton::clicked,
            this, &AdminMainMenuView::handleViewEventLog);
    connect(viewManagerViewButton, &QPushButton::clicked,
            this, &AdminMainMenuView::handleViewManagerView);
    connect(viewReceivingWorkerViewButton, &QPushButton::clicked,
            this, &AdminMainMenuView::handleViewReceivingWorkerView);
    connect(viewWarehouseWorkerViewButton, &QPushButton::clicked,
            this, &AdminMainMenuView::handleViewWarehouseWorkerView);
    connect(logoutButton, &QPushButton::clicked,
            this, &AdminMainMenuView::handleLogout);
}

void AdminMainMenuView::setEmployeeInfo(int employeeId,const QString& firstName,const QString& lastName,const QString& job) {
    employeeIdLabel->setText(QString("Identyfikator: %1").arg(employeeId));
    nameLabel->setText(QString("Zalogowany jako: %1 %2").arg(firstName, lastName));
    jobLabel->setText(QString("Stanowisko: %1").arg(job));
}

void AdminMainMenuView::viewError(const QString& message) {
    QMessageBox::critical(this, "Błąd", message);
}

void AdminMainMenuView::handleViewEmployees() {
    emit viewEmployees();
}

void AdminMainMenuView::handleViewEventLog() {
    emit viewEventLog();
}

void AdminMainMenuView::handleViewManagerView() {
    emit viewManagerView();
}

void AdminMainMenuView::handleViewReceivingWorkerView() {
    emit viewReceivingWorkerView();
}

void AdminMainMenuView::handleViewWarehouseWorkerView() {
    emit viewWarehouseWorkerView();
}

void AdminMainMenuView::handleLogout() {
    emit logoutRequest();
}
