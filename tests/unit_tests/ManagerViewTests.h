//
// Created by jakub on 2026-01-12.
//

#ifndef MAGAZYN_MANAGERVIEWTESTS_H
#define MAGAZYN_MANAGERVIEWTESTS_H

#include <QObject>
#include <QTest>


class ManagerViewTests: public QObject {
Q_OBJECT

private slots:
    void testAddEmployee(); // test dodania pracownika UI
    void testSaveVehicle(); // test modyfikacji przypisania pojazdu
    void testModifyOrderItem(); // test modyfikacji zamowienia
};




#endif //MAGAZYN_MANAGERVIEWTESTS_H

