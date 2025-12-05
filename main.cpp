#include <QApplication>
#include <QPushButton>
#include <QtSql>
#include <QtCore>
#include <QDebug>
#include <iostream>
#include <QMessageBox>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QPushButton button("Magazyn ĄŁął :DDD!", nullptr);
    button.resize(200, 100);
    button.show();

    // Rejestracja sterownika MySQL
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");

    // Dane połączenia
    db.setHostName("localhost");
    db.setPort(3306);
    db.setDatabaseName("magazyn");
    db.setUserName("root");
    db.setPassword("root");

    // Próba połączenia
    if (!db.open()) {
        QMessageBox::critical(nullptr, "Błąd", "Nie można połączyć z bazą!\n" +
                                               db.lastError().text());
        return -1;
    }

    // --- Zapytanie SELECT ---
    QSqlQuery query(db);

    if (!query.exec("SELECT * FROM items;")) {
        QMessageBox::critical(nullptr, "Błąd zapytania",
                              "Zapytanie nie powiodło się:\n" + query.lastError().text());
        return -1;
    }

    // --- Budujemy tekst do wyświetlenia ---
    QString result;
    QSqlRecord rec = query.record();

    // Nagłówki kolumn
    for (int i = 0; i < rec.count(); i++) {
        result += rec.fieldName(i) + "\t";
    }
    result += "\n----------------------------------------\n";

    // Wiersze
    while (query.next()) {
        for (int i = 0; i < rec.count(); i++) {
            result += query.value(i).toString() + "\t";
        }
        result += "\n";
    }

    // --- Wyświetlenie wyników w okienku ---
    QMessageBox::information(nullptr, "Wyniki SELECT", result);

    return QApplication::exec();
}
