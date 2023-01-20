#ifndef LOOPTASKFRAME_H
#define LOOPTASKFRAME_H

#include "taskframe.h"
#include "QVBoxLayout"

class LoopTaskFrame : public TaskFrame
{
public:
    inline LoopTaskFrame(Task* t, QWidget *parent = 0) : TaskFrame(t, parent){
    }

    QFrame* sub = nullptr;

    void SetTaskList(QWidget* tasklist);
    QWidget* GetTaskList();

private:
    QWidget* tasklist = nullptr;
};

#endif // LOOPTASKFRAME_H
