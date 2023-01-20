#ifndef MARKETMACROFRAME_H
#define MARKETMACROFRAME_H

#include <QObject>
#include <QFrame>
#include <QWidget>
#include "qpushbutton.h"
#include <QtGui>
#include "sequence.h"
#include "qlabel.h"
#include "qlineedit.h"
#include "HTTPclient.h"


struct Macro{
    param id;
    param downloads;
    param name;
    param description;
    param user;
    param macro;

    Macro(param id, param downloads, param name, param description, param user, param macro){
        this->id = id;
        this->downloads = downloads;
        this->name = name;
        this->description = description;
        this->user = user;
        this->macro = macro;
    }


};

struct Comment{
    param id;
    param user;
    param content;

    Comment(param id, param user, param content){
        this->id = id;
        this->user = user;
        this->content = content;
    }
};


class MarketMacroFrame : public QFrame
{
    Q_OBJECT
public:

    QLabel* name = nullptr;
    QPushButton* avatar = nullptr;
    QPushButton* explore = nullptr;

    Macro* macro = nullptr;
    User* user = nullptr;

    HttpClient* client = nullptr;
    Sequence* seq = nullptr;

    MarketMacroFrame(Macro* macro, HttpClient* client, QWidget *parent = 0);

signals:
   void ExplorePressed();

private slots:
   void emitExplorePressed();

private:
    QWidget *parent = nullptr;

    void GetUser(QString userid);
};

#endif // MARKETMACROFRAME_H
