#include "ReceivingWorkerFunctionalTest.h"
#include "../../controllers/roles/ReceivingWorkerController.h"
#include "../../controllers/databases/ReceivingWorkerDatabase.h"
#include "../../widgets/MainWindow.h"

using IntIntMap = QMap<int,int>;
Q_DECLARE_METATYPE(IntIntMap)

ReceivingWorkerFunctionalTest::ReceivingWorkerFunctionalTest(QSqlDatabase& db)
        : db(db)
{
    qRegisterMetaType<IntIntMap>("IntIntMap");
}

void ReceivingWorkerFunctionalTest::testHandleConfirmOrder() {
    db.transaction();

    MainWindow mw;
    ReceivingWorkerDatabase dbCtrl(db);
    int employeeId = 1;
    int jobId = 1;
    int orderId = 1;

    ReceivingWorkerController controller(&mw, dbCtrl, employeeId, jobId);

    bool ok = QMetaObject::invokeMethod(
            &controller,
            "handleConfirmOrder",
            Qt::DirectConnection,
            Q_ARG(int, orderId)
    );
    QVERIFY(ok);

    QSqlQuery q(db);
    q.prepare("SELECT status FROM orders WHERE order_id = ?");
    q.addBindValue(orderId);
    QVERIFY(q.exec());
    QVERIFY(q.next());
    QCOMPARE(q.value(0).toString(), QString("wyslane"));

    db.rollback();
}

void ReceivingWorkerFunctionalTest::testHandleRemoveItem() {
    db.transaction();

    MainWindow mw;
    ReceivingWorkerDatabase dbCtrl(db);
    int employeeId = 1;
    int jobId = 1;
    int deliveryId = 1;
    int itemId = 1;

    ReceivingWorkerController controller(&mw, dbCtrl, employeeId, jobId);

    bool ok = QMetaObject::invokeMethod(
            &controller,
            "handleRemoveItem",
            Qt::DirectConnection,
            Q_ARG(int, deliveryId),
            Q_ARG(int, itemId)
    );
    QVERIFY(ok);

    QSqlQuery q(db);
    q.prepare("SELECT COUNT(*) FROM delivery_items WHERE delivery_id=? AND item_id=?");
    q.addBindValue(deliveryId);
    q.addBindValue(itemId);
    QVERIFY(q.exec());
    QVERIFY(q.next());
    QCOMPARE(q.value(0).toInt(), 0);

    // Sprawdzenie w magazynie
    q.prepare("SELECT quantity FROM items WHERE item_id=?");
    q.addBindValue(itemId);
    QVERIFY(q.exec());
    QVERIFY(q.next());
    QCOMPARE(q.value(0).toInt(), 0);

    db.rollback();
}

void ReceivingWorkerFunctionalTest::testHandleAddItem() {
    db.transaction();

    MainWindow mw;
    ReceivingWorkerDatabase dbCtrl(db);
    int employeeId = 1;
    int jobId = 1;
    int deliveryId = 1;
    int itemId = 2;

    ReceivingWorkerController controller(&mw, dbCtrl, employeeId, jobId);

    bool ok = QMetaObject::invokeMethod(
            &controller,
            "handleAddItem",
            Qt::DirectConnection,
            Q_ARG(int, deliveryId),
            Q_ARG(int, itemId)
    );
    QVERIFY(ok);

    QSqlQuery q(db);
    q.prepare("SELECT COUNT(*) FROM delivery_items WHERE delivery_id=? AND item_id=?");
    q.addBindValue(deliveryId);
    q.addBindValue(itemId);
    QVERIFY(q.exec());
    QVERIFY(q.next());
    QCOMPARE(q.value(0).toInt(), 1);

    db.rollback();
}

void ReceivingWorkerFunctionalTest::testHandleSubmitDelivery() {
    db.transaction();

    MainWindow mw;
    ReceivingWorkerDatabase dbCtrl(db);
    int employeeId = 1;
    int jobId = 1;
    int deliveryId = 1;

    IntIntMap quantities;
    quantities.insert(1, 5);    // item_id, quantity
    quantities.insert(4, 100);

    ReceivingWorkerController controller(&mw, dbCtrl, employeeId, jobId);

    bool ok = QMetaObject::invokeMethod(
            &controller,
            "handleSubmitDelivery",
            Qt::DirectConnection,
            Q_ARG(int, deliveryId),
            Q_ARG(IntIntMap, quantities)
    );
    QVERIFY(ok);

    QSqlQuery q(db);
    for (auto it = quantities.begin(); it != quantities.end(); ++it) {
        int itemId = it.key();
        int expected = it.value();

        q.prepare("SELECT quantity FROM items WHERE item_id=?");
        q.addBindValue(itemId);
        QVERIFY(q.exec());
        QVERIFY(q.next());
        QCOMPARE(q.value(0).toInt(), expected);
    }

    db.rollback();
}
