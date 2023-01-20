#ifndef MACRO_FRAME_H
#define MACRO_FRAME_H

#include <QObject>
#include <QFrame>
#include <QWidget>
#include "qpushbutton.h"
#include <QtGui>
#include "sequence.h"
#include "qlabel.h"
#include "qlineedit.h"

class MacroFrame : public QFrame
{
    Q_OBJECT

public:

    static int deletedMacro;
    QString name;
    int id = 0;
    QLabel* name_label = nullptr;
    QLineEdit* name_change = nullptr;


    MacroFrame(QWidget *parent = 0);
    MacroFrame(QString* name, Sequence* seq, QWidget *parent = 0);
    ~MacroFrame();


    inline void SetMoreBtn(QPushButton* moreBtn){
        this->moreBtn = moreBtn;
    }

    inline Sequence* GetSequence(){
        return sequence;
    }
    inline QPushButton* GetOptionsBtn(){
        return moreBtn;
    }
    inline void SetSequence(Sequence* sequence){
        this->sequence = sequence;
    }

private:
    QWidget *parent = nullptr;
    Sequence *sequence = nullptr;
    QPushButton* moreBtn = nullptr;
};

#endif // MACRO_FRAME_H
