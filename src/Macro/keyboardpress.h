#ifndef KEYBOARDPRESS_H
#define KEYBOARDPRESS_H

#include "task.h"

class KeyboardPress : public Task
{
    public:
        HKL myKL = LoadKeyboardLayoutA("00000409", 0x00000001);
        int keystroke;
        inline KeyboardPress(int keystroke, QString name, double delay, int repeats) : Task(name, delay, repeats, Keyboard){
           this->keystroke = keystroke;
        }

        void SetKeystroke(int keystroke);

        void Execute();
        void HoldKey();

        QJsonObject Serialize();
        static Task* Deserialize(QVariantMap task_map);
    private:
        void PressKey();
};

#endif // KEYBOARDPRESS_H
