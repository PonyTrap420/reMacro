#include "macro_frame.h"

int MacroFrame::deletedMacro = 0;

MacroFrame::MacroFrame(QWidget *parent): QFrame(parent)
{
    this->id = 0;
    this->parent = parent;
    sequence = new Sequence(0);
}

MacroFrame::MacroFrame(QString* name, Sequence* sequence, QWidget *parent) : QFrame(parent){
    this->id = 0;
    this->parent = parent;
    this->name = *name;
    this->sequence = sequence;
}

MacroFrame::~MacroFrame()
{
}
