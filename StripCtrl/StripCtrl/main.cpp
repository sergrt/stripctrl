#include "stdafx.h"
#include "StripCtrl.h"
#include <QtWidgets/QApplication>

void showConsole() {
    AllocConsole();
    /*FILE* pFileCon = */freopen("CONOUT$", "w", stdout);
    COORD coordInfo{ 130, 9000 };

    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coordInfo);
    SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), ENABLE_QUICK_EDIT_MODE | ENABLE_EXTENDED_FLAGS);
}

int main(int argc, char *argv[]) {
    showConsole();
    QApplication a(argc, argv);
    StripCtrl w;
    w.show();
    return a.exec();
}
