#include <QLineEdit>
#include "Macro/task.h"
#include "Macro/sequence.h"

class KeyboardField : public QLineEdit
{
  Q_OBJECT
public:

  KeyboardField(QWidget *parent = 0, bool keybind = false);

  bool eventFilter(QObject* object, QEvent* event) override;

  inline void SetTask(Task* t){
      this->t = t;
  }

signals:
    void ChangedKeybind(int key);

private:
  Task* t = nullptr;
  bool keybind = false;
};
