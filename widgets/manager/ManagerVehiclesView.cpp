//
// Created by jakub on 2025-12-07.
//

#include "ManagerVehiclesView.h"
#include <QHBoxLayout>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QScrollBar>

ManagerVehiclesView::ManagerVehiclesView(QWidget* parent) : QWidget(parent) {
    setUpHeader();
    setUpScrollArea();

    // Nagłówek kolumn nad scroll area
    QWidget* columnHeaderWidget = new QWidget(this);
    QHBoxLayout* columnHeaderLayout = new QHBoxLayout(columnHeaderWidget);
    columnHeaderLayout->setContentsMargins(5, 5, 5, 5);
    columnHeaderLayout->setSpacing(10);

    QLabel* vehicleIdHeader = new QLabel("Numer pojazdu", columnHeaderWidget);
    QLabel* statusHeader = new QLabel("Status", columnHeaderWidget);
    QLabel* employeeHeader = new QLabel("Zajęty przez pracownika", columnHeaderWidget);
    QLabel* actionHeader = new QLabel("       ", columnHeaderWidget);

    columnHeaderLayout->addWidget(vehicleIdHeader, 1, Qt::AlignCenter);
    columnHeaderLayout->addWidget(statusHeader, 1, Qt::AlignCenter);
    columnHeaderLayout->addWidget(employeeHeader, 2, Qt::AlignCenter);
    columnHeaderLayout->addWidget(actionHeader, 1, Qt::AlignCenter);
    columnHeaderWidget->setLayout(columnHeaderLayout);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->addWidget(headerLabel);
    topLayout->addStretch();
    topLayout->addWidget(backButton);
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(columnHeaderWidget);
    mainLayout->addWidget(scrollArea);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);
    setLayout(mainLayout);
}


void ManagerVehiclesView::setUpHeader() {
    headerLabel = new QLabel("Zarządzanie pojazdami", this);
    backButton = new QPushButton("Powrót", this);
    connect(backButton, &QPushButton::clicked, this, &ManagerVehiclesView::handleBackButton);
}

void ManagerVehiclesView::setUpScrollArea() {
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);

    scrollWidget = new QWidget(scrollArea);
    vehiclesLayout = new QVBoxLayout(scrollWidget);
    vehiclesLayout->setSpacing(5);
    vehiclesLayout->setContentsMargins(5, 5, 5, 5);

    scrollWidget->setLayout(vehiclesLayout);
    scrollArea->setWidget(scrollWidget);
}

void ManagerVehiclesView::handleBackButton() {
    emit backToMainMenu();
}

void ManagerVehiclesView::clearVehicles() {
    QLayoutItem* item;
    while ((item = vehiclesLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    vehicleRows.clear();
}


void ManagerVehiclesView::setEmployeeMap(const QMap<int, QString>& employeeMap) {
    employeeIdToName = employeeMap;
}

void ManagerVehiclesView::addVehicleRow(int vehicleId, const QString& status, int assignedEmployeeId) {
    QWidget* rowWidget = new QWidget(scrollWidget);
    QHBoxLayout* rowLayout = new QHBoxLayout(rowWidget);
    QLabel* idLabel = new QLabel(QString::number(vehicleId), rowWidget);

    QComboBox* statusCombo = new QComboBox(rowWidget);
    statusCombo->addItems({"wolny", "zajety"});
    statusCombo->setCurrentText(status);

    QComboBox* employeeCombo = new QComboBox(rowWidget);
    employeeCombo->addItem(""); // employee_id = NULL

    int currentIndex = 0;
    for (auto it = employeeIdToName.begin(); it != employeeIdToName.end(); ++it) {
        employeeCombo->addItem(it.value(), it.key());
        if (it.key() == assignedEmployeeId) {
            currentIndex = employeeCombo->count() - 1;
        }
    }
    employeeCombo->setCurrentIndex(currentIndex);
    QPushButton* saveButton = new QPushButton("Zapisz", rowWidget);
    rowLayout->addWidget(idLabel, 1, Qt::AlignCenter);
    rowLayout->addWidget(statusCombo, 1, Qt::AlignCenter);
    rowLayout->addWidget(employeeCombo, 2, Qt::AlignCenter);
    rowLayout->addWidget(saveButton, 1, Qt::AlignCenter);
    rowWidget->setLayout(rowLayout);
    vehiclesLayout->addWidget(rowWidget);

    vehicleRows[vehicleId] = {idLabel, statusCombo, employeeCombo, saveButton};
    connect(saveButton, &QPushButton::clicked, [this, vehicleId, statusCombo, employeeCombo]() {
        int employeeId = -1; // default val (null)
        int currentIndex = employeeCombo->currentIndex();
        if (currentIndex >= 0) {
            QVariant data = employeeCombo->itemData(currentIndex);
            if (!data.isNull()) {
                employeeId = data.toInt();
            }
        }
        emit saveVehicle(vehicleId, statusCombo->currentText(), employeeId);
    });
}
