//
// Created by jakub on 2025-12-06.
//

#include <QFile>
#include <QDir>
#include <QApplication>
#include "MainWindow.h"

MainWindow::MainWindow(QWidget* parent): QMainWindow(parent){
    stack = new QStackedWidget();
    setCentralWidget(stack);
}

void MainWindow::showView(QWidget *widget) {
    stack->setCurrentWidget(widget);
}

void MainWindow::addView(QWidget* view) {
    stack->addWidget(view);            // stack zarządza pamięcią widoku
}

void MainWindow::loadStyle() {
    QFile file(stylePath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "Nie udalo sie otworzyc .qss" << stylePath;
        return;
    }

    QString styleSheet = file.readAll();
    qApp->setStyleSheet(styleSheet);
    qDebug() << "Styl zaladowany: " << stylePath;
}

