#include "looptaskframe.h"

void LoopTaskFrame::SetTaskList(QWidget* tasklist){
    this->tasklist = tasklist;
}
QWidget* LoopTaskFrame::GetTaskList(){
    return tasklist;
}
