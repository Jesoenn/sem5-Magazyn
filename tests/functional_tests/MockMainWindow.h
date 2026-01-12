//
// Created by jakub on 2026-01-12.
//

#ifndef MAGAZYN_MOCKMAINWINDOW_H
#define MAGAZYN_MOCKMAINWINDOW_H


#include "../../widgets/MainWindow.h"

class MockMainWindow : public MainWindow {
public:
    void showView(QWidget* widget) { }
    void addView(QWidget* widget) { }
    void removeView(QWidget* widget) { }
};



#endif //MAGAZYN_MOCKMAINWINDOW_H
