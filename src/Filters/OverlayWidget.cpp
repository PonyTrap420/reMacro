#include "OverlayWidget.h"
#include <QPainter>
#include "QComboBox"

OverlayWidget::OverlayWidget(int duration, State state, HttpClient* client, QSize size,Sequence* seq, QWidget* parent)
    : QWidget{ parent }
    , fade_effect(this)
    , animation(&fade_effect, "opacity")
{
    setPalette(Qt::transparent);
    setAttribute(Qt::WA_NoSystemBackground);
    newParent();
    setGraphicsEffect(&fade_effect);
    animation.setStartValue(0.0);
    animation.setEndValue(1.0);
    animation.setEasingCurve(QEasingCurve::InOutQuint);
    animation.setDuration(duration);
    setVisible(false);
    setEnabled(false);
    this->client = client;
    this->seq = seq;
    resize(size);
    SetState(state);
    installEventFilter(this);
    this->setFocusPolicy(Qt::StrongFocus);
}

void OverlayWidget::newParent()
{
    if (!parent()) return;
    parent()->installEventFilter(this);
    raise();
}

bool OverlayWidget::eventFilter(QObject* obj, QEvent* ev)
{
    if(ev->type() == QEvent::KeyPress){
        QKeyEvent *ke = static_cast<QKeyEvent *>(ev);
        if (ke->key() == Qt::Key_Escape){
            show(false);
        }
    }
    if (obj == parent()) {
        if (ev->type() == QEvent::Resize)
            resize(static_cast<QResizeEvent*>(ev)->size());
        else if (ev->type() == QEvent::ChildAdded)
            raise();
    }
    return QWidget::eventFilter(obj, ev);
}

bool OverlayWidget::event(QEvent* ev)
{
    if (ev->type() == QEvent::ParentAboutToChange) {
        if (parent()) parent()->removeEventFilter(this);
    }
    else if (ev->type() == QEvent::ParentChange)
        newParent();
    return QWidget::event(ev);
}

void OverlayWidget::show(const bool& show)
{
    animation.stop();
    animation.setStartValue(animation.currentValue());
    animation.setEndValue(show ? 1.0 : 0.0); // show or hide
    animation.start();
    if (show) {
        setVisible(true);
        setEnabled(true);
        setAttribute(Qt::WA_TransparentForMouseEvents, false);
        setFocus();
    }
    else {
        setEnabled(false);
        setAttribute(Qt::WA_TransparentForMouseEvents);
    }
}

void OverlayWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QBrush(QColor(0, 0, 0, 150)));
    painter.setOpacity(50);
    painter.setPen(Qt::NoPen);
    //painter.drawRoundedRect(rect(), 8.0, 8.0); // becasue my frame is round
    painter.drawRect(rect());
}

void OverlayWidget::SetState(State state){
    switch(state){
        case EmailValid:
            client->EmailVerifyRequest();
            EmailValidState();
        break;
        case PwdReset:
            PwdResetState1();
        break;
        case Keybinds:
            KeybindsState();
        break;

    }
}

#include "uiUtils.h"

bool CheckEmail(QString t){
    QRegularExpression rx("^[A-Z0-9a-z._-]{1,}@(\\w+)(\\.(\\w+))(\\.(\\w+))?(\\.(\\w+))?$");
    if(rx.match(t).hasMatch()){
        return true;
    }
    return false;
}

void OverlayWidget::sentEmail(){
    QString t = email->text();
    if(CheckEmail(t)){
        client->ForgotPwd(t);
        Util::clearWidgets(this->layout());
        PwdResetState2();
    }
}
void OverlayWidget::newPwd(){
    QString res = client->ResetPwd(secret->text(), newpwd->text());
    if(res == QString("")){
        qDebug("success");
        show(false);
    }
    else{
        reset_error->setText(res);
    }
}
void OverlayWidget::validEmail(){
    QString res = client->VerifyEmail(secret->text());
    if(res == QString("")){
        qDebug("success");
        show(false);
        validemailframe->setVisible(false);
    }
    else{
        reset_error->setText(res);
    }
}
void OverlayWidget::changedMethod(int index){
    seq->SetMethod(index);
    qDebug(QString::number(index).toLocal8Bit());
}
void OverlayWidget::changedKeybind(int key){
    seq->SetKeybind(key);
    emit ChangedKeybind(key);
}

