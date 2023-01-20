#include "taskframe.h"
#include "qlayout.h"

TaskFrame::TaskFrame(Task* task, QWidget *parent): QFrame(parent)
{
    this->task = task;
    parent->installEventFilter(this);
}

TaskFrame::~TaskFrame()
{

}

void TaskFrame::SetName(QLabel* name){
    this->name = name;
}

void TaskFrame::SetDelay(QLabel* delay){
    this->delay = delay;
}

void TaskFrame::SetLoop(LoopTask* loop){
    this->loop = loop;
}

void TaskFrame::DeleteFromLoop(){
    if(loop!=nullptr)
        loop->RemoveTask(task);
}
