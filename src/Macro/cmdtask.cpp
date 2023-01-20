#include "cmdtask.h"

void CmdTask::Execute(){
   for(int i = 0; i < repeats; i++){
       Sleep(delay*1000);
       ShellExecuteA(0, "open", "cmd.exe", "/C "+cmd.toLocal8Bit(), 0, SW_HIDE);
   }
}

QJsonObject CmdTask::Serialize(){
    QJsonObject task_obj;
    task_obj["name"] = name;
    task_obj["delay"] = delay;
    task_obj["repeats"] = repeats;
    task_obj["type"] = type;
    task_obj["cmd"] = cmd;

    return task_obj;
}

Task* CmdTask::Deserialize(QVariantMap task_map){
    return new CmdTask(task_map["cmd"].toString(), task_map["name"].toString(), task_map["delay"].toDouble(), task_map["repeats"].toInt());
}
