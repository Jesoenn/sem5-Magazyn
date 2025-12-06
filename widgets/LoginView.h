//
// Created by jakub on 2025-12-05.
//

#ifndef MAGAZYN_LOGINVIEW_H
#define MAGAZYN_LOGINVIEW_H


#include <QWidget>
#include <QLineEdit>

class LoginView : public QWidget {
    Q_OBJECT
private:
    void onLoginButtonClicked(QLineEdit* usernameEdit, QLineEdit* passwordEdit);
public:
    LoginView(QWidget* parent = nullptr);
    void loginFailed();

    signals:
        void loginRequested(QString user, QString password);
};


#endif //MAGAZYN_LOGINVIEW_H
