#ifndef SEQUENCE_H
#define SEQUENCE_H

#include "task.h"
#include "list"
#include "QJsonObject"
#include "qpushbutton.h"

#define NO_MOD 0x4000

using std::list;

enum Methods{
    Once,
    Toggle,
    WhilePressed
};

class Sequence
{
    public:
        list<Task*> tasks;

        Sequence(list<Task*> tasks, int keybind, int method = Once);
        Sequence(int keybind, int method = Once);

        int GetKeybind();
        int GetMethod();

        void SetKeybind(int keybind);
        void SetMethod(int method);

        void StartSequence();
        void Insert(Task* task);
        void InsertFront(Task* task);
        void InsertAt(int position, Task* task);
        void RemoveTask(Task* task);

        QJsonObject Serialize();
        static Sequence* Deserialize(QJsonObject obj, bool fromString = false);

        static Task* DeserializeTask(QVariantMap taskmap);

        static QPushButton* stopBtn;
        bool IsRunning();

    private:
        int keybind = 0;
        int method = 0;

        bool running = false;
        bool waitFlag = false;

        void RunOnce();
        void SequenceStopWait();
        void SequenceLoop();

        int CharToVirtual(char c);
};

#endif // SEQUENCE_H
