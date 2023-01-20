#ifndef MACROLIST_H
#define MACROLIST_H

#include <QListWidget>
#include <QDropEvent>
#include <QDragEnterEvent>

class MacroList : public QListWidget
{
public:
    MacroList(QWidget *parent = nullptr);
protected:
    void dropEvent(QDropEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
//    void startDrag(Qt::DropActions supportedActions) override;

};


#endif // MACROLIST_H
