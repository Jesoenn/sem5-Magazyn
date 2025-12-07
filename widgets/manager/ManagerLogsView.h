//
// Created by jakub on 2025-12-07.
//

#ifndef MAGAZYN_MANAGERLOGSVIEW_H
#define MAGAZYN_MANAGERLOGSVIEW_H

#include <QWidget>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

class ManagerLogsView : public QWidget {
Q_OBJECT
private:
    QVBoxLayout* mainLayout;
    QHBoxLayout* headerLayout;
    QPushButton* backButton;

    QScrollArea* scrollArea;
    QWidget* scrollWidget;
    QVBoxLayout* logsLayout;

    void setUp();

public:
    explicit ManagerLogsView(QWidget* parent = nullptr);

    void addLogEntry(const QString& event, const QString& creator);
    void clearLogs();

signals:
    void backToMainMenu();
};

#endif //MAGAZYN_MANAGERLOGSVIEW_H
