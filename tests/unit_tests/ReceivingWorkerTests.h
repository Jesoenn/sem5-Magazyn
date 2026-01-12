//
// Created by jakub on 2026-01-12.
//

#ifndef MAGAZYN_RECEIVINGWORKERTESTS_H
#define MAGAZYN_RECEIVINGWORKERTESTS_H


#include <QObject>
#include <QTest>


class ReceivingWorkerTests: public QObject {
Q_OBJECT

private slots:
    void testModifyOrder();
};

#endif //MAGAZYN_RECEIVINGWORKERTESTS_H
