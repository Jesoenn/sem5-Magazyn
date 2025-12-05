#include <QApplication>
#include <QPushButton>
#include <iostream>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QPushButton button("Magazyn :DDD!", nullptr);
    button.resize(200, 100);
    button.show();
    return QApplication::exec();
}