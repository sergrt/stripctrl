#include "stdafx.h"
#include "StripCtrl.h"
#include <QtWidgets/QApplication>



void Console() {
    AllocConsole();
    FILE *pFileCon = NULL;
    pFileCon = freopen("CONOUT$", "w", stdout);

    COORD coordInfo;
    coordInfo.X = 130;
    coordInfo.Y = 9000;

    //SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coordInfo);
    //SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), ENABLE_QUICK_EDIT_MODE | ENABLE_EXTENDED_FLAGS);
}

int main(int argc, char *argv[])
{

    Console();
    QApplication a(argc, argv);
    StripCtrl w;
    w.show();
    return a.exec();
}
