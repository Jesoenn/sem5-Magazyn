#include <QApplication>
#include "controllers/AppController.h"

/*
 SCREENY!!!!!

 Pamietac!
- AppController zna kazdy widokw aplikacji
- Widok na ogol zna wylacznie sam siebie

 Przelaczanie widokow: QStackedWidget

 W widoku jakims moge stworzyc QScrollArea jak jest duzy

 Etapy tworzenia w widoku (widok extends QWidget):
 1. Tworze layout
 2. Przypisuje przyciski i tekst do layoutu
 3. w widoku wpisuje: setLayout(obiektlayout)

 Layouty:
 - QVBoxLayout -> jeden widget pod drugim
 - QHBoxLayout -> poziomo - jeden obok drugiego
 - QGridLayout -> Wiersze i kolumny!!
 - QFormLayout -> TEKST | PRZYCISK


 TODO: Logika aplikacji:
 - AppController wywoluje logowanie, ma jako zmienna MainWindow
 - Po zalogowaniu przydziela programowi odpowiedni widok i daje wskaznik na MainWindow
 - Reszte robi rolesController

 TODO: Pasek na gorze:
 - Po zalogowaniu do mainWindow odpowiedni controller przekazuje jak bedzie wygladal pasek
 -
*/

// slots w odpowiedzi na signal
// signal - cos sie dzije w qt
// emit -> emitowanie sygnalu

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    AppController controller;
    controller.start();

    return app.exec();
}


