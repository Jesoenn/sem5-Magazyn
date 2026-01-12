//
// Created by jakub on 2026-01-12.
//

#include "ReceivingWorkerTests.h"
#include "../../widgets/warehouseworker/WarehouseWorkerOrderView.h"
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTest>

void ReceivingWorkerTests::testModifyOrder() {
    WarehouseWorkerOrderView view;
    view.setOrderInfo(10, "2026-01-12 10:12:10", "Jan Kowalski");

    view.addOrderItem(301, "Młotek", 5, 0, 10);
    view.addOrderItem(302, "Śrubki", 10, 0, 20);


    QMap<int, bool> signalEmitted;
    signalEmitted[301] = false;
    signalEmitted[302] = false;

    QObject::connect(&view, &WarehouseWorkerOrderView::submitOrder,
                     [&](int orderId, QMap<int,int> pickedQuantities){
                         QCOMPARE(orderId, 10);

                         QCOMPARE(pickedQuantities[301], 3);
                         QCOMPARE(pickedQuantities[302], 7);

                         signalEmitted[301] = true;
                         signalEmitted[302] = true;
                     });


    QList<QWidget*> rows = view.findChildren<QWidget*>();
    for (QWidget* row : rows) {
        QLineEdit* pickedEdit = row->findChild<QLineEdit*>();
        QLabel* nameLabel = row->findChild<QLabel*>();
        if (!pickedEdit || !nameLabel) continue;

        QString name = nameLabel->text();
        if (name == "Młotek") {
            pickedEdit->setText("3");
        } else if (name == "Śrubki") {
            pickedEdit->setText("7");
        }
    }

    QPushButton* submitButton = view.findChild<QPushButton*>("", Qt::FindDirectChildrenOnly);
    QList<QPushButton*> buttons = view.findChildren<QPushButton*>();
    for (QPushButton* btn : buttons) {
        if (btn->text() == "Zatwierdź") {
            submitButton = btn;
            break;
        }
    }

    QTest::mouseClick(submitButton, Qt::LeftButton);

    QVERIFY(signalEmitted[301]);
    QVERIFY(signalEmitted[302]);
}
