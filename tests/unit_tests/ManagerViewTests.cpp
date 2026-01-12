//
// Created by jakub on 2026-01-12.
//

#include "ManagerViewTests.h"
#include "../../widgets/manager/ManagerVehiclesView.h"
#include "../../widgets/manager/ManagerOrderView.h"
#include "../../widgets/manager/ManagerEmployeesView.h"
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>

void ManagerViewTests::testAddEmployee() {
    ManagerEmployeesView view;

    QMap<int, QString> jobs;
    jobs[1] = "Magazynier";
    jobs[2] = "Kierownik";
    view.setJobMap(jobs);

    view.show();

    QLineEdit* loginEdit = view.findChild<QLineEdit*>("newLoginEdit");
    QLineEdit* firstNameEdit = view.findChild<QLineEdit*>("newFirstNameEdit");
    QLineEdit* lastNameEdit = view.findChild<QLineEdit*>("newLastNameEdit");
    QLineEdit* passwordEdit = view.findChild<QLineEdit*>("newPasswordEdit");
    QComboBox* jobCombo = view.findChild<QComboBox*>("newJobCombo");
    QPushButton* addButton = view.findChild<QPushButton*>("addButton");

    QVERIFY(loginEdit != nullptr);
    QVERIFY(firstNameEdit != nullptr);
    QVERIFY(lastNameEdit != nullptr);
    QVERIFY(passwordEdit != nullptr);
    QVERIFY(jobCombo != nullptr);
    QVERIFY(addButton != nullptr);

    QString login = "testuser";
    QString firstName = "Jan";
    QString lastName = "Kowalski";
    QString password = "tajne";

    bool signalEmitted = false;
    QObject::connect(&view, &ManagerEmployeesView::addEmployee,
                     [&](const QString& l, const QString& f, const QString& s, int jobId, const QString& pass) {
                         QCOMPARE(l, login);
                         QCOMPARE(f, firstName);
                         QCOMPARE(s, lastName);
                         QCOMPARE(jobId, 2);
                         QCOMPARE(pass, password);
                         signalEmitted = true;
                     });

    QTest::keyClicks(loginEdit, login);
    QTest::keyClicks(firstNameEdit, firstName);
    QTest::keyClicks(lastNameEdit, lastName);
    QTest::keyClicks(passwordEdit, password);
    jobCombo->setCurrentIndex(1);
    QTest::mouseClick(addButton, Qt::LeftButton);

    QVERIFY(signalEmitted);
}

void ManagerViewTests::testSaveVehicle() {
    ManagerVehiclesView view;

    QMap<int, QString> employees;
    employees[1] = "Jan Kowalski";
    employees[2] = "Anna Nowak";
    view.setEmployeeMap(employees);

    view.show();
    view.addVehicleRow(101, "wolny", 1);  // id 101, wolny, Jan Kowalski
    view.addVehicleRow(102, "zajety", 2); // id 102, zajęty, Anna Nowak

    QMap<int, bool> signalEmitted;
    signalEmitted[101] = false;
    signalEmitted[102] = false;

    QObject::connect(&view, &ManagerVehiclesView::saveVehicle,
                     [&](int vehicleId, const QString& status, int employeeId){
                         if(vehicleId == 101){
                             QCOMPARE(status, QString("zajety"));
                             QCOMPARE(employeeId, 2);
                             signalEmitted[101] = true;
                         }
                         if(vehicleId == 102){
                             QCOMPARE(status, QString("wolny"));
                             QCOMPARE(employeeId, 1);
                             signalEmitted[102] = true;
                         }
                     });


    QList<QPushButton*> saveButtons = view.findChildren<QPushButton*>();
    for (QPushButton* btn : saveButtons) {
        if (btn->text() != "Zapisz") continue;

        QWidget* rowWidget = btn->parentWidget();
        QComboBox* statusCombo = rowWidget->findChild<QComboBox*>();
        QComboBox* employeeCombo = rowWidget->findChildren<QComboBox*>().last();

        QLabel* idLabel = rowWidget->findChild<QLabel*>();
        int vehicleId = idLabel->text().toInt();

        if(vehicleId == 101){
            statusCombo->setCurrentText("zajety");
            employeeCombo->setCurrentIndex(2); // Anna Nowak
        } else if(vehicleId == 102){
            statusCombo->setCurrentText("wolny");
            employeeCombo->setCurrentIndex(1); // Jan Kowalski
        }

        QTest::mouseClick(btn, Qt::LeftButton);
    }

    QVERIFY(signalEmitted[101]);
    QVERIFY(signalEmitted[102]);
}

void ManagerViewTests::testModifyOrderItem() {
    ManagerOrderView view;

    view.setOrderInfo(1, "Jan Kowalski", "2026-01-12");
    view.addOrderItemRow(201, 1, "Młotek", 5);
    view.addOrderItemRow(202, 2, "Śrubki", 10);

    QMap<int, bool> signalEmitted;
    signalEmitted[201] = false;
    signalEmitted[202] = false;

    QObject::connect(&view, &ManagerOrderView::modifyOrderItem,
                     [&](int orderId, int orderItemId, int newQuantity){
                         QCOMPARE(orderId, 1);
                         if(orderItemId == 201){
                             QCOMPARE(newQuantity, 8);
                             signalEmitted[201] = true;
                         }
                         if(orderItemId == 202){
                             QCOMPARE(newQuantity, 15);
                             signalEmitted[202] = true;
                         }
                     });


    QList<QPushButton*> modifyButtons = view.findChildren<QPushButton*>("");
    for(QPushButton* btn : modifyButtons){
        if(btn->text() != "Modyfikuj") continue;

        QWidget* rowWidget = btn->parentWidget();
        QLineEdit* quantityEdit = rowWidget->findChild<QLineEdit*>();
        QLabel* itemLabel = rowWidget->findChild<QLabel*>();
        QString itemName = itemLabel->text();

        if(itemName == "Młotek"){
            quantityEdit->setText("8");
        } else if(itemName == "Śrubki"){
            quantityEdit->setText("15");
        }

        QTest::mouseClick(btn, Qt::LeftButton);
    }

    QVERIFY(signalEmitted[201]);
    QVERIFY(signalEmitted[202]);
}
