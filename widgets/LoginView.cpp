//
// Created by jakub on 2025-12-05.
//

#include "LoginView.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>

LoginView::LoginView(QWidget* parent) : QWidget(parent)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QFormLayout* formLayout = new QFormLayout();

    QLineEdit* usernameEdit = new QLineEdit(this);
    QLineEdit* passwordEdit = new QLineEdit(this);
    passwordEdit->setEchoMode(QLineEdit::Password);

    formLayout->addRow("Login:", usernameEdit);
    formLayout->addRow("Hasło:", passwordEdit);

    QPushButton* loginButton = new QPushButton("Zaloguj", this);

    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(loginButton);

    connect(loginButton, &QPushButton::clicked, this, [this, usernameEdit, passwordEdit]() {
        onLoginButtonClicked(usernameEdit, passwordEdit);
    });
}

void LoginView::onLoginButtonClicked(QLineEdit* usernameEdit, QLineEdit* passwordEdit)
{
    emit loginRequested(usernameEdit->text(), passwordEdit->text());
}

void LoginView::loginFailed() {
    QMessageBox::critical(nullptr, "Logowanie", "Nieprawidłowy login lub hasło!");
}
