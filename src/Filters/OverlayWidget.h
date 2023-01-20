#pragma once

#include <QObject>
#include <QFrame>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QPaintEvent>
#include <QMouseEvent>
#include "HTTPclient.h"
#include "QLineEdit"
#include "QLabel"


class OverlayWidget : public QWidget
{
    Q_OBJECT
public:
    enum State{
        EmailValid,
        PwdReset,
        Keybinds
    };

    HttpClient* client = nullptr;

    explicit OverlayWidget(int duration, State state, HttpClient* client, QSize size,Sequence* seq = nullptr, QWidget* parent = nullptr);
    void show(const bool& show = true);

    QFrame* validemailframe = nullptr;
    Sequence* seq = nullptr;
private slots:
    void sentEmail();
    void newPwd();
    void validEmail();
    void changedMethod(int index);
    void changedKeybind(int key);

signals:
    void pressed();
    void ChangedKeybind(int key);

private:
    QGraphicsOpacityEffect fade_effect;
    QPropertyAnimation animation;

    void SetState(State state);

    QLineEdit* email = nullptr;
    QLineEdit* secret = nullptr;
    QLineEdit* newpwd = nullptr;

    QLabel* reset_error = nullptr;

    inline void EmailValidState();
    inline void KeybindsState();
    inline void PwdResetState1();
    inline void PwdResetState2();

protected:
    void paintEvent(QPaintEvent* event);
    bool eventFilter(QObject* obj, QEvent* ev) override;
    bool event(QEvent* ev) override;


private:
    void newParent();
};
