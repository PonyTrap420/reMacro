#ifndef CMDTASK_H
#define CMDTASK_H

#include "task.h"

class CmdTask : public Task
{
    public:
       QString cmd;

       inline CmdTask(QString cmd, QString name, double delay, int repeats) : Task(name, delay, repeats, Cmd){
           this->cmd = cmd;
       }

       void Execute();

       QJsonObject Serialize();
       static Task* Deserialize(QVariantMap task_map);

};

#endif // CMDTASK_H
