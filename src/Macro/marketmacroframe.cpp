#include "marketmacroframe.h"
#include "QHBoxLayout"

MarketMacroFrame::MarketMacroFrame(Macro* macro, HttpClient* client, QWidget *parent) : QFrame(parent){
    this->client = client;
    this->macro = macro;

    this->setStyleSheet(QString::fromUtf8("background-color: rgb(32, 32, 32); border-radius:5px;"));
    this->setLayoutDirection(Qt::RightToLeft);

    QFrame *frame_28;
    QVBoxLayout *verticalLayout_44;
    QFrame *frame_31;
    QHBoxLayout *horizontalLayout_26;
    QHBoxLayout *horizontalLayout_27;
    QFrame *frame_32;
    QHBoxLayout *horizontalLayout_25;
    QFrame *frame_33;
    QSpacerItem *horizontalSpacer_12;

    this->setMaximumWidth(230);

    QVBoxLayout *layout;
    layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setObjectName(QString::fromUtf8("verticalLayout_44"));
    layout->setContentsMargins(10, 0, 10, 0);

    frame_28 = new QFrame(this);
    frame_28->setObjectName(QString::fromUtf8("frame_28"));
    frame_28->setMinimumSize(QSize(200, 0));
    frame_28->setMaximumSize(QSize(200, 16777215));
    frame_28->setStyleSheet(QString::fromUtf8("border:none"));
    frame_28->setFrameShape(QFrame::StyledPanel);
    frame_28->setFrameShadow(QFrame::Raised);
    verticalLayout_44 = new QVBoxLayout(frame_28);
    verticalLayout_44->setSpacing(0);
    verticalLayout_44->setObjectName(QString::fromUtf8("verticalLayout_44"));
    verticalLayout_44->setContentsMargins(0, 0, 0, 0);
    frame_31 = new QFrame(frame_28);
    frame_31->setObjectName(QString::fromUtf8("frame_31"));
    frame_31->setMaximumSize(QSize(16777215, 50));
    frame_31->setStyleSheet(QString::fromUtf8("border:0;"));
    frame_31->setFrameShape(QFrame::StyledPanel);
    frame_31->setFrameShadow(QFrame::Raised);
    horizontalLayout_26 = new QHBoxLayout(frame_31);
    horizontalLayout_26->setSpacing(0);
    horizontalLayout_26->setObjectName(QString::fromUtf8("horizontalLayout_26"));
    horizontalLayout_26->setContentsMargins(0, 0, 0, 0);

    QFont font8;
    font8.setPointSize(15);
    font8.setBold(false);

    name = new QLabel(frame_31);
    name->setObjectName(QString::fromUtf8("name"));
    name->setMinimumSize(QSize(90, 0));
    name->setFont(font8);
    name->setAlignment(Qt::AlignCenter);
    name->setText(macro->name);

    horizontalLayout_26->addWidget(name);


    verticalLayout_44->addWidget(frame_31);

    frame_32 = new QFrame(frame_28);
    frame_32->setObjectName(QString::fromUtf8("frame_32"));
    frame_32->setStyleSheet(QString::fromUtf8("border:0;"));
    frame_32->setFrameShape(QFrame::StyledPanel);
    frame_32->setFrameShadow(QFrame::Raised);
    horizontalLayout_25 = new QHBoxLayout(frame_32);
    horizontalLayout_25->setSpacing(0);
    horizontalLayout_25->setObjectName(QString::fromUtf8("horizontalLayout_25"));
    horizontalLayout_25->setContentsMargins(10, 10, 10, 10);
    frame_33 = new QFrame(frame_32);
    frame_33->setObjectName(QString::fromUtf8("frame_33"));
    frame_33->setMinimumSize(QSize(70, 0));
    frame_33->setMaximumSize(QSize(16777215, 30));
    frame_33->setFrameShape(QFrame::StyledPanel);
    frame_33->setFrameShadow(QFrame::Raised);
    horizontalLayout_27 = new QHBoxLayout(frame_33);
    horizontalLayout_27->setSpacing(5);
    horizontalLayout_27->setObjectName(QString::fromUtf8("horizontalLayout_27"));
    horizontalLayout_27->setContentsMargins(0, 0, 0, 0);

    QIcon icon11;
    icon11.addFile(QString::fromUtf8(":/assets/icons/expand.svg"), QSize(), QIcon::Normal, QIcon::Off);
    explore = new QPushButton(frame_33);
    explore->setObjectName(QString::fromUtf8("explore"));
    explore->setMinimumSize(QSize(30, 30));
    explore->setStyleSheet(QString::fromUtf8("background-color:none;"));
    explore->setIcon(icon11);
    explore->setIconSize(QSize(30, 30));
    explore->setFocusPolicy(Qt::NoFocus);
    explore->setCursor(QCursor(Qt::PointingHandCursor));


    horizontalLayout_27->addWidget(explore);


    horizontalLayout_25->addWidget(frame_33);

    horizontalSpacer_12 = new QSpacerItem(54, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_25->addItem(horizontalSpacer_12);

    avatar = new QPushButton(frame_32);
    avatar->setObjectName(QString::fromUtf8("icon"));
    avatar->setMinimumSize(QSize(70, 70));
    avatar->setMaximumSize(QSize(70, 70));
    avatar->setStyleSheet(QString::fromUtf8("background-color: none;"));
    avatar->setFocusPolicy(Qt::NoFocus);
    avatar->setIconSize(QSize(70, 70));

    horizontalLayout_25->addWidget(avatar);


    verticalLayout_44->addWidget(frame_32);


    layout->addWidget(frame_28);


    connect(explore, SIGNAL(pressed()), this, SLOT(emitExplorePressed()));
    GetUser(macro->user);
}
void MarketMacroFrame::GetUser(QString userid){
    User u = client->GetUser(userid);
    avatar->setIcon(client->GetIcon(u.avatar, avatar->size()));
    avatar->setObjectName(userid);
    avatar->setCursor(QCursor(Qt::PointingHandCursor));
}

void MarketMacroFrame::emitExplorePressed(){
    emit ExplorePressed();
}