#include "QVBoxLayout"
#include "QLabel"
#include "QPushButton"
#include "keyboard_field.h"

void OverlayWidget::KeybindsState(){
    QFrame* macro_keybinds;
    QVBoxLayout* verticalLayout_83;
    KeyboardField* keybinds_key;
    QComboBox* keybinds_method;

    QHBoxLayout* layout;
    layout = new QHBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);


    QFont font9;
    font9.setPointSize(11);
    font9.setBold(true);

    macro_keybinds = new QFrame(this);
    macro_keybinds->setObjectName(QString::fromUtf8("macro_keybinds"));
    macro_keybinds->setMinimumSize(QSize(300, 0));
    macro_keybinds->setMaximumSize(QSize(300, 100));
    macro_keybinds->setStyleSheet(QString::fromUtf8("\n"
"color: rgb(184, 184, 184); border-radius:5px;"));
    macro_keybinds->setFrameShape(QFrame::NoFrame);
    macro_keybinds->setFrameShadow(QFrame::Raised);

    layout->addWidget(macro_keybinds);

    verticalLayout_83 = new QVBoxLayout(macro_keybinds);
    verticalLayout_83->setSpacing(0);
    verticalLayout_83->setObjectName(QString::fromUtf8("verticalLayout_83"));
    verticalLayout_83->setContentsMargins(10, 0, 10, 0);
    keybinds_key = new KeyboardField(macro_keybinds, true);
    keybinds_key->setObjectName(QString::fromUtf8("keybinds_key"));
    keybinds_key->setMinimumSize(QSize(0, 40));
    keybinds_key->setMaximumSize(QSize(16777215, 40));
    keybinds_key->setFont(font9);
    keybinds_key->setStyleSheet(QString::fromUtf8("padding-right:5px;\n"
"border-radius: 5px;\n"
"background-color: rgb(18, 18, 18);\n"
"color: rgb(184, 184, 184);"));
    keybinds_key->setMaxLength(30);

    verticalLayout_83->addWidget(keybinds_key);

    keybinds_method = new QComboBox(macro_keybinds);
    keybinds_method->addItem(QString());
    keybinds_method->addItem(QString());
    keybinds_method->addItem(QString());
    keybinds_method->setObjectName(QString::fromUtf8("keybinds_method"));
    keybinds_method->setMinimumSize(QSize(0, 40));
    keybinds_method->setFont(font9);
    keybinds_method->setLayoutDirection(Qt::LeftToRight);
    keybinds_method->setStyleSheet(QString::fromUtf8("QComboBox{\n"
"	padding-right:5px;\n"
"	background-color: rgb(18, 18, 18);\n"
"	color: rgb(184, 184, 184);\n"
"	padding-left: 5px;\n"
"	border:0;\n"
"	border-radius:5px;\n"
"}\n"
"QComboBox::drop-down{\n"
"	border:0;\n"
"}\n"
"QComboBox::down-arrow{\n"
"	image: url(:/assets/icons/down-arrow.svg);\n"
"	width: 20px;\n"
"	height: 20px;\n"
"	margin-right: 20px;\n"
"}\n"
"\n"
"QListView{\n"
"	background-color: rgb(18, 18, 18);\n"
"	border:0;\n"
"}\n"
""));
    keybinds_method->setFrame(false);

    verticalLayout_83->addWidget(keybinds_method);

    keybinds_method->setItemText(0, "Play Once");
    keybinds_method->setItemText(1, "Toggle");
    keybinds_method->setItemText(2, "Play While Pressed");

    keybinds_method->setPlaceholderText("Execution Methods");

    keybinds_method->setCurrentIndex(seq->GetMethod());


    if(Util::ScanToChar(seq->GetKeybind()) == 0)
        keybinds_key->setPlaceholderText("No Keybind");
    else
        keybinds_key->setPlaceholderText("Keybind - " + QString(Util::ScanToChar(seq->GetKeybind())));


    connect(keybinds_method, SIGNAL(currentIndexChanged(int)), this, SLOT(changedMethod(int)));
    connect(keybinds_key, SIGNAL(ChangedKeybind(int)), this, SLOT(changedKeybind(int)));

}
void OverlayWidget::EmailValidState(){
    QFrame *forgot_password;
    QVBoxLayout *verticalLayout_69;
    QLabel *label_30;
    QFrame *frame_58;
    QVBoxLayout *verticalLayout_70;
    QSpacerItem *verticalSpacer_16;
    QPushButton *reset_pwd;
    QFont font13;
    font13.setPointSize(19);
    font13.setBold(true);
    QFont font10;
    font10.setPointSize(15);
    font10.setBold(true);
    QFont font11;
    font11.setPointSize(11);
    font11.setBold(true);

    QHBoxLayout* layout;
    layout = new QHBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    forgot_password = new QFrame(this);
    forgot_password->setObjectName(QString::fromUtf8("forgot_password"));
    forgot_password->setMaximumSize(QSize(360, 200));
    forgot_password->setStyleSheet(QString::fromUtf8("background-color: rgb(29, 29, 29);\n"
"border-radius:10px;"));
    forgot_password->setFrameShape(QFrame::StyledPanel);
    forgot_password->setFrameShadow(QFrame::Raised);
    layout->addWidget(forgot_password);

    verticalLayout_69 = new QVBoxLayout(forgot_password);
    verticalLayout_69->setSpacing(0);
    verticalLayout_69->setObjectName(QString::fromUtf8("verticalLayout_69"));
    verticalLayout_69->setContentsMargins(0, 5, 0, 0);
    label_30 = new QLabel(forgot_password);
    label_30->setObjectName(QString::fromUtf8("label_30"));
    label_30->setMaximumSize(QSize(16777215, 50));
    label_30->setFont(font13);
    label_30->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);"));
    label_30->setAlignment(Qt::AlignCenter);
    label_30->setText("Verify Email");

    verticalLayout_69->addWidget(label_30);

    frame_58 = new QFrame(forgot_password);
    frame_58->setObjectName(QString::fromUtf8("frame_58"));
    frame_58->setFrameShape(QFrame::StyledPanel);
    frame_58->setFrameShadow(QFrame::Raised);
    verticalLayout_70 = new QVBoxLayout(frame_58);
    verticalLayout_70->setSpacing(0);
    verticalLayout_70->setObjectName(QString::fromUtf8("verticalLayout_70"));
    verticalLayout_70->setContentsMargins(10, 0, 10, 0);
    secret = new QLineEdit(frame_58);
    secret->setObjectName(QString::fromUtf8("secret"));
    secret->setMinimumSize(QSize(0, 40));
    secret->setMaximumSize(QSize(16777215, 40));
    secret->setFont(font11);
    secret->setStyleSheet(QString::fromUtf8("padding-right:5px;\n"
"border-radius: 5px;\n"
"background-color: rgb(18, 18, 18);\n"
"color: rgb(184, 184, 184);"));
    secret->setPlaceholderText("Secret Key");

    verticalLayout_70->addWidget(secret);

    QFrame* log_error_frame;
    QVBoxLayout* verticalLayout_29;
    log_error_frame = new QFrame(frame_58);
    log_error_frame->setObjectName(QString::fromUtf8("log_error_frame"));
    log_error_frame->setMinimumSize(QSize(0, 0));
    log_error_frame->setStyleSheet(QString::fromUtf8("color: rgb(241, 58, 61);"));
    log_error_frame->setFrameShape(QFrame::StyledPanel);
    log_error_frame->setFrameShadow(QFrame::Raised);
    verticalLayout_29 = new QVBoxLayout(log_error_frame);
    verticalLayout_29->setSpacing(0);
    verticalLayout_29->setObjectName(QString::fromUtf8("verticalLayout_29"));
    verticalLayout_29->setContentsMargins(0, 0, 0, 0);
    reset_error = new QLabel(log_error_frame);
    reset_error->setObjectName(QString::fromUtf8("log_error_email"));
    reset_error->setFont(font11);
    reset_error->setWordWrap(true);

    log_error_frame->setMaximumHeight(35);

    verticalLayout_29->addWidget(reset_error);
    verticalLayout_70->addWidget(log_error_frame);

    verticalSpacer_16 = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed);

    verticalLayout_70->addItem(verticalSpacer_16);

    reset_pwd = new QPushButton(frame_58);
    reset_pwd->setObjectName(QString::fromUtf8("reset_pwd"));
    reset_pwd->setMinimumSize(QSize(0, 40));
    reset_pwd->setMaximumSize(QSize(16777215, 40));
    reset_pwd->setFont(font10);
    reset_pwd->setCursor(QCursor(Qt::PointingHandCursor));
    reset_pwd->setFocusPolicy(Qt::NoFocus);
    reset_pwd->setStyleSheet(QString::fromUtf8("border-radius: 5px;\n"
"background-color: rgb(18, 18, 18);\n"
"color: rgb(0, 149, 194);"));
    reset_pwd->setText("Verify");

    verticalLayout_70->addWidget(reset_pwd);


    verticalLayout_69->addWidget(frame_58);

    connect(reset_pwd, SIGNAL(clicked(bool)), this, SLOT(validEmail()));
}
void OverlayWidget::PwdResetState1(){
    QPushButton *reset_pwd;
    QLineEdit *reset_email;
    QFrame *forgot_password;
    QVBoxLayout *verticalLayout_69;
    QLabel *label_30;
    QFrame *frame_58;
    QVBoxLayout *verticalLayout_70;
    QSpacerItem *verticalSpacer_17;
    QSpacerItem *verticalSpacer_16;
    QFont font13;
    font13.setPointSize(19);
    font13.setBold(true);
    QFont font10;
    font10.setPointSize(15);
    font10.setBold(true);
    QFont font11;
    font11.setPointSize(11);
    font11.setBold(true);

    QHBoxLayout* layout;
    layout = new QHBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    forgot_password = new QFrame(this);
    forgot_password->setObjectName(QString::fromUtf8("forgot_password"));
    forgot_password->setMaximumSize(QSize(360, 200));
    forgot_password->setStyleSheet(QString::fromUtf8("background-color: rgb(29, 29, 29);\n"
"border-radius:10px;"));
    forgot_password->setFrameShape(QFrame::StyledPanel);
    forgot_password->setFrameShadow(QFrame::Raised);
    layout->addWidget(forgot_password);

    verticalLayout_69 = new QVBoxLayout(forgot_password);
    verticalLayout_69->setSpacing(0);
    verticalLayout_69->setObjectName(QString::fromUtf8("verticalLayout_69"));
    verticalLayout_69->setContentsMargins(0, 5, 0, 0);
    label_30 = new QLabel(forgot_password);
    label_30->setObjectName(QString::fromUtf8("label_30"));
    label_30->setMaximumSize(QSize(16777215, 50));
    label_30->setFont(font13);
    label_30->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);"));
    label_30->setAlignment(Qt::AlignCenter);
    label_30->setText("Reset Password");

    verticalLayout_69->addWidget(label_30);

    frame_58 = new QFrame(forgot_password);
    frame_58->setObjectName(QString::fromUtf8("frame_58"));
    frame_58->setFrameShape(QFrame::StyledPanel);
    frame_58->setFrameShadow(QFrame::Raised);
    verticalLayout_70 = new QVBoxLayout(frame_58);
    verticalLayout_70->setSpacing(0);
    verticalLayout_70->setObjectName(QString::fromUtf8("verticalLayout_70"));
    verticalLayout_70->setContentsMargins(10, 0, 10, 0);
    verticalSpacer_17 = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed);

    verticalLayout_70->addItem(verticalSpacer_17);

    reset_email = new QLineEdit(frame_58);
    reset_email->setObjectName(QString::fromUtf8("newpwd"));
    reset_email->setMinimumSize(QSize(0, 40));
    reset_email->setMaximumSize(QSize(16777215, 40));
    reset_email->setFont(font11);
    reset_email->setStyleSheet(QString::fromUtf8("padding-right:5px;\n"
"border-radius: 5px;\n"
"background-color: rgb(18, 18, 18);\n"
"color: rgb(184, 184, 184);"));
    reset_email->setMaxLength(30);
    reset_email->setPlaceholderText("Email");
    reset_email->setFocusPolicy(Qt::StrongFocus);

    verticalLayout_70->addWidget(reset_email);

    verticalSpacer_16 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed);

    verticalLayout_70->addItem(verticalSpacer_16);

    reset_pwd = new QPushButton(frame_58);
    reset_pwd->setObjectName(QString::fromUtf8("reset_pwd"));
    reset_pwd->setMinimumSize(QSize(0, 40));
    reset_pwd->setMaximumSize(QSize(16777215, 40));
    reset_pwd->setFont(font10);
    reset_pwd->setCursor(QCursor(Qt::PointingHandCursor));
    reset_pwd->setFocusPolicy(Qt::NoFocus);
    reset_pwd->setStyleSheet(QString::fromUtf8("border-radius: 5px;\n"
"background-color: rgb(18, 18, 18);\n"
"color: rgb(0, 149, 194);"));
    reset_pwd->setText("Continue");

    verticalLayout_70->addWidget(reset_pwd);


    verticalLayout_69->addWidget(frame_58);

    email = reset_email;
    connect(reset_pwd, SIGNAL(clicked(bool)), this, SLOT(sentEmail()));

}
void OverlayWidget::PwdResetState2(){
    QFrame *forgot_password;
    QVBoxLayout *verticalLayout_69;
    QLabel *label_30;
    QFrame *frame_58;
    QVBoxLayout *verticalLayout_70;
    QSpacerItem *verticalSpacer_17;
    QSpacerItem *verticalSpacer_16;
    QPushButton *reset_pwd;
    QFont font13;
    font13.setPointSize(19);
    font13.setBold(true);
    QFont font10;
    font10.setPointSize(15);
    font10.setBold(true);
    QFont font11;
    font11.setPointSize(11);
    font11.setBold(true);


    forgot_password = new QFrame(this);
    forgot_password->setObjectName(QString::fromUtf8("forgot_password"));
    forgot_password->setMaximumSize(QSize(360, 265));
    forgot_password->setStyleSheet(QString::fromUtf8("background-color: rgb(29, 29, 29);\n"
"border-radius:10px;"));
    forgot_password->setFrameShape(QFrame::StyledPanel);
    forgot_password->setFrameShadow(QFrame::Raised);
    this->layout()->addWidget(forgot_password);

    verticalLayout_69 = new QVBoxLayout(forgot_password);
    verticalLayout_69->setSpacing(0);
    verticalLayout_69->setObjectName(QString::fromUtf8("verticalLayout_69"));
    verticalLayout_69->setContentsMargins(0, 5, 0, 0);
    label_30 = new QLabel(forgot_password);
    label_30->setObjectName(QString::fromUtf8("label_30"));
    label_30->setMaximumSize(QSize(16777215, 50));
    label_30->setFont(font13);
    label_30->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);"));
    label_30->setAlignment(Qt::AlignCenter);
    label_30->setText("Reset Password");

    verticalLayout_69->addWidget(label_30);

    frame_58 = new QFrame(forgot_password);
    frame_58->setObjectName(QString::fromUtf8("frame_58"));
    frame_58->setFrameShape(QFrame::StyledPanel);
    frame_58->setFrameShadow(QFrame::Raised);
    verticalLayout_70 = new QVBoxLayout(frame_58);
    verticalLayout_70->setSpacing(0);
    verticalLayout_70->setObjectName(QString::fromUtf8("verticalLayout_70"));
    verticalLayout_70->setContentsMargins(10, 0, 10, 0);
    secret = new QLineEdit(frame_58);
    secret->setObjectName(QString::fromUtf8("secret"));
    secret->setMinimumSize(QSize(0, 40));
    secret->setMaximumSize(QSize(16777215, 40));
    secret->setFont(font11);
    secret->setStyleSheet(QString::fromUtf8("padding-right:5px;\n"
"border-radius: 5px;\n"
"background-color: rgb(18, 18, 18);\n"
"color: rgb(184, 184, 184);"));
    secret->setPlaceholderText("Secret Key");

    verticalLayout_70->addWidget(secret);

    newpwd = new QLineEdit(frame_58);
    newpwd->setObjectName(QString::fromUtf8("newpwd"));
    newpwd->setMinimumSize(QSize(0, 40));
    newpwd->setMaximumSize(QSize(16777215, 40));
    newpwd->setFont(font11);
    newpwd->setStyleSheet(QString::fromUtf8("padding-right:5px;\n"
"border-radius: 5px;\n"
"background-color: rgb(18, 18, 18);\n"
"color: rgb(184, 184, 184);"));
    newpwd->setMaxLength(30);
    newpwd->setPlaceholderText("New Password");

    verticalLayout_70->addWidget(newpwd);

    QFrame* log_error_frame;
    QVBoxLayout* verticalLayout_29;
    log_error_frame = new QFrame(frame_58);
    log_error_frame->setObjectName(QString::fromUtf8("log_error_frame"));
    log_error_frame->setMinimumSize(QSize(0, 0));
    log_error_frame->setStyleSheet(QString::fromUtf8("color: rgb(241, 58, 61);"));
    log_error_frame->setFrameShape(QFrame::StyledPanel);
    log_error_frame->setFrameShadow(QFrame::Raised);
    verticalLayout_29 = new QVBoxLayout(log_error_frame);
    verticalLayout_29->setSpacing(0);
    verticalLayout_29->setObjectName(QString::fromUtf8("verticalLayout_29"));
    verticalLayout_29->setContentsMargins(0, 0, 0, 0);
    reset_error = new QLabel(log_error_frame);
    reset_error->setObjectName(QString::fromUtf8("log_error_email"));
    reset_error->setFont(font11);
    reset_error->setWordWrap(true);

    log_error_frame->setMaximumHeight(35);

    verticalLayout_29->addWidget(reset_error);
    verticalLayout_70->addWidget(log_error_frame);

    verticalSpacer_16 = new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Fixed);

    verticalLayout_70->addItem(verticalSpacer_16);

    reset_pwd = new QPushButton(frame_58);
    reset_pwd->setObjectName(QString::fromUtf8("reset_pwd"));
    reset_pwd->setMinimumSize(QSize(0, 40));
    reset_pwd->setMaximumSize(QSize(16777215, 40));
    reset_pwd->setFont(font10);
    reset_pwd->setCursor(QCursor(Qt::PointingHandCursor));
    reset_pwd->setFocusPolicy(Qt::NoFocus);
    reset_pwd->setStyleSheet(QString::fromUtf8("border-radius: 5px;\n"
"background-color: rgb(18, 18, 18);\n"
"color: rgb(0, 149, 194);"));
    reset_pwd->setText("Reset");

    verticalLayout_70->addWidget(reset_pwd);


    verticalLayout_69->addWidget(frame_58);

    connect(reset_pwd, SIGNAL(clicked(bool)), this, SLOT(newPwd()));
}
