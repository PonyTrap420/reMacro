#include <QApplication>

#ifdef _WIN32
#include "QWinWidget.h"
#include "mainwindow.h"
#include "windows.h"
#else
#include "widget.h"
#endif

int main(int argc, char *argv[])
{
    //This has the app draw at HiDPI scaling on HiDPI displays, usually two pixels for every one logical pixel
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    //This has QPixmap images use the @2x images when available
    //See this bug for more details on how to get this right: https://bugreports.qt.io/browse/QTBUG-44486#comment-327410
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication app(argc, argv);

    app.setWindowIcon(QIcon(":/icon/icons/reMacro.ico"));
    app.setApplicationName("reMacro");

    //A common feature is to save your app's geometry on close such that you can draw in the same place on relaunch
    //Thus this project supports specifying the X/Y/Width/Height in a cross-platform manner
    int windowXPos, windowYPos, windowWidth, windowHeight;
    windowXPos = 100;
    windowYPos = 100;
    windowWidth = 1100;
    windowHeight = 600;

    //On Windows, the widget needs to be encapsulated in a native window for frameless rendering
    //In this case, QWinWidget #includes "Widget.h", creates it, and adds it to a layout
    QWinWidget w(&app);
    //MainWindow w;


    w.setGeometry(windowXPos, windowYPos, windowWidth, windowHeight);
    w.setWindowIcon(QIcon(":/icon/icons/reMacro.ico"));
    w.setWindowTitle("reMacro");
    w.show();

    return app.exec();
}
