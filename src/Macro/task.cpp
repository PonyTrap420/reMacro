#include "task.h"

Task::Task(){
    this->name = "name";
    this->delay = 1;
    this->repeats = 1;
}

void Task::SetDelay(double delay){
    this->delay = delay;
}
void Task::SetName(QString name){
    this->name = name;
}
void Task::SetRepeats(int repeats){
    this->repeats = repeats;
}
