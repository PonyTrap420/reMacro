#ifndef MACROMENUOPTIONS_H
#define MACROMENUOPTIONS_H

#include <QWidget>
#include "Macro/macro_frame.h"
#define _WIN32_WINNT 0x0600
#include "windows.h"
#include <shlobj.h>
#include "qfiledialog.h"
#include "Filters/OverlayWidget.h"


namespace Ui {
class MacroMenuOptions;
}

class MacroMenuOptions : public QWidget
{
    Q_OBJECT

public:
    MacroFrame* macroframe = nullptr;

    explicit MacroMenuOptions(QWidget *parent = nullptr);
    ~MacroMenuOptions();

    inline void SetMacroFrame(MacroFrame* macroframe){
        if(macroframe != nullptr)
            this->macroframe = macroframe;
    }

signals:
    void OpenKeybinds();
    void ExportMacro();

private slots:
    void on_deleteBtn_clicked();
    void on_exportBtn_clicked();
    void on_changename_clicked();

    void on_keybinds_clicked();

private:
    Ui::MacroMenuOptions *ui;
};

#endif // MACROMENUOPTIONS_H
