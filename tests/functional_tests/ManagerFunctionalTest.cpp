//
// Created by jakub on 2026-01-12.
//

#include "ManagerFunctionalTest.h"
#include "../../controllers/roles/ManagerController.h"
#include "../../controllers/databases/ManagerDatabase.h"
#include "../../widgets/MainWindow.h"

ManagerFunctionalTest::ManagerFunctionalTest(QSqlDatabase& db): db(db){
}

void ManagerFunctionalTest::testHandleSaveVehicle() {
    db.transaction();

    MainWindow mw;
    ManagerDatabase managerDb(db);
    int managerId = 1;
    int jobId = 1;

    ManagerController controller(&mw, db, managerDb, managerId, jobId);

    int vehicleId = 3;
    QString status = "zajety";
    int employeeId = -1;

    bool invoked = QMetaObject::invokeMethod(
            &controller,
            "handleSaveVehicle",
            Qt::DirectConnection,
            Q_ARG(int, vehicleId),
            Q_ARG(QString, status),
            Q_ARG(int, employeeId)
    );

    QVERIFY(invoked);

    QSqlQuery q(db);
    q.prepare("SELECT status, employee_id FROM vehicles WHERE vehicle_id=?");
    q.addBindValue(vehicleId);
    QVERIFY(q.exec());
    QVERIFY(q.next());
    QCOMPARE(q.value(0).toString(), status);
    QVERIFY(q.value(1).isNull());

    db.rollback();
}

void ManagerFunctionalTest::testHandleAddEmployee() {
    db.transaction();

    MainWindow mw;
    ManagerDatabase managerDb(db);
    int managerId = 1;
    int jobId = 1;

    ManagerController controller(&mw, db, managerDb, managerId, jobId);

    QString login = "test";
    QString firstName = "Test1";
    QString lastName = "Test2";
    int jobIdEmployee = 2;
    QString password = "123";

    bool invoked = QMetaObject::invokeMethod(
            &controller,
            "handleAddEmployee",
            Qt::DirectConnection,
            Q_ARG(QString, login),
            Q_ARG(QString, firstName),
            Q_ARG(QString, lastName),
            Q_ARG(int, jobIdEmployee),
            Q_ARG(QString, password)
    );

    QVERIFY(invoked);

    QSqlQuery q(db);
    q.prepare("SELECT login, first_name, last_name, job_id FROM employees WHERE login=?");
    q.addBindValue(login);
    QVERIFY(q.exec());
    QVERIFY(q.next());
    QCOMPARE(q.value(0).toString(), login);
    QCOMPARE(q.value(1).toString(), firstName);
    QCOMPARE(q.value(2).toString(), lastName);
    QCOMPARE(q.value(3).toInt(), jobIdEmployee);

    db.rollback();
}

void ManagerFunctionalTest::testHandleAssignEmployee() {
    db.transaction();

    MainWindow mw;
    ManagerDatabase managerDb(db);
    int managerId = 1;
    int jobId = 1;

    ManagerController controller(&mw, db, managerDb, managerId, jobId);

    int orderId = 1;
    int employeeId = 1;

    bool invoked = QMetaObject::invokeMethod(
            &controller,
            "handleAssignEmployee",
            Qt::DirectConnection,
            Q_ARG(int, orderId),
            Q_ARG(int, employeeId)
    );

    QVERIFY(invoked);

    // Sprawdzenie w DB
    QSqlQuery q(db);
    q.prepare("SELECT assigned_employee_id FROM orders WHERE order_id=?");
    q.addBindValue(orderId);
    QVERIFY(q.exec());
    QVERIFY(q.next());
    QCOMPARE(q.value(0).toInt(), employeeId);

    db.rollback();
}

void ManagerFunctionalTest::testHandleCreateOrder() {
    db.transaction();

    MainWindow mw;
    ManagerDatabase managerDb(db);
    int managerId = 4;
    int jobId = 1;

    ManagerController controller(&mw, db, managerDb, managerId, jobId);

    int assignedEmployeeId = 1;

    bool invoked = QMetaObject::invokeMethod(
            &controller,
            "handleCreateOrder",
            Qt::DirectConnection,
            Q_ARG(int, assignedEmployeeId)
    );

    QVERIFY(invoked);


    QSqlQuery q(db);
    q.prepare("SELECT COUNT(*) FROM orders WHERE assigned_employee_id = 1 and created_by_id = 4");
    QVERIFY(q.exec());
    QVERIFY(q.next());
    QCOMPARE(q.value(0).toInt(), 1);

    db.rollback();
}

void ManagerFunctionalTest::testHandleAddOrderItem() {
    db.transaction();

    MainWindow mw;
    ManagerDatabase managerDb(db);
    int managerId = 1;
    int jobId = 1;

    ManagerController controller(&mw, db, managerDb, managerId, jobId);

    int orderId = 1;
    int itemId = 3;
    int quantity = 5;

    bool invoked = QMetaObject::invokeMethod(
            &controller,
            "handleAddOrderItem",
            Qt::DirectConnection,
            Q_ARG(int, orderId),
            Q_ARG(int, itemId),
            Q_ARG(int, quantity)
    );

    QVERIFY(invoked);

    QSqlQuery q(db);
    q.prepare("SELECT quantity FROM order_items WHERE order_id=? AND item_id=?");
    q.addBindValue(orderId);
    q.addBindValue(itemId);
    QVERIFY(q.exec());
    QVERIFY(q.next());
    QCOMPARE(q.value(0).toInt(), quantity);

    db.rollback();
}
