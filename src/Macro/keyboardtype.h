#ifndef KEYBOARDTYPE_H
#define KEYBOARDTYPE_H

#include "task.h"

class KeyboardType : public Task
{
    public:
        HKL myKL = LoadKeyboardLayoutA("00000409", 0x00000001);
        QString text;

        inline KeyboardType(QString text, QString name, double delay, int repeats) : Task(name, delay, repeats, Text){
            this->text = text;
        }

        void Execute();

        QJsonObject Serialize();
        static Task* Deserialize(QVariantMap task_map);
    private:
        void PressKey(std::string key);

        std::string int_to_hex(int i);
};

#endif // KEYBOARDTYPE_H
