//
// Created by jakub on 2026-01-12.
//

#ifndef MAGAZYN_RECEIVINGWORKERFUNCTIONALTEST_H
#define MAGAZYN_RECEIVINGWORKERFUNCTIONALTEST_H

#include <QtTest>
#include <QSqlDatabase>

class ReceivingWorkerFunctionalTest : public QObject {
Q_OBJECT

public:
    explicit ReceivingWorkerFunctionalTest(QSqlDatabase& db);

private slots:
    void testHandleConfirmOrder();  // orderId -> czy zmieni sie status zamowienia na 'potwierdzone'
    void testHandleRemoveItem(); // deliveryId, int itemId -> sprawdzenie czy przedmiot usunie sie ze stanu magazynu
    void testHandleAddItem(); // deliveryId, int itemId -> sprawdzenie czy przedmiot doda sie do dostawy
    void testHandleSubmitDelivery(); // deliveryId, map<itemId, quantity> -> sprawdzenie czy dostawa zostanie zatwierdzona i przedmioty pojawia sie w magazynie


private:
    QSqlDatabase& db;
};


#endif //MAGAZYN_RECEIVINGWORKERFUNCTIONALTEST_H
