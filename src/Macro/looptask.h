#ifndef LOOPTASK_H
#define LOOPTASK_H

#include "task.h"
#include "list"



using std::list;

class LoopTask : public Task
{
    public:
       list<Task*> tasks;

       inline LoopTask(QString name, double delay, int repeats) : Task(name, delay, repeats, Loop){
       }

       void Execute();
       QJsonObject Serialize();
       static Task* Deserialize(QVariantMap loop_task_map);

       void Insert(Task* task);
       void RemoveTask(Task* task);


};
#endif // LOOPTASK_H
