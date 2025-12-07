//
// Created by jakub on 2025-12-07.
//

#ifndef MAGAZYN_MANAGEREMPLOYEESVIEW_H
#define MAGAZYN_MANAGEREMPLOYEESVIEW_H

#include <QWidget>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QLabel>
#include <QMap>
#include <QLineEdit>
#include <QComboBox>

class ManagerEmployeesView : public QWidget {
Q_OBJECT
private:
    QPushButton* backButton;
    QLabel* headerLabel;
    QVBoxLayout* mainLayout;

    QScrollArea* scrollArea;
    QWidget* scrollWidget;
    QVBoxLayout* employeesLayout;

    QWidget* addEmployeeWidget;
    QLineEdit* newLoginEdit;
    QLineEdit* newFirstNameEdit;
    QLineEdit* newLastNameEdit;
    QComboBox* newJobCombo;
    QLineEdit* newPasswordEdit;
    QPushButton* addButton;
    bool hireWidgetCreated = false;

    struct EmployeeRow {
        QLabel* employeeIdLabel;
        QLineEdit* loginEdit;
        QLineEdit* firstNameEdit;
        QLineEdit* lastNameEdit;
        QLabel* jobLabel;
        QComboBox* employedCombo;
        QPushButton* fireButton;
        QPushButton* modifyButton;
        int jobId;
    };

    QMap<int, EmployeeRow> employeeRows; // employee_id -> row
    QMap<int, QString> jobIdToName; // job_id -> job name

    void setUpHeader();
    void setUpScrollArea();
    void addColumnLabels();
    void setUpAddEmployeeWidget();

public:
    explicit ManagerEmployeesView(QWidget* parent = nullptr);

    void addEmployeeRow(int employeeId, const QString& login, const QString& firstName, const QString& lastName, int jobId, const QString& jobName, bool employed);
    void clearEmployees();
    void setJobMap(const QMap<int, QString>& jobMap);

signals:
    void backToMainMenu();
    void fireEmployee(int employeeId);
    void modifyEmployee(int employeeId, const QString& login, const QString& firstName, const QString& lastName, bool employed);
    void addEmployee(const QString& login, const QString& firstName, const QString& lastName, int jobId, const QString& password);

private slots:
    void handleBackButton();
};

#endif //MAGAZYN_MANAGEREMPLOYEESVIEW_H
