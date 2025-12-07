//
// Created by jakub on 2025-12-07.
//

#include "ManagerLogsView.h"

ManagerLogsView::ManagerLogsView(QWidget* parent) : QWidget(parent) {
    setUp();
}

void ManagerLogsView::setUp() {
    mainLayout = new QVBoxLayout(this);

    headerLayout = new QHBoxLayout();
    QLabel* titleLabel = new QLabel("Dziennik zdarzeń", this);
    backButton = new QPushButton("Powrót", this);

    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();  // wypycha przycisk w prawy róg
    headerLayout->addWidget(backButton);
    mainLayout->addLayout(headerLayout);
    scrollArea = new QScrollArea(this);
    scrollWidget = new QWidget(scrollArea);
    logsLayout = new QVBoxLayout(scrollWidget);
    scrollWidget->setLayout(logsLayout);
    scrollArea->setWidget(scrollWidget);
    scrollArea->setWidgetResizable(true);
    mainLayout->addWidget(scrollArea);

    connect(backButton, &QPushButton::clicked, this, &ManagerLogsView::backToMainMenu);
}

void ManagerLogsView::addLogEntry(const QString& event, const QString& creator) {
    QWidget* rowWidget = new QWidget(scrollWidget);
    QHBoxLayout* rowLayout = new QHBoxLayout(rowWidget);

    QLabel* eventLabel = new QLabel(event, rowWidget);
    QLabel* creatorLabel = new QLabel(creator, rowWidget);

    rowLayout->addWidget(eventLabel, 3);
    rowLayout->addWidget(creatorLabel, 1);

    rowWidget->setLayout(rowLayout);
    logsLayout->addWidget(rowWidget);
}

void ManagerLogsView::clearLogs() {
    while (logsLayout->count() > 0) {
        QLayoutItem* item = logsLayout->takeAt(0);
        if (item) {
            QWidget* widget = item->widget();
            if (widget) widget->deleteLater();
            delete item;
        }
    }
}
