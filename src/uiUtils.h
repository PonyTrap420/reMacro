#ifndef UIUTILS_H
#define UIUTILS_H

#include "Macro/macro_frame.h"
#include "ui_mainwindow.h"
#include "Macro/sequence.h"
#include "Macro/macro_frame.h"
#include "Macro/taskframe.h"
#include "QPixmap"
#include "Macro/task.h"

QT_BEGIN_NAMESPACE
namespace Util{

    void ToggleBtn(QPushButton* btn);
    void UpdateButtonIcon(QPushButton* btn, bool pressed, int index);
    void ResetButton(QLabel* label, QPushButton* btn, int index);
    void NavbarBtnPressed(Ui::MainWindow* ui, int index);
    void clearWidgets(QLayout * layout);
    void clearSpacers(QLayout * layout);
    void HandleTaskProp(Ui::MainWindow* ui, Task* task, bool readOnly = false);
    bool ValidateInput(QLineEdit* le, QLabel* err, QRegularExpression* rx);
    bool InputCheck(Ui::MainWindow* ui);
    bool InputCheckLogin(Ui::MainWindow* ui);
    void ResetErrors(Ui::MainWindow* ui);
    QIcon RoundImage(QByteArray b, QSize s, QString type);
    void AddMarketMacro(QString name, QIcon);
    uint ScanToVirtual(int scan);
    char ScanToChar(int scan);
}
QT_END_NAMESPACE

#endif // UIUTILS_H
