#include "MacroList.h"
#include <QDebug>
#include <QMimeData>
#include <QDrag>
#include "Macro/taskframe.h"


MacroList::MacroList(QWidget *parent) : QListWidget (parent)
{
    setAcceptDrops(true);
}
void MacroList::dropEvent(QDropEvent *event)
{
    qDebug() << "dropEvent"<<event;
    if(event->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")){
        QListWidgetItem* w = dynamic_cast<QListWidgetItem*>(event->source());
        qDebug() << "item"<<w;
        //TaskFrame* t = dynamic_cast<TaskFrame*>(itemWidget(w));
        //w->setSizeHint(QSize(16777215, 50)); //When minimumSizeHint doesn't work
    }
    event->acceptProposedAction();
}
void MacroList::dragEnterEvent(QDragEnterEvent *event)
{
    qDebug() << "dragEvent"<<event;
    event->acceptProposedAction();
}
void MacroList::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

//void MacroList::startDrag(Qt::DropActions supportedActions)
//{
//   Q_UNUSED (supportedActions)

//   auto mimeData = new QMimeData ();
//   mimeData->setData("application/x-item", currentItem ()->text ().toUtf8 ());

//   auto drag = new QDrag(this);
//   drag->setMimeData (mimeData);
//   drag->exec(Qt::MoveAction);
//}
