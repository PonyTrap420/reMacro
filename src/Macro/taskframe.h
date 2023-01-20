#ifndef TASKFRAME_H
#define TASKFRAME_H

#include <QObject>
#include <QFrame>
#include <QWidget>
#include <QtGui>
#include <qlabel.h>
#include "task.h"
#include "looptask.h"

class TaskFrame : public QFrame
{
    Q_OBJECT

public:
    TaskFrame( Task* task, QWidget *parent = 0);
    ~TaskFrame();

    inline Task* GetTask(){
        return task;
    }

    inline QWidget* GetParent(){
        return m_parent;
    }

    inline QLabel* GetName(){
        return name;
    }

    inline QLabel* GetDelay(){
        return delay;
    }

    void SetName(QLabel* name);
    void SetDelay(QLabel* delay);
    void SetLoop(LoopTask* loop);
    void DeleteFromLoop();

private:
    QWidget *m_parent = nullptr;

    LoopTask* loop = nullptr;
    Task* task = nullptr;

    QLabel* name = nullptr;
    QLabel* delay = nullptr;
};

#endif // TASKFRAME_H
