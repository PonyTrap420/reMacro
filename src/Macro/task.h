#ifndef TASK_H
#define TASK_H

#include <iomanip>
#include <iostream>
#include <sstream>
#include <QString>
#include "QJsonObject"

#define WINVER 0x0500

#include "windows.h"
#include "memory"

enum TaskType{
    Mouse,
    Keyboard,
    Text,
    Loop,
    Cmd
};
static const char * TaskNames[] = { "Mouse", "Keyboard", "Text", "Loop", "Cmd"};

class Task
{
    public:  
        QString name;
        double delay;
        int repeats;
        TaskType type;

        inline Task(QString name, double delay, int repeats, TaskType type){
            this->name = name;
            this->delay = delay;
            this->repeats = repeats;
            this->type = type;
        }

        Task();
        inline virtual ~Task(){}

        virtual void Execute() = 0;
        virtual QJsonObject Serialize() = 0;

        inline std::string GetTaskName()
        {
          return std::string(TaskNames[type]);
        }

        void SetDelay(double delay);
        void SetName(QString name);
        void SetRepeats(int repeats);

};

#endif // TASK_H
