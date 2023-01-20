#ifndef MOUSECLICK_H
#define MOUSECLICK_H

#include "task.h"
#include "map"


using namespace std;

class MouseClick : public Task
{
    public:
       int x;
       int y;
       int keystroke;

       inline MouseClick(int x, int y, int keystroke, QString name, double delay, int repeats) : Task(name, delay, repeats, Mouse){
           this->x = x;
           this->y = y;
           this->keystroke = keystroke;
       }
       MouseClick();

       void Execute();
       QJsonObject Serialize();
       static Task* Deserialize(QVariantMap task_map);

       void SetMouse(int x, int y);
       void SetKeystroke(int keystroke);

    private:
       void MoveMouse(int x, int y);
       void ClickMouse();
       POINT WaitForMouseClick();

};

#endif // MOUSECLICK_H
