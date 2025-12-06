//
// Created by jakub on 2025-12-06.
//

#ifndef MAGAZYN_MAINWINDOW_H
#define MAGAZYN_MAINWINDOW_H
#include <QMainWindow>
#include <QStackedWidget>


class MainWindow: public QMainWindow {
    Q_OBJECT
private:
    const QString stylePath=R"(C:\Users\jakub\CLionProjects\Magazyn\widgets\styles\style.qss)";
    QStackedWidget* stack;

public:
    MainWindow(QWidget* parent = nullptr);
    void showView(QWidget* widget);
    void addView(QWidget* widget);
    void loadStyle();
};


#endif //MAGAZYN_MAINWINDOW_H
