#include "looptask.h"
#include "sequence.h"

void LoopTask::Insert(Task* task){
    tasks.push_back(task);
}

void LoopTask::RemoveTask(Task* task){
    tasks.remove(task);
}

void LoopTask::Execute(){
    for(int i = 0; i < repeats; i++){
        for(Task *task : tasks){
                task->Execute();
            }
        }
}

QJsonObject LoopTask::Serialize(){
    QJsonObject task_obj;
    task_obj["name"] = name;
    task_obj["delay"] = delay;
    task_obj["repeats"] = repeats;
    task_obj["type"] = type;

    QJsonObject tasks_list;

    int i = 0;
    for(Task *task : tasks){
        QJsonObject task_obj;
        task_obj = task->Serialize();

        tasks_list.insert(QString::number(i),task_obj);
        i++;
    }

    task_obj.insert("list", tasks_list);

    return task_obj;
}

Task* LoopTask::Deserialize(QVariantMap task_map){
    return new LoopTask(task_map["name"].toString(), task_map["delay"].toDouble(), task_map["repeats"].toInt());
}
