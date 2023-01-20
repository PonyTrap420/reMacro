#include "passwordedit.h"

PasswordEdit::PasswordEdit(bool visibility, QWidget*parent)
{
    this->visibility = visibility;

    visible = QIcon(":/assets/icons/visible.svg");
    hidden = QIcon(":/assets/icons/hidden.svg");

    setEchoMode(EchoMode::Password);

    if(visibility){
        toggleaction = new QAction(visible, "", this);
        addAction(toggleaction, QLineEdit::TrailingPosition);
        connect(toggleaction, SIGNAL(triggered()), this, SLOT(Toggle()));

        shown = false;
    }
}

int PasswordEdit::Toggle(){
    if(!shown){
        setEchoMode(EchoMode::Normal);
        shown = true;
        toggleaction->setIcon(hidden);
    }
    else{
        setEchoMode(EchoMode::Password);
        shown = false;
        toggleaction->setIcon(visible);
    }
    return 1;
}
