#include "keyboard_field.h"
#include <QDebug>
#include <QEvent>
#include "QLineEdit"
#include "QKeyEvent"
#include "Macro/keyboardpress.h"
#include "uiUtils.h"

KeyboardField::KeyboardField(QWidget *parent, bool keybind) : QLineEdit (parent)
{
  installEventFilter(this);
  parent->installEventFilter(this);
  this->keybind = keybind;
}

bool KeyboardField::eventFilter(QObject* object, QEvent* event)
{
    if(event->type() == QEvent::KeyPress)
    {
      this->clear();
      QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
      if(t!=nullptr){
          KeyboardPress* kp = dynamic_cast<KeyboardPress*>(t);
          kp->keystroke = static_cast<int>(keyEvent->nativeScanCode());
      }
      else if(keybind){
          if(keyEvent->key() == Qt::Key_Escape)
              return true;
          Sleep(100);
          qDebug("changed keybind");
          emit ChangedKeybind(static_cast<int>(keyEvent->nativeScanCode()));
      }
      if(keyEvent->text() == "" || keyEvent->key() == Qt::Key_Escape || keyEvent->key() == Qt::Key_Tab){
          this->setText("KeyBind - " + QString::number(static_cast<int>(keyEvent->nativeScanCode())));
      }
   }
    return QLineEdit::eventFilter(object, event);
}
