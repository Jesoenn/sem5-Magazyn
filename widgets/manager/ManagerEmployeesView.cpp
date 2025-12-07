//
// Created by jakub on 2025-12-07.
//

#include "ManagerEmployeesView.h"
#include <QHBoxLayout>
#include <QMessageBox>
#include <QComboBox>

ManagerEmployeesView::ManagerEmployeesView(QWidget* parent) : QWidget(parent) {
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
}

void ManagerEmployeesView::setUpHeader() {
    headerLabel = new QLabel("Lista pracowników", this);
    backButton = new QPushButton("Powrót", this);
    connect(backButton, &QPushButton::clicked, this, &ManagerEmployeesView::handleBackButton);
}

void ManagerEmployeesView::setUpScrollArea() {
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollWidget = new QWidget(scrollArea);
    employeesLayout = new QVBoxLayout(scrollWidget);
    employeesLayout->setSpacing(5);
    employeesLayout->setContentsMargins(5,5,5,5);
    scrollWidget->setLayout(employeesLayout);
    scrollArea->setWidget(scrollWidget);
}

void ManagerEmployeesView::addColumnLabels() {
    QWidget* headerRow = new QWidget(scrollWidget);
    QHBoxLayout* headerLayout = new QHBoxLayout(headerRow);
    headerLayout->addWidget(new QLabel("ID"));
    headerLayout->addWidget(new QLabel("Login"));
    headerLayout->addWidget(new QLabel("Imię"));
    headerLayout->addWidget(new QLabel("Nazwisko"));
    headerLayout->addWidget(new QLabel("Stanowisko"));
    headerLayout->addWidget(new QLabel("Zatrudniony"));
    headerLayout->addWidget(new QLabel("   "));
    headerRow->setLayout(headerLayout);
    employeesLayout->addWidget(headerRow);
}

void ManagerEmployeesView::addEmployeeRow(int employeeId, const QString& login, const QString& firstName, const QString& lastName, int jobId, const QString& jobName, bool employed) {
    QWidget* rowWidget = new QWidget(scrollWidget);
    QHBoxLayout* rowLayout = new QHBoxLayout(rowWidget);
    QLabel* idLabel = new QLabel(QString::number(employeeId), rowWidget);
    QLineEdit* loginEdit = new QLineEdit(login, rowWidget);
    QLineEdit* firstNameEdit = new QLineEdit(firstName, rowWidget);
    QLineEdit* lastNameEdit = new QLineEdit(lastName, rowWidget);
    QLabel* jobLabel = new QLabel(jobName, rowWidget);
    QComboBox* employedCombo = new QComboBox(rowWidget);
    employedCombo->addItem("Tak", true);
    employedCombo->addItem("Nie", false);
    employedCombo->setCurrentIndex(employed ? 0 : 1);
    QPushButton* fireButton = new QPushButton("Zwolnij", rowWidget);
    QPushButton* modifyButton = new QPushButton("Modyfikuj", rowWidget);

    // Active for 1 lub 2
    if (jobId != 1 && jobId != 2) {
        fireButton->setEnabled(false);
        modifyButton->setEnabled(false);
        loginEdit->setReadOnly(true);
        firstNameEdit->setReadOnly(true);
        lastNameEdit->setReadOnly(true);
        employedCombo->setEnabled(false);
    }

    rowLayout->addWidget(idLabel);
    rowLayout->addWidget(loginEdit);
    rowLayout->addWidget(firstNameEdit);
    rowLayout->addWidget(lastNameEdit);
    rowLayout->addWidget(jobLabel);
    rowLayout->addWidget(employedCombo);
    rowLayout->addWidget(fireButton);
    rowLayout->addWidget(modifyButton);

    rowWidget->setLayout(rowLayout);
    employeesLayout->addWidget(rowWidget);

    employeeRows[employeeId] = {idLabel, loginEdit, firstNameEdit, lastNameEdit, jobLabel, employedCombo, fireButton, modifyButton, jobId};

    connect(fireButton, &QPushButton::clicked, [this, employeeId]() {
        emit fireEmployee(employeeId);
    });

    connect(modifyButton, &QPushButton::clicked, [this, employeeId]() {
        EmployeeRow row = employeeRows[employeeId];
        emit modifyEmployee(employeeId,row.loginEdit->text(),row.firstNameEdit->text(),row.lastNameEdit->text(),row.employedCombo->currentText() == "Tak");
    });
}

void ManagerEmployeesView::clearEmployees() {
    QLayoutItem* item;
    while ((item = employeesLayout->takeAt(1)) != nullptr) { // skip first row (column labels)
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }
    employeeRows.clear();
}

void ManagerEmployeesView::handleBackButton() {
    emit backToMainMenu();
}

void ManagerEmployeesView::setUpAddEmployeeWidget() {
    addEmployeeWidget = new QWidget(this);
    QVBoxLayout* addLayout = new QVBoxLayout(addEmployeeWidget);

    QHBoxLayout* headerLayout = new QHBoxLayout();
    headerLayout->addWidget(new QLabel("Login"));
    headerLayout->addWidget(new QLabel("Imię"));
    headerLayout->addWidget(new QLabel("Nazwisko"));
    headerLayout->addWidget(new QLabel("Stanowisko"));
    headerLayout->addWidget(new QLabel("Hasło"));
    headerLayout->addWidget(new QLabel("\t\t"));
    addLayout->addLayout(headerLayout);

    QHBoxLayout* fieldsLayout = new QHBoxLayout();
    newLoginEdit = new QLineEdit(addEmployeeWidget);
    newFirstNameEdit = new QLineEdit(addEmployeeWidget);
    newLastNameEdit = new QLineEdit(addEmployeeWidget);
    newJobCombo = new QComboBox(addEmployeeWidget);
    for (auto it = jobIdToName.begin(); it != jobIdToName.end(); ++it) {
        if (it.key() != 1 && it.key() != 2)
            continue;
        newJobCombo->addItem(it.value(), it.key());
    }
    newPasswordEdit = new QLineEdit(addEmployeeWidget);
    newPasswordEdit->setEchoMode(QLineEdit::Password);
    addButton = new QPushButton("Dodaj", addEmployeeWidget);

    fieldsLayout->addWidget(newLoginEdit);
    fieldsLayout->addWidget(newFirstNameEdit);
    fieldsLayout->addWidget(newLastNameEdit);
    fieldsLayout->addWidget(newJobCombo);
    fieldsLayout->addWidget(newPasswordEdit);
    fieldsLayout->addWidget(addButton);
    addLayout->addLayout(fieldsLayout);

    // under scroll
    mainLayout->addWidget(addEmployeeWidget);

    connect(addButton, &QPushButton::clicked, [this]() {
        int jobId = newJobCombo->currentData().toInt();
        emit addEmployee(newLoginEdit->text(), newFirstNameEdit->text(), newLastNameEdit->text(), jobId, newPasswordEdit->text());

        newLoginEdit->clear();
        newFirstNameEdit->clear();
        newLastNameEdit->clear();
        newPasswordEdit->clear();
        newJobCombo->setCurrentIndex(0);
    });
}

void ManagerEmployeesView::setJobMap(const QMap<int, QString>& jobMap) {
    jobIdToName = jobMap;
    if(!hireWidgetCreated){
        setUpAddEmployeeWidget();
        hireWidgetCreated = true;
    }
}

