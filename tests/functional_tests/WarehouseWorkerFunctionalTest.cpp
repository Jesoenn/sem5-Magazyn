#include "WarehouseWorkerFunctionalTest.h"

#include "../../controllers/roles/WarehouseWorkerController.h"
#include "../../controllers/databases/WarehouseWorkerDatabase.h"
#include "../../widgets/MainWindow.h"

using IntIntMap = QMap<int, int>;
Q_DECLARE_METATYPE(IntIntMap)

WarehouseWorkerFunctionalTest::WarehouseWorkerFunctionalTest(QSqlDatabase& db)
        : db(db)
{
    qRegisterMetaType<IntIntMap>("IntIntMap");
}

void WarehouseWorkerFunctionalTest::testHandleOrderUpdate() {
    db.transaction();

    MainWindow mw;
    WarehouseWorkerDatabase wwDb(db);

    int employeeId = 1;
    int jobId = 1;
    int orderId = 1;

    WarehouseWorkerController controller(&mw, wwDb, employeeId, jobId);

    IntIntMap picked;
    picked.insert(1, 1);    // ItemId, Quantity

    bool ok = QMetaObject::invokeMethod(
            &controller,
            "handleOrderUpdate",
            Qt::DirectConnection,
            Q_ARG(int, orderId),
            Q_ARG(IntIntMap, picked)
    );

    QVERIFY(ok);

    QSqlQuery q(db);
    q.exec("SELECT picked_quantity FROM order_items WHERE order_id=1 AND item_id=1");
    QVERIFY(q.next());
    QCOMPARE(q.value(0).toInt(), 1);

    db.rollback();
}

void WarehouseWorkerFunctionalTest::testHandleFreeVehicle() {
    db.transaction();

    MainWindow mw;
    WarehouseWorkerDatabase wwDb(db);

    int employeeId = 1;
    int jobId = 1;

    WarehouseWorkerController controller(&mw, wwDb, employeeId, jobId);

    bool ok = QMetaObject::invokeMethod(
            &controller,
            "handleFreeVehicle",
            Qt::DirectConnection
    );

    QVERIFY(ok);

    QSqlQuery verifyQuery(db);
    verifyQuery.prepare("SELECT employee_id FROM vehicles WHERE vehicle_id = 1");
    verifyQuery.exec();
    QVERIFY(verifyQuery.next());
    QVERIFY(verifyQuery.value(0).isNull());

    db.rollback();
}


