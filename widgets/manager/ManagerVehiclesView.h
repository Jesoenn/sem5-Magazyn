#ifndef MAGAZYN_MANAGERVEHICLESVIEW_H
#define MAGAZYN_MANAGERVEHICLESVIEW_H

#include <QWidget>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QMap>

class ManagerVehiclesView : public QWidget {
Q_OBJECT
private:
    QPushButton* backButton;
    QLabel* headerLabel;

    QScrollArea* scrollArea;
    QWidget* scrollWidget;
    QVBoxLayout* vehiclesLayout;

    struct VehicleRow {
        QLabel* vehicleIdLabel;
        QComboBox* statusComboBox;
        QComboBox* employeeComboBox;
        QPushButton* saveButton;
    };

    QMap<int, VehicleRow> vehicleRows; // vehicle_id -> row widgets
    QMap<int, QString> employeeIdToName; // employee_id -> name

    void setUpHeader();
    void setUpScrollArea();

public:
    explicit ManagerVehiclesView(QWidget* parent = nullptr);

    // employeeMap: id -> imię i nazwisko
    void setEmployeeMap(const QMap<int, QString>& employeeMap);
    void addVehicleRow(int vehicleId, const QString& status, int assignedEmployeeId = -1);
    void clearVehicles();

signals:
    void backToMainMenu();
    void saveVehicle(int vehicleId, const QString& status, int employeeId);

private slots:
    void handleBackButton();
};

#endif //MAGAZYN_MANAGERVEHICLESVIEW_H
