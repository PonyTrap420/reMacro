#include "sequence.h"
#include "thread"
#include "iostream"
#include "mouseclick.h"
#include "keyboardpress.h"
#include "keyboardtype.h"
#include "cmdtask.h"
#include "looptask.h"
#include "../uiUtils.h"

using namespace std;

QPushButton* Sequence::stopBtn = nullptr;

Sequence::Sequence(list<Task*> tasks, int keybind, int method)
{
    this->tasks = tasks;
    this->keybind = keybind;
    this->method = method;

    qDebug()<<this->keybind;
}

Sequence::Sequence(int keybind, int method){
    this->keybind = keybind;
    this->method = method;
}

int Sequence::GetKeybind(){
    return keybind;
}

int Sequence::GetMethod(){
    return method;
}

bool Sequence::IsRunning(){
    return running;
}

void Sequence::SetKeybind(int keybind){
    this->keybind = keybind;
}

void Sequence::SetMethod(int method){
    this->method = method;
}

QJsonObject Sequence::Serialize(){
    QJsonObject sequence_obj;
    sequence_obj["keybind"] = keybind;
    sequence_obj["method"] = method;
    QJsonObject tasks_obj;

    int i = 0;
    for(Task *task : tasks){
        QJsonObject task_obj;
        task_obj = task->Serialize();

        tasks_obj.insert(QString::number(i),task_obj);
        i++;
    }

    sequence_obj.insert("tasks", tasks_obj);
    return sequence_obj;
}
Sequence* Sequence::Deserialize(QJsonObject obj, bool fromString){
    QVariantMap json_map = obj.toVariantMap();
    QVariantMap tasks_map;
    QVariantMap sequence_map;

    int keybind, method;
    if(!fromString){
        sequence_map = json_map["sequence"].toMap();
        keybind = sequence_map["keybind"].toInt();
        method = sequence_map["method"].toInt();
        tasks_map = sequence_map["tasks"].toMap();

    }
    else{
        keybind = json_map["keybind"].toInt();
        method = json_map["method"].toInt();
        tasks_map = json_map["tasks"].toMap();
    }

    list<Task*> tasksl;
    QStringList key_list = tasks_map.keys();
    for(int i=0; i < key_list.count(); ++i){
        QString key=key_list.at(i);
        QVariantMap task_map = tasks_map[key.toLocal8Bit()].toMap();

        Task* t = DeserializeTask(task_map);

        tasksl.push_back(t);
    }

    if(!fromString){
        return new Sequence(tasksl, keybind, method);
    }
    return new Sequence(tasksl, keybind, method);
}

Task* Sequence::DeserializeTask(QVariantMap task_map){
    Task* t;
    switch(task_map["type"].toInt()){
        case(Keyboard):
                t = KeyboardPress::Deserialize(task_map);
            break;
        case(Mouse):
                t = MouseClick::Deserialize(task_map);
            break;
        case(Text):
                t = KeyboardType::Deserialize(task_map);
            break;
        case(Cmd):
                t = CmdTask::Deserialize(task_map);
            break;
        case(Loop):
                t = LoopTask::Deserialize(task_map);
                LoopTask* l = dynamic_cast<LoopTask*>(t);

                QVariantMap tasks_map = task_map["list"].toMap();

                QStringList key_list = tasks_map.keys();
                for(int i=0; i < key_list.count(); ++i){
                    QString key=key_list.at(i);
                    QVariantMap task_map = tasks_map[key.toLocal8Bit()].toMap();

                    Task* t = Sequence::DeserializeTask(task_map);

                    l->Insert(t);
                }
            break;
        }
    return t;
}

void Sequence::StartSequence(){
    Sleep(500);//Hotkey pressed twice, dont stop
    switch(method){
        case Once:
        {
            thread t(&Sequence::SequenceStopWait, this);
            thread t2(&Sequence::RunOnce, this);

            t.detach();
            t2.detach();
        }
        break;
        case Toggle:
        {
            thread t(&Sequence::SequenceStopWait, this);
            thread t2(&Sequence::SequenceLoop, this);

            t.detach();
            t2.detach();
        }
        break;
        case WhilePressed:

        break;
    }

}

void Sequence::Insert(Task* task){
    tasks.push_back(task);
}

void Sequence::InsertFront(Task* task){
    tasks.push_front(task);
}

void Sequence::InsertAt(int position, Task* task){
    list<Task*>::iterator it = tasks.begin();
    advance(it, position);
    tasks.insert(it, task);
}

void Sequence::RemoveTask(Task* task){
    tasks.remove(task);
}

void Sequence::RunOnce(){
    cout<<"Run Once"<<endl;
    for(Task *task : tasks){
        if(running)
            task->Execute();
        else{
            break;
        }
    }
    running = false;
    waitFlag = true;
}

void Sequence::SequenceStopWait(){
    running = true;

    while(running){
        Sleep(10);
        if (GetAsyncKeyState(MapVirtualKeyA(keybind, 1))<0)
        {
            running = false;
            qDebug("STOP");
        }
    }
    while(!waitFlag){
        Sleep(50);
    }
    std::string filepath = std::string(":/assets/icons/play.svg");
    std::transform(filepath.begin(), filepath.end(), filepath.begin(), ::tolower);
    QIcon icon;
    icon.addFile(QString::fromUtf8(filepath), QSize(), QIcon::Normal, QIcon::Off);
    stopBtn->setIcon(icon);
}


void Sequence::SequenceLoop(){
    cout<<"Loop Started"<<endl;
    while(running){
        for(Task *task : tasks){
            if(running)
                task->Execute();
            else{
                break;
            }
        }
    }
    waitFlag = true;
}

int Sequence::CharToVirtual(char c){
    return VkKeyScanEx(c, LoadKeyboardLayoutA("00000409", 0x00000001));
}
