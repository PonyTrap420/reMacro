#ifndef PASSWORDEDIT_H
#define PASSWORDEDIT_H


#include <QLineEdit>
#include <QIcon>
#include "QAction"

class PasswordEdit : public QLineEdit
{
  Q_OBJECT
public:

  PasswordEdit(bool visibility = true, QWidget *parent = 0);


private slots:
   int Toggle();

private:
   bool visibility;
   QIcon visible;
   QIcon hidden;

   bool shown;
   QAction* toggleaction;


};

#endif // PASSWORDEDIT_H
