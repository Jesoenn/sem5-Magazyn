#include "ReceivingWorkerDeliveryView.h"
#include <QMessageBox>
#include <QIntValidator>

ReceivingWorkerDeliveryView::ReceivingWorkerDeliveryView(QWidget* parent) : QWidget(parent) {
    mainLayout = new QVBoxLayout(this);

    setUpHeader();
    setUpScrollArea();
    QWidget* bottomWidget = new QWidget(this);
    QHBoxLayout* bottomLayout = new QHBoxLayout(bottomWidget);

    itemComboBox = new QComboBox(bottomWidget);      // nowy
    addItemButton = new QPushButton("Dodaj przedmiot", bottomWidget);
    submitButton = new QPushButton("Zatwierdź", bottomWidget);

    bottomLayout->addWidget(itemComboBox, 2, Qt::AlignLeft);
    bottomLayout->addWidget(addItemButton, 0, Qt::AlignLeft);
    bottomLayout->addWidget(submitButton, 0, Qt::AlignRight);

    bottomWidget->setLayout(bottomLayout);
    mainLayout->addWidget(bottomWidget);

    connect(addItemButton, &QPushButton::clicked, this, &ReceivingWorkerDeliveryView::handleAddItem);
    connect(submitButton, &QPushButton::clicked, this, &ReceivingWorkerDeliveryView::handleSubmitDelivery);
}

void ReceivingWorkerDeliveryView::setDeliveryInfo(int deliveryId, const QDateTime& creationDate) {
    deliveryInfoLabel->setText(
            QString("Dostawa ID: %1\nUtworzona: %2").arg(deliveryId).arg(creationDate.toString("dd.MM.yyyy HH:mm"))
    );
}

void ReceivingWorkerDeliveryView::addDeliveryItem(int itemId, const QString& itemName, int quantity) {
    QWidget* rowWidget = new QWidget(scrollWidget);
    QHBoxLayout* rowLayout = new QHBoxLayout(rowWidget);

    QLabel* idLabel = new QLabel(QString::number(itemId), rowWidget);
    QLabel* nameLabel = new QLabel(itemName, rowWidget);
    QLineEdit* qtyEdit = new QLineEdit(QString::number(quantity), rowWidget);
    QPushButton* removeBtn = new QPushButton("Usuń", rowWidget);

    // Validator: liczby > 0
    QIntValidator* validator = new QIntValidator(1, 1000000, qtyEdit);
    qtyEdit->setValidator(validator);

    rowLayout->addWidget(idLabel, 1, Qt::AlignLeft);
    rowLayout->addWidget(nameLabel, 3, Qt::AlignLeft);
    rowLayout->addWidget(qtyEdit, 1, Qt::AlignLeft);
    rowLayout->addWidget(removeBtn, 1, Qt::AlignLeft);

    rowWidget->setLayout(rowLayout);
    itemsLayout->addWidget(rowWidget);

    itemRows[itemId] = {idLabel, nameLabel, qtyEdit, removeBtn};

    connect(removeBtn, &QPushButton::clicked, [this, itemId]() {
        emit removeItem(deliveryId, itemId);
    });
}

void ReceivingWorkerDeliveryView::clearDeliveryItems() {
    while (itemsLayout->count() > 0) {
        QLayoutItem* item = itemsLayout->takeAt(0);
        if (item) {
            QWidget* widget = item->widget();
            if (widget) widget->deleteLater();
            delete item;
        }
    }
    itemRows.clear();
}

void ReceivingWorkerDeliveryView::setUpHeader() {
    QWidget* topWidget = new QWidget(this);
    QHBoxLayout* topLayout = new QHBoxLayout(topWidget);

    deliveryInfoLabel = new QLabel(topWidget);
    backButton = new QPushButton("Powrót", topWidget);

    topLayout->addWidget(deliveryInfoLabel, 1);
    topLayout->addWidget(backButton, 0, Qt::AlignRight);
    topWidget->setLayout(topLayout);

    mainLayout->addWidget(topWidget);

    connect(backButton, &QPushButton::clicked, this, &ReceivingWorkerDeliveryView::handleBackButton);
}

void ReceivingWorkerDeliveryView::setUpScrollArea() {
    scrollArea = new QScrollArea(this);
    scrollWidget = new QWidget(scrollArea);
    itemsLayout = new QVBoxLayout(scrollWidget);

    scrollWidget->setLayout(itemsLayout);
    scrollArea->setWidget(scrollWidget);
    scrollArea->setWidgetResizable(true);

    mainLayout->addWidget(scrollArea);
}

// -----------------------------------------------------
// Sloty
// -----------------------------------------------------

void ReceivingWorkerDeliveryView::handleBackButton() {
    emit backToMainMenu();
}

void ReceivingWorkerDeliveryView::handleAddItem() {
    if (itemComboBox->currentIndex() < 0) return;

    int itemId = itemComboBox->currentData().toInt();
    emit addItem(deliveryId, itemId);
}

void ReceivingWorkerDeliveryView::handleSubmitDelivery() {
    QMap<int,int> quantities;
    for (auto it = itemRows.begin(); it != itemRows.end(); ++it) {
        int qty = it.value().quantityLineEdit->text().toInt();
        if (qty <= 0) {
            QMessageBox::warning(this, "Błąd", "Wszystkie ilości muszą być większe od 0!");
            return;
        }
        quantities[it.key()] = qty;
    }
    emit submitDelivery(deliveryId, quantities);
}

void ReceivingWorkerDeliveryView::addAvailableItem(int itemId, const QString& name) {
    if (itemRows.contains(itemId))
        return;
    itemComboBox->addItem(name, itemId);
    addItemButton->setEnabled(itemComboBox->count() > 0);
}

void ReceivingWorkerDeliveryView::setDeliveryId(int id) {
    deliveryId = id;
}

int ReceivingWorkerDeliveryView::getDeliveryId() {
    return deliveryId;
}
