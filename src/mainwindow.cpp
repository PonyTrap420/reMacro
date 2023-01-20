#define _GLIBCXX_USE_CXX11_ABI 0

#include "mainwindow.h"

#include "iostream"
#include <QLabel>
#include <QLayout>
#include "ui_mainwindow.h"
#include <QMenu>
#include <QAction>
#include "stdlib.h"
#include "HTTPclient.h"
#include "QScrollBar"
#include <QGraphicsDropShadowEffect>

#include "Macro/sequence.h"
#include "Macro/task.h"
#include "Macro/keyboardpress.h"
#include "Macro/keyboardtype.h"
#include "Macro/mouseclick.h"
#include "Macro/macro_frame.h"
#include "Macro/taskframe.h"
#include "Macro/marketmacroframe.h"
#include "Macro/cmdtask.h"
#include "Macro/looptask.h"

#define _WIN32_WINNT 0x0600
#include "windows.h"

#include "uiUtils.h"

#include "macromenuoptions.h"

#include <shlobj.h>

#include "Filters/keyboard_field.h"
#include "Filters/OverlayWidget.h"
#include "Filters/passwordedit.h"


static map<int, int> fixCode;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    client = HttpClient::getInstance();

    fixCode[WM_MOUSEMOVE] = MOUSEEVENTF_MOVE;
    fixCode[WM_LBUTTONDOWN] = MOUSEEVENTF_LEFTDOWN;
    fixCode[WM_LBUTTONUP] = MOUSEEVENTF_LEFTUP;
    fixCode[WM_RBUTTONDOWN] = MOUSEEVENTF_RIGHTDOWN;
    fixCode[WM_RBUTTONUP] = MOUSEEVENTF_RIGHTUP;
    ui->setupUi(this);
    SetupAppdata();
    SetupGUI();

}

void MainWindow::SetupAppdata(){
    WCHAR* path;
    auto get_folder_path_ret = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &path);

    if (get_folder_path_ret != S_OK) {
        CoTaskMemFree(path);
    }

    macropath = path;
    macropath += L"\\reMacro";
    CreateDirectoryW(macropath.c_str(), NULL);
    QString filename = QString::fromStdWString(macropath) + "\\macros.json";
    qDebug(filename.toLocal8Bit());
    QFile file(filename);
    if (file.open(QIODevice::ReadWrite)) {
        QTextStream file_text(&file);
        QString json_string;
        json_string = file_text.readAll();
        file.close();
        QByteArray json_bytes = json_string.toLocal8Bit();
        QJsonDocument json_doc=QJsonDocument::fromJson(json_bytes);
        if(json_doc.isNull()){
            qDebug()<<"Failed to create JSON doc.";
        }
        if(!json_doc.isObject()){
            qDebug()<<"JSON is not an object.";
        }

        macrosJson = json_doc.array();

        if(macrosJson.isEmpty()){
            qDebug()<<"JSON object is empty.";
        }

        int times = macrosJson.count();
        int lastId = 0;
        for(int i = 0; i< times; i++){
            QJsonObject d = macrosJson[i].toObject();
            QJsonObject macro_obj;
            QString id;
            foreach(const QString& key, d.keys()) {
                  id = key;
                  macro_obj = d.value(key).toObject();
            }
            int counter = 0;
            while(id.toInt() != i && counter < times){
                d = macrosJson[counter].toObject();
                foreach(const QString& key, d.keys()) {
                      id = key;
                      macro_obj = d.value(key).toObject();
                }
                counter++;
            }
            QVariantMap json_map = macro_obj.toVariantMap();

            Sequence* s = Sequence::Deserialize(macro_obj);
            NewMacroFrame(json_map["name"].toString(),s);

            if(id.toInt() != i){
                QJsonObject replaceId;
                replaceId.insert(QString::number(selectedMacro->id), macro_obj);
                qDebug(std::to_string(counter).c_str());
                macrosJson.removeAt(times-1);
                macrosJson.push_front(replaceId);

                QJsonDocument json_doc(macrosJson);
                json_string = json_doc.toJson();

                QFile save_file(filename);
                if(save_file.open(QIODevice::WriteOnly)){
                    save_file.write(json_string.toLocal8Bit());
                    save_file.close();
                }
            }

        }

    }

    CoTaskMemFree(path);
}

void MainWindow::SetupGUI(){
    auto trayIconMenu = new QMenu(this);


    executeTray = new QAction(QIcon(":/assets/icons/play.svg"),tr("&Play Macro"), this);
    connect(executeTray, &QAction::triggered, [this]()
    {
        on_executeMacro_clicked();
    });

    auto title = new QAction(QIcon(":icon"),tr("&reMacro"), this);
    title->setDisabled(true);
    trayIconMenu->addAction(title);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(executeTray);
    QString tooltip("reMacro"); //NOT WORKING
    trayIconMenu->setToolTip(tooltip);
    trayIconMenu->setStyleSheet(""
"QMenu{"
"border:0px;"
"}"
"QMenu::item{"
"color:white; background-color:#1d1d1d;"
"}"
"QMenu::item:selected{"
"background-color:#3e3e3e"
"}"
"QMenu::separator{"
"height:1px;"
"background-color:#3c4043;"
"}");
    trayIconMenu->addSeparator();
    trayIconMenu->setTitle("reMacro");

    sysTrayIcon = new QSystemTrayIcon(this);
    sysTrayIcon->setContextMenu(trayIconMenu);
    sysTrayIcon->setIcon(QIcon(":icon"));
    sysTrayIcon->show();

    auto exitAction = new QAction(("&Quit"),this);
    connect(exitAction, &QAction::triggered, [this]()
    {
        exit(0);
    });
    trayIconMenu->addAction(exitAction);

    //Windows example of adding a toolbar + min/max/close buttons
#ifdef _WIN32

    //Add the toolbar
    toolBar = new QToolBar(this);
    toolBar->setMovable(false);
    toolBar->setFloatable(false);
    toolBar->setContextMenuPolicy(Qt::NoContextMenu);
    toolBar->setContextMenuPolicy(Qt::PreventContextMenu);
    addToolBar(toolBar);

    //toolBar->setMaximumHeight(25);

    toolBar->setStyleSheet("background-color: #202020; border: none; color:white; padding:0px;");

    //Create a transparent-to-mouse-events widget that pads right for a fixed width equivalent to min/max/close buttons
    QWidget* btnSpacer = new QWidget(toolBar);
    btnSpacer->setAttribute(Qt::WA_TransparentForMouseEvents);
    btnSpacer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    btnSpacer->setStyleSheet("background-color: none; border: none;");
    btnSpacer->setFixedWidth(135 /* rough width of close/min/max buttons */);
    //toolBar->addWidget(btnSpacer);


    //Create a title label just because
    QLabel* titleLabel = new QLabel("reMacro");
    QFont font;
    font.setPointSize(9);
    font.setBold(true);
    titleLabel->setFont(font);
    titleLabel->setStyleSheet("margin-left: 5px;");


    titleLabel->setFixedWidth(160);

    //Set it transparent to mouse events such that you can click and drag when moused over the label
    titleLabel->setAttribute(Qt::WA_TransparentForMouseEvents);


    //Create spacer widgets to keep the title centered
    QWidget* leftSpacer = new QWidget(toolBar);
    QWidget* rightSpacer = new QWidget(toolBar);

    //Set them transparent to mouse events + auto-expanding in size
    leftSpacer->setAttribute(Qt::WA_TransparentForMouseEvents);
    leftSpacer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    leftSpacer->setMinimumSize(QSize(10,0));
    leftSpacer->setStyleSheet("background-color: none; border: none;");
    rightSpacer->setAttribute(Qt::WA_TransparentForMouseEvents);
    rightSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    rightSpacer->setStyleSheet("background-color: none; border: none;");

    //Add spacers & title label
    toolBar->addWidget(leftSpacer);
    toolBar->addWidget(titleLabel);
    toolBar->addWidget(rightSpacer);


    //Create the min/max/close buttons
    QIcon icon1, icon2, icon3;
    icon1.addFile(QString::fromUtf8(":/final/assets/final/minimize.svg"), QSize(), QIcon::Normal, QIcon::Off);
    icon2.addFile(QString::fromUtf8(":/final/assets/final/maximize.svg"), QSize(), QIcon::Normal, QIcon::Off);
    icon3.addFile(QString::fromUtf8(":/final/assets/final/close.svg"), QSize(), QIcon::Normal, QIcon::Off);

    //pushButton->setIconSize(QSize(18, 18));

    minimizeButton = new QPushButton("");
    minimizeButton->setIcon(icon1);
    minimizeButton->setCursor(QCursor(Qt::PointingHandCursor));
    minimizeButton->setFocusPolicy(Qt::NoFocus);
    minimizeButton->setStyleSheet(""
    "QPushButton{"
        "background-color: #0f0f0f;"
    "}"
    "QPushButton:hover{"
        "background-color: #575757;"
        "border: 0px;"
    "}"
    "QPushButton:pressed{"
    "background-color: #4d4d4d;"
        "border: 0px;"
    "}");


    maximizeButton = new QPushButton("");
    maximizeButton->setIcon(icon2);
    maximizeButton->setCursor(QCursor(Qt::PointingHandCursor));
    maximizeButton->setFocusPolicy(Qt::NoFocus);
    maximizeButton->setStyleSheet(""
"QPushButton{"
   "background-color: #0f0f0f;"
"}"
"QPushButton:hover{"
   "background-color: #575757;"
    "border: 0px;"
"}"
"QPushButton:pressed{"
    "background-color: #4d4d4d;"
    "border: 0px;"
"}");


    closeButton = new QPushButton("");
    closeButton->setIcon(icon3);
    closeButton->setCursor(QCursor(Qt::PointingHandCursor));
    closeButton->setFocusPolicy(Qt::NoFocus);
    closeButton->setStyleSheet(""
"QPushButton{"
 "background-color: #0f0f0f;"
"}"
"QPushButton:hover{"
    "background-color: rgb(240, 71,71);"
    "border: 0px;"
"}"
"QPushButton:pressed{"
    "border: 0px;"
"}");



    maximizeButton->setCheckable(true);

    minimizeButton->setFixedSize(45, 31);
    maximizeButton->setFixedSize(45, 31);
    closeButton->setFixedSize(45, 31);

    minimizeButton->setContextMenuPolicy(Qt::NoContextMenu);
    maximizeButton->setContextMenuPolicy(Qt::NoContextMenu);
    closeButton->setContextMenuPolicy(Qt::NoContextMenu);

    toolBar->addWidget(minimizeButton);
    toolBar->addWidget(maximizeButton);
    toolBar->addWidget(closeButton);
    toolBar->layout()->setAlignment(minimizeButton, Qt::AlignTop);
    toolBar->layout()->setAlignment(maximizeButton, Qt::AlignTop);
    toolBar->layout()->setAlignment(closeButton, Qt::AlignTop);


    //An actual app should use icons for the buttons instead of text
    //and style the different button states / widget margins in css
    //ui->stackedWidget->setCurrentIndex(0);

    ui->home_label->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->library_label->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->market_label->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->user_label->setAttribute(Qt::WA_TransparentForMouseEvents);

    on_home_btn_pressed();
    ui->windowBar->setVisible(false);
    ui->prop_body->setVisible(false);

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect;
    shadow->setBlurRadius(15);
    shadow->setColor(QColor(25,25,25));
    shadow->setXOffset(0);
    shadow->setYOffset(0);

    QGraphicsDropShadowEffect *shadow1 = new QGraphicsDropShadowEffect;
    shadow1->setBlurRadius(15);
    shadow1->setColor(QColor(25,25,25));
    shadow1->setXOffset(0);
    shadow1->setYOffset(0);

    QGraphicsDropShadowEffect *shadow2 = new QGraphicsDropShadowEffect;
    shadow2->setBlurRadius(15);
    shadow2->setColor(QColor(25,25,25));
    shadow2->setXOffset(0);
    shadow2->setYOffset(0);

    QGraphicsDropShadowEffect *shadow3 = new QGraphicsDropShadowEffect;
    shadow3->setBlurRadius(15);
    shadow3->setColor(QColor(25,25,25));
    shadow3->setXOffset(0);
    shadow3->setYOffset(0);

    QGraphicsDropShadowEffect *shadow4 = new QGraphicsDropShadowEffect;
    shadow4->setBlurRadius(15);
    shadow4->setColor(QColor(25,25,25));
    shadow4->setXOffset(0);
    shadow4->setYOffset(0);

    ui->macro_list->setGraphicsEffect(shadow);
    ui->macro_editor->setGraphicsEffect(shadow1);
    ui->macro_prop->setGraphicsEffect(shadow2);
    ui->macro_items->setGraphicsEffect(shadow3);
    ui->macro_leftside->setGraphicsEffect(shadow4);


    ui->verticalLayout_22->removeWidget(ui->field_keystroke);
    ui->field_keystroke->close();
    ui->field_keystroke = new KeyboardField(ui->prop_field_keystroke);
    ui->field_keystroke->setObjectName(QString::fromUtf8("field_keystroke"));
    ui->field_keystroke->setStyleSheet(QString::fromUtf8("border: 2px solid #222222;\n"
"border-radius:0; font-size: 12px; font-weight: 600; color: #FFFFFF;"));
    ui->verticalLayout_22->addWidget(ui->field_keystroke);

    connect(ui->field_macro_name, SIGNAL(returnPressed()), this, SLOT(UpdateTaskProp()));
    connect(ui->field_delay, SIGNAL(returnPressed()), this, SLOT(UpdateTaskProp()));
    connect(ui->field_repeats, SIGNAL(returnPressed()), this, SLOT(UpdateTaskProp()));
    connect(ui->field_keystroke, SIGNAL(returnPressed()), this, SLOT(UpdateTaskProp()));
    connect(ui->field_x, SIGNAL(returnPressed()), this, SLOT(UpdateTaskProp()));
    connect(ui->field_y, SIGNAL(returnPressed()), this, SLOT(UpdateTaskProp()));
    connect(ui->field_text, SIGNAL(returnPressed()), this, SLOT(UpdateTaskProp()));

    ui->field_delay->setValidator(new QDoubleValidator(0, 99, 4, this));

    MacroMenuWidget = new MacroMenuOptions(this);
    MacroMenuWidget->setVisible(false);
    connect(MacroMenuWidget, SIGNAL(OpenKeybinds()), this, SLOT(OpenKeybinds()));
    connect(MacroMenuWidget, SIGNAL(ExportMacro()), this, SLOT(ExportMacro()));
    //macromenu = new QMenu(MacroMenuWidget);
    //macromenu->setVisible(false);

    overlay = new OverlayWidget(250, OverlayWidget::PwdReset,client, ui->login_page->size(),nullptr, ui->login_page);

    connect(ui->market_user_profile, SIGNAL(clicked(bool)), this, SLOT(ClickedUserAvatar()));
    connect(ui->macro_author, SIGNAL(clicked(bool)), this, SLOT(ClickedUserAvatar()));

    Sequence::stopBtn = ui->executeMacro;

    connect(ui->stackedWidget, SIGNAL(currentChanged(int)), this, SLOT(SwitchedPage(int)));
    connect(ui->search_macros, SIGNAL(returnPressed()), this, SLOT(SearchMacro()));


#endif

}

void MainWindow::CheckChoosing(){
    if(isChoosing){
        isChoosing = false;
    }
}

int MainWindow::GetMacroPos(){
    QLayout *layout = ui->verticalLayout_11;
    int count = layout->count();


    int i = 0;
    for(i; i< count; i++){
        QLayoutItem *item = 0;
        item = layout->itemAt(i);
        if(item){
            MacroFrame* m = dynamic_cast<MacroFrame*>(item->widget());
            if(m != nullptr && m->id != i){
                qDebug(QString::number(m->id).toLocal8Bit());
                qDebug(QString::number(i).toLocal8Bit());
                return i;
            }
        }
    }

    return count;
}

MacroFrame* MainWindow::AddMacroFrame(QString name){
    MacroFrame *macro_sequence;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *seq_more;
    QLabel *seq_name;

    int id = GetMacroPos();


    macro_sequence = new MacroFrame(ui->mlist);
    macro_sequence->setObjectName(QString::fromUtf8("macro_sequence"));
    macro_sequence->setMinimumSize(QSize(0, 70));
    macro_sequence->setMaximumSize(QSize(16777215, 70));
    macro_sequence->setCursor(QCursor(Qt::PointingHandCursor));
    macro_sequence->setStyleSheet(QString::fromUtf8("QFrame{\n"
    "	border-radius:0;\n"
    "}\n"
    "QFrame:hover{\n"
    "	background-color: rgb(44, 44, 44);\n"
    "}"));
    macro_sequence->setFrameShape(QFrame::StyledPanel);
    macro_sequence->setFrameShadow(QFrame::Raised);
    horizontalLayout_5 = new QHBoxLayout(macro_sequence);
    horizontalLayout_5->setSpacing(0);
    horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
    horizontalLayout_5->setContentsMargins(20, 0, 20, 0);
    seq_more = new QPushButton(macro_sequence);
    seq_more->setObjectName(QString::fromUtf8("seq_more"));
    seq_more->setMaximumSize(QSize(20, 20));
    seq_more->setCursor(QCursor(Qt::PointingHandCursor));
    seq_more->setStyleSheet(QString::fromUtf8("background-color: none;"));
    QIcon icon10;
    icon10.addFile(QString::fromUtf8(":/assets/icons/more.svg"), QSize(), QIcon::Normal, QIcon::Off);
    seq_more->setIcon(icon10);
    seq_more->setIconSize(QSize(22, 22));
    seq_more->setFocusPolicy(Qt::NoFocus);
    seq_more->setVisible(false);
    macro_sequence->SetMoreBtn(seq_more);

    horizontalLayout_5->addWidget(seq_more);

    seq_name = new QLabel(macro_sequence);
    seq_name->setObjectName(QString::fromUtf8("seq_name"));
    seq_name->setMinimumSize(QSize(0, 0));
    QFont font6;
    font6.setPointSize(12);
    font6.setBold(true);
    seq_name->setFont(font6);
    seq_name->setStyleSheet(QString::fromUtf8("background-color: none;\n"
    "color: rgb(255, 255, 255);"));
    seq_name->setText(name);

    horizontalLayout_5->addWidget(seq_name);

    QLineEdit* name_change;
    name_change = new QLineEdit(macro_sequence);
    name_change->setObjectName(QString::fromUtf8("name_change"));
    name_change->setStyleSheet(QString::fromUtf8("border: 2px solid #222222;\n"
"border-radius:0; color:#FFFFFF; font-weight: bold;"));
    name_change->setVisible(false);

    horizontalLayout_5->addWidget(name_change);
    macro_sequence->name = name;
    macro_sequence->name_label = seq_name;
    macro_sequence->name_change = name_change;
    macro_sequence->id = id;

    name_change->installEventFilter(this);
    macro_sequence->installEventFilter(this);
    connect(macro_sequence, SIGNAL(destroyed(QObject*)), this, SLOT(MacroFrameDestroyed()));
    connect(seq_more, SIGNAL(clicked(bool)), this, SLOT(Sequence_more_pressed()));
    connect(name_change, SIGNAL(returnPressed()), this, SLOT(ChangeMacroName()));


    ui->verticalLayout_11->insertWidget(id,macro_sequence);

    return macro_sequence;
}

TaskFrame* MainWindow::AddTaskFrame(Task* _task, QWidget* parent,bool readOnly){

    Util::clearSpacers(parent->layout());

    TaskFrame* task;
    QHBoxLayout *horizontalLayout_7;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *pushButton;
    QPushButton *deleteBtn;
    QFrame *line_3;
    QLabel *delayLabel;

    //QListWidgetItem* item;
    //item = new QListWidgetItem(ui->editor_list);
    //ui->editor_list->addItem(item);

    task = new TaskFrame(_task, parent);
    task->setObjectName(QString::fromUtf8("task"));
    task->setMinimumSize(QSize(0, 50));
    task->setMaximumSize(QSize(16777215, 50));
    task->setStyleSheet(QString::fromUtf8("QFrame{\n"
    "	border-radius:0;\n"
    "}\n"
    "QFrame:hover{\n"
    "	background-color: rgb(44, 44, 44);\n"
    "}"));
    task->setFrameShape(QFrame::StyledPanel);
    task->setFrameShadow(QFrame::Raised);
    horizontalLayout_7 = new QHBoxLayout(task);
    horizontalLayout_7->setSpacing(35);
    horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
    horizontalLayout_7->setContentsMargins(10, 0, 10, 0);

    std::string filepath;
    if(!readOnly){
        deleteBtn = new QPushButton(task);
        deleteBtn->setObjectName(QString::fromUtf8("deleteBtn"));
        deleteBtn->setStyleSheet(QString::fromUtf8("background-color:none;"));
        QIcon deleteBtnIcon;
        filepath = std::string(":/assets/icons/delete.svg");
        std::transform(filepath.begin(), filepath.end(), filepath.begin(), ::tolower);
        deleteBtnIcon.addFile(QString::fromUtf8(filepath), QSize(), QIcon::Normal, QIcon::Off);
        deleteBtn->setIcon(deleteBtnIcon);
        deleteBtn->setIconSize(QSize(25, 25));
        deleteBtn->setFlat(true);
        deleteBtn->setCursor(QCursor(Qt::PointingHandCursor));
        horizontalLayout_7->addWidget(deleteBtn);
        deleteBtn->setVisible(false);
        connect(deleteBtn, SIGNAL(clicked(bool)), this, SLOT(DeleteTaskClicked()));
    }


    horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_7->addItem(horizontalSpacer_3);

    pushButton = new QPushButton(task);
    pushButton->setObjectName(QString::fromUtf8("pushButton"));
    pushButton->setStyleSheet(QString::fromUtf8("background-color:none;"));
    QIcon icon4;
    filepath = std::string(":/assets/assets/")+_task->GetTaskName()+std::string(".svg");
    std::transform(filepath.begin(), filepath.end(), filepath.begin(), ::tolower);
    icon4.addFile(QString::fromUtf8(filepath), QSize(), QIcon::Normal, QIcon::Off);
    pushButton->setIcon(icon4);
    pushButton->setIconSize(QSize(40, 40));
    pushButton->setFlat(true);
    pushButton->setFocusPolicy(Qt::NoFocus);

    horizontalLayout_7->addWidget(pushButton);

    line_3 = new QFrame(task);
    line_3->setObjectName(QString::fromUtf8("line_3"));
    line_3->setMinimumSize(QSize(2, 35));
    line_3->setMaximumSize(QSize(2, 35));
    line_3->setStyleSheet(QString::fromUtf8("background-color: rgb(18, 18, 18);"));
    line_3->setFrameShape(QFrame::StyledPanel);
    line_3->setFrameShadow(QFrame::Raised);

    horizontalLayout_7->addWidget(line_3);

    delayLabel = new QLabel(task);
    delayLabel->setObjectName(QString::fromUtf8("delayLabel"));
    delayLabel->setMinimumSize(QSize(50, 0));
    delayLabel->setMaximumSize(QSize(50, 16777215));
    QFont font4;
    font4.setBold(true);
    delayLabel->setFont(font4);
    delayLabel->setStyleSheet(QString::fromUtf8("background-color: none;\n"
    "color: rgb(255, 255, 255);"));
    delayLabel->setText(QString(QString::number(_task->delay)+"s"));

    horizontalLayout_7->addWidget(delayLabel);


    //List stuff
    //item->setSizeHint(task->minimumSizeHint());
    //item->setSizeHint(QSize(task->width(),task->height())); //When minimumSizeHint doesn't work
    //ui->editor_list->setItemWidget(item, task);

    parent->layout()->addWidget(task);

    QSpacerItem *verticalSpacer;
    verticalSpacer = new QSpacerItem(20, 438, QSizePolicy::Minimum, QSizePolicy::Expanding);

    parent->layout()->addItem(verticalSpacer);

    if(!readOnly)
        task->SetDelay(delayLabel);

    task->installEventFilter(this);

    return task;
}

LoopTaskFrame* MainWindow::AddLoopTaskFrame(Task* _task, QWidget* parent, bool readOnly){

    Util::clearSpacers(parent->layout());

    LoopTaskFrame* task;
    QHBoxLayout *horizontalLayout_7;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *pushButton;
    QPushButton *deleteBtn;
    QFrame *line_3;
    QLabel *delayLabel;

    QFrame *frame_36;
    QVBoxLayout *verticalLayout_73;
    QFrame *frame_49;
    QFrame *frame_50;
    QHBoxLayout *horizontalLayout_29;
    QFrame *frame_61;
    QVBoxLayout *verticalLayout_75;
    QWidget *loop_list;
    QVBoxLayout *verticalLayout_74;
    QFrame *frame_60;

    //QListWidgetItem* item;
    //item = new QListWidgetItem(ui->editor_list);
    //ui->editor_list->addItem(item);

    frame_36 = new QFrame(parent);
    frame_36->setObjectName(QString::fromUtf8("frame_36"));
    frame_36->setFrameShape(QFrame::StyledPanel);
    frame_36->setFrameShadow(QFrame::Raised);

    verticalLayout_73 = new QVBoxLayout(frame_36);
    verticalLayout_73->setSpacing(0);
    verticalLayout_73->setObjectName(QString::fromUtf8("verticalLayout_73"));
    verticalLayout_73->setContentsMargins(0, 0, 0, 0);

    task = new LoopTaskFrame(_task, frame_36);
    task->setObjectName(QString::fromUtf8("task"));
    task->setMinimumSize(QSize(0, 50));
    task->setMaximumSize(QSize(16777215, 50));
    task->setStyleSheet(QString::fromUtf8("QFrame{\n"
    "	border-radius:0;\n"
    "}\n"
    "QFrame:hover{\n"
    "	background-color: rgb(44, 44, 44);\n"
    "}"));
    task->setFrameShape(QFrame::StyledPanel);
    task->setFrameShadow(QFrame::Raised);
    horizontalLayout_7 = new QHBoxLayout(task);
    horizontalLayout_7->setSpacing(35);
    horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
    horizontalLayout_7->setContentsMargins(10, 0, 10, 0);

    std::string filepath;
    if(!readOnly){
        deleteBtn = new QPushButton(task);
        deleteBtn->setObjectName(QString::fromUtf8("deleteBtn"));
        deleteBtn->setStyleSheet(QString::fromUtf8("background-color:none;"));
        QIcon deleteBtnIcon;
        filepath = std::string(":/assets/icons/delete.svg");
        std::transform(filepath.begin(), filepath.end(), filepath.begin(), ::tolower);
        deleteBtnIcon.addFile(QString::fromUtf8(filepath), QSize(), QIcon::Normal, QIcon::Off);
        deleteBtn->setIcon(deleteBtnIcon);
        deleteBtn->setIconSize(QSize(25, 25));
        deleteBtn->setFlat(true);
        deleteBtn->setCursor(QCursor(Qt::PointingHandCursor));
        horizontalLayout_7->addWidget(deleteBtn);
        deleteBtn->setVisible(false);
        connect(deleteBtn, SIGNAL(clicked(bool)), this, SLOT(DeleteTaskClicked()));
    }


    horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout_7->addItem(horizontalSpacer_3);

    pushButton = new QPushButton(task);
    pushButton->setObjectName(QString::fromUtf8("pushButton"));
    pushButton->setStyleSheet(QString::fromUtf8("background-color:none;"));
    QIcon icon4;
    filepath = std::string(":/assets/assets/")+_task->GetTaskName()+std::string(".svg");
    std::transform(filepath.begin(), filepath.end(), filepath.begin(), ::tolower);
    icon4.addFile(QString::fromUtf8(filepath), QSize(), QIcon::Normal, QIcon::Off);
    pushButton->setIcon(icon4);
    pushButton->setIconSize(QSize(40, 40));
    pushButton->setFlat(true);
    pushButton->setFocusPolicy(Qt::NoFocus);

    horizontalLayout_7->addWidget(pushButton);

    line_3 = new QFrame(task);
    line_3->setObjectName(QString::fromUtf8("line_3"));
    line_3->setMinimumSize(QSize(2, 35));
    line_3->setMaximumSize(QSize(2, 35));
    line_3->setStyleSheet(QString::fromUtf8("background-color: rgb(18, 18, 18);"));
    line_3->setFrameShape(QFrame::StyledPanel);
    line_3->setFrameShadow(QFrame::Raised);

    horizontalLayout_7->addWidget(line_3);

    delayLabel = new QLabel(task);
    delayLabel->setObjectName(QString::fromUtf8("delayLabel"));
    delayLabel->setMinimumSize(QSize(50, 0));
    delayLabel->setMaximumSize(QSize(50, 16777215));
    QFont font4;
    font4.setBold(true);
    delayLabel->setFont(font4);
    delayLabel->setStyleSheet(QString::fromUtf8("background-color: none;\n"
    "color: rgb(255, 255, 255);"));
    delayLabel->setText(QString(QString::number(_task->delay)+"s"));

    horizontalLayout_7->addWidget(delayLabel);


    //List stuff
    //item->setSizeHint(task->minimumSizeHint());
    //item->setSizeHint(QSize(task->width(),task->height())); //When minimumSizeHint doesn't work
    //ui->editor_list->setItemWidget(item, task);

    verticalLayout_73->addWidget(task);

    frame_50 = new QFrame(frame_36);
    frame_50->setObjectName(QString::fromUtf8("frame_50"));
    frame_50->setFrameShape(QFrame::StyledPanel);
    frame_50->setFrameShadow(QFrame::Raised);
    horizontalLayout_29 = new QHBoxLayout(frame_50);
    horizontalLayout_29->setSpacing(0);
    horizontalLayout_29->setObjectName(QString::fromUtf8("horizontalLayout_29"));
    horizontalLayout_29->setContentsMargins(0, 0, 0, 0);
    frame_61 = new QFrame(frame_50);
    frame_61->setObjectName(QString::fromUtf8("frame_61"));
    frame_61->setFrameShape(QFrame::StyledPanel);
    frame_61->setFrameShadow(QFrame::Raised);
    verticalLayout_75 = new QVBoxLayout(frame_61);
    verticalLayout_75->setSpacing(0);
    verticalLayout_75->setObjectName(QString::fromUtf8("verticalLayout_75"));
    verticalLayout_75->setContentsMargins(0, 0, 0, 0);

    loop_list = new QWidget(frame_61);
    loop_list->setObjectName(QString::fromUtf8("loop_list"));
    loop_list->setGeometry(QRect(0, 0, 68, 452));
    loop_list->setLayoutDirection(Qt::RightToLeft);


    verticalLayout_74 = new QVBoxLayout(loop_list);
    verticalLayout_74->setSpacing(0);
    verticalLayout_74->setObjectName(QString::fromUtf8("verticalLayout_74"));
    verticalLayout_74->setContentsMargins(0, 0, 0, 0);

    verticalLayout_75->addWidget(loop_list);


    horizontalLayout_29->addWidget(frame_61);

    frame_60 = new QFrame(frame_50);
    frame_60->setObjectName(QString::fromUtf8("frame_60"));
    frame_60->setMinimumSize(QSize(100, 0));
    frame_60->setMaximumSize(QSize(100, 20));
    frame_60->setFrameShape(QFrame::StyledPanel);
    frame_60->setFrameShadow(QFrame::Raised);

    horizontalLayout_29->addWidget(frame_60);

    //frame_50->setVisible(false);


    verticalLayout_73->addWidget(frame_50);


    parent->layout()->addWidget(frame_36);

    QSpacerItem *verticalSpacer;
    verticalSpacer = new QSpacerItem(20, 438, QSizePolicy::Minimum, QSizePolicy::Expanding);

    parent->layout()->addItem(verticalSpacer);

    if(!readOnly)
        task->SetDelay(delayLabel);

    task->SetTaskList(loop_list);
    task->sub = frame_50;
    task->installEventFilter(this);

    return task;
}

TaskFrame* MainWindow::AddTask(Task* task, QWidget* parent, bool readOnly){
    if(task->type == Loop){
        LoopTaskFrame* loopframe = MainWindow::AddLoopTaskFrame(task, parent, readOnly);
        LoopTask* l = dynamic_cast<LoopTask*>(task);
        foreach (auto task, l->tasks) {
            TaskFrame* t = AddTask(task, loopframe->GetTaskList(), readOnly);
            t->SetLoop(l);
        }
        Util::clearSpacers(loopframe->GetTaskList()->layout());
        return loopframe;
    }
    return MainWindow::AddTaskFrame(task, parent, readOnly);
}

void MainWindow:: MacroActionClick(Task* task){
    if(MainWindow::selectedMacro != nullptr){
        if(task->type == Mouse){
            MouseClick* m = dynamic_cast<MouseClick*>(task);
            m->SetKeystroke(fixCode[m->keystroke]);
        }
        QWidget* parent = ui->editor_list;
        bool inLoop;
        LoopTask* m;
        TaskFrame* t;

        if(recording){
            high_resolution_clock::time_point currentMS = high_resolution_clock::now();
            duration<double, std::milli> time = currentMS - recordMS;
            qDebug()<<time.count();

            recordMS = currentMS;

            double taskdelay = (double)time.count()/1000;
            taskdelay = std::ceil(taskdelay * 100.0) / 100.0;
            task->SetDelay(taskdelay);
        }


        if(selectedTask != nullptr && selectedTask->GetTask()->type == Loop){
            LoopTaskFrame* l = dynamic_cast<LoopTaskFrame*>(selectedTask);
            m = dynamic_cast<LoopTask*>(l->GetTask());

            l->sub->setVisible(true);
            inLoop = true;

            parent = l->GetTaskList();
        }
        if(task->type == Loop){
            t = MainWindow::AddLoopTaskFrame(task, parent);
        }
        else{
            t = MainWindow::AddTaskFrame(task, parent);
        }
        if(inLoop){
            m->Insert(task);
            Util::clearSpacers(parent->layout());
            t->SetLoop(m);
        }
        else
            selectedMacro->GetSequence()->Insert(task);
        qDebug("here");
        MainWindow::SaveMacro(MainWindow::selectedMacro, MainWindow::macropath);
    }
}

bool MainWindow::SaveMacro(MacroFrame* m, std::wstring path){
    QString filename = QString::fromStdWString(path) + "\\macros.json";
    QString json_string;

    for(int i = 0; i< macrosJson.count(); i++){
        QJsonObject d = macrosJson[i].toObject();
        foreach(const QString& key, d.keys()) {
              QJsonValue value = d.value(key);
              if(key.toInt() == m->id){
                  macrosJson.removeAt(i);
              }
        }
    }

    QJsonObject root_macro_obj;

    QJsonObject macro_obj;
    macro_obj["name"] = m->name;


    Sequence* s = m->GetSequence();
    QJsonObject sequence_obj = s->Serialize();


    macro_obj.insert("sequence", sequence_obj);
    root_macro_obj.insert(QString::number(m->id),macro_obj);

    macrosJson.push_back(root_macro_obj);

    QJsonDocument json_doc(macrosJson);
    json_string = json_doc.toJson();

    QFile save_file(filename);
    if(!save_file.open(QIODevice::WriteOnly)){
        qDebug("failed to open save file");
        return false;
    }
    save_file.write(json_string.toLocal8Bit());
    save_file.close();
    return true;
}

bool MainWindow::SaveAllMacros(){
    QString filename = QString::fromStdWString(macropath) + "\\macros.json";
    QFile file(filename);
    file.open(QFile::WriteOnly|QFile::Truncate);
    file.close();

    QString json_string;

    QJsonDocument json_doc(macrosJson);
    json_string = json_doc.toJson();

    QFile save_file(filename);
    if(!save_file.open(QIODevice::WriteOnly)){
        qDebug("failed to open save file");
        return false;
    }
    save_file.write(json_string.toLocal8Bit());
    save_file.close();
    return true;
}

void MainWindow::AddMarketMacros(QJsonDocument macros, QLayout* l){
    Util::clearWidgets(l);

    QJsonArray jsonArray = macros.array();

    QScrollBar* scrollbar = ui->market_macros_scroll->horizontalScrollBar();
    scrollbar->setValue(scrollbar->maximum());

    foreach (const QJsonValue & value, jsonArray) {
        QJsonObject obj = value.toObject();

        Macro* macro = new Macro((obj["id"].toString()), QString::number(obj["downloads"].toInt()),(obj["name"].toString()),(obj["description"].toString()),(obj["user"].toString()),(obj["macro"].toString()));
        MarketMacroFrame* m = new MarketMacroFrame(macro,client, ui->market_macros);
        l->addWidget(m);
        connect(m, SIGNAL(ExplorePressed()), this, SLOT(ExploreMacro()));
        connect(m->avatar, SIGNAL(clicked(bool)), this, SLOT(ClickedUserAvatar()));
    }
    QSpacerItem *spacer;
    spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    l->addItem(spacer);

    if(isSearching && l->count() == 1){
        isSearching = false;
        SetUpMarket();
    }
}

void MainWindow::SetUpMarket(){
    ui->market_user_profile->setIcon(ui->user_avatar->icon());
    ui->market_user_profile->setObjectName(client->user->id);

    AddMarketMacros(client->GetMacros(QString::number(macroPage)), ui->market_macros->layout());
}

void MainWindow::NewMacroFrame(QString name, Sequence *seq){
    Util::clearSpacers(ui->verticalLayout_11);
    if(selectedMacro != nullptr){
        selectedMacro->setStyleSheet(QString::fromUtf8("QFrame{\n"
        "	border-radius:0;\n"
        "}\n"
        "QFrame:hover{\n"
        "	background-color: rgb(44, 44, 44);\n"
        "}"));
        selectedMacro->GetOptionsBtn()->setVisible(false);
    }

    selectedMacro = AddMacroFrame(name);
    selectedMacro->setStyleSheet(QString::fromUtf8("QFrame{\n"
     "	border-radius:0; background-color: rgb(44, 44, 44);\n"
     "}"));

    selectedMacro->GetOptionsBtn()->setVisible(true);

    selectedMacro->SetSequence(seq);

    QSpacerItem* mlist_spacer;
    mlist_spacer = new QSpacerItem(20, 310, QSizePolicy::Minimum, QSizePolicy::Expanding);
    ui->verticalLayout_11->addItem(mlist_spacer);

    ui->selected_error->setVisible(false);
    Util::clearWidgets(ui->editor_list->layout());
    selectedTask = nullptr;
    ui->prop_body->setVisible(false);

    foreach (auto task, selectedMacro->GetSequence()->tasks) {
        AddTask(task, ui->editor_list);
    }

    QSpacerItem *verticalSpacer;
    verticalSpacer = new QSpacerItem(20, 438, QSizePolicy::Minimum, QSizePolicy::Expanding);

    ui->editor_list->layout()->addItem(verticalSpacer);

}

void MainWindow::AddCommentFrame(Comment comment){
    User u = client->GetUser(comment.user);

    QFrame* frame_48;
    QHBoxLayout* horizontalLayout_28;
    QFrame* frame_34;
    QVBoxLayout* verticalLayout_63;
    QLabel* label_10;
    QFrame* frame_35;
    QTextEdit* textEdit;
    QVBoxLayout* verticalLayout_62;
    QPushButton* comment_avatar_2;


    frame_48 = new QFrame(ui->comments_frame);
    frame_48->setObjectName(QString::fromUtf8("frame_48"));
    frame_48->setMinimumSize(QSize(0, 60));
    frame_48->setMaximumSize(QSize(16777215, 16777215));
    frame_48->setStyleSheet(QString::fromUtf8("background-color: rgb(50, 50, 50);"));
    frame_48->setFrameShape(QFrame::NoFrame);
    frame_48->setFrameShadow(QFrame::Raised);
    horizontalLayout_28 = new QHBoxLayout(frame_48);
    horizontalLayout_28->setSpacing(0);
    horizontalLayout_28->setObjectName(QString::fromUtf8("horizontalLayout_28"));
    horizontalLayout_28->setContentsMargins(0, 0, 0, 0);
    frame_34 = new QFrame(frame_48);
    frame_34->setObjectName(QString::fromUtf8("frame_34"));
    frame_34->setFrameShape(QFrame::NoFrame);
    frame_34->setFrameShadow(QFrame::Raised);
    verticalLayout_63 = new QVBoxLayout(frame_34);
    verticalLayout_63->setSpacing(0);
    verticalLayout_63->setObjectName(QString::fromUtf8("verticalLayout_63"));
    verticalLayout_63->setContentsMargins(5, 3, 0, 0);
    label_10 = new QLabel(frame_34);
    label_10->setObjectName(QString::fromUtf8("label_10"));
    QFont font16;
    font16.setFamily(QString::fromUtf8("Segoe UI"));
    font16.setPointSize(10);
    font16.setBold(true);
    label_10->setFont(font16);
    label_10->setStyleSheet(QString::fromUtf8("color: rgb(204, 204, 204);"));
    label_10->setText(u.name);


    verticalLayout_63->addWidget(label_10);

    textEdit = new QTextEdit(frame_34);
    textEdit->setObjectName(QString::fromUtf8("textEdit"));
    QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Minimum);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(textEdit->sizePolicy().hasHeightForWidth());
    textEdit->setSizePolicy(sizePolicy1);
    textEdit->setMinimumSize(QSize(0, 40));
    textEdit->setMaximumSize(QSize(16777215, 100));
    textEdit->setStyleSheet(QString::fromUtf8("color: rgb(184, 184, 184);"));
    textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    textEdit->setLineWrapMode(QTextEdit::WidgetWidth);
    textEdit->setReadOnly(true);
    textEdit->setText(comment.content);

    verticalLayout_63->addWidget(textEdit);


    horizontalLayout_28->addWidget(frame_34);

    frame_35 = new QFrame(frame_48);
    frame_35->setObjectName(QString::fromUtf8("frame_35"));
    frame_35->setMaximumSize(QSize(60, 60));
    frame_35->setFrameShape(QFrame::NoFrame);
    frame_35->setFrameShadow(QFrame::Raised);
    verticalLayout_62 = new QVBoxLayout(frame_35);
    verticalLayout_62->setSpacing(0);
    verticalLayout_62->setObjectName(QString::fromUtf8("verticalLayout_62"));
    verticalLayout_62->setContentsMargins(5, 0, 5, 0);
    comment_avatar_2 = new QPushButton(frame_35);
    comment_avatar_2->setObjectName(u.id);
    comment_avatar_2->setMinimumSize(QSize(50, 50));
    comment_avatar_2->setMaximumSize(QSize(50, 50));
    comment_avatar_2->setIconSize(QSize(50, 50));
    comment_avatar_2->setFlat(true);
    comment_avatar_2->setFocusPolicy(Qt::NoFocus);

    verticalLayout_62->addWidget(comment_avatar_2);


    horizontalLayout_28->addWidget(frame_35);

    QSpacerItem *verticalSpacer;
    verticalSpacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

    comment_avatar_2->setIcon(client->GetIcon(u.avatar, comment_avatar_2->size()));

    ui->comments_frame->layout()->addWidget(frame_48);
    Util::clearSpacers(ui->comments_frame->layout());
    ui->comments_frame->layout()->addItem(verticalSpacer);


    connect(comment_avatar_2, SIGNAL(clicked(bool)), this, SLOT(ClickedUserAvatar()));
}

void MainWindow::RegisterSequenceKey(Sequence* seq){
    int key = Util::ScanToVirtual(seq->GetKeybind());
    qDebug("registering....");
    qDebug()<<key;
    emit RegisterKey(NO_MOD, key);
    registeredKey = key;
}

void MainWindow::PressedHotkey(WORD key)
{
    qDebug("pressed hotkey");
    if((int)key == 0x52)
        on_recordMacro_clicked(true);
    else if((int)key == registeredKey)
        on_executeMacro_clicked();
}

//----------------PUBLIC SLOTS-------------------

void MainWindow::UpdateTaskProp(){
    Task* t = selectedTask->GetTask();
    QLineEdit* l = qobject_cast<QLineEdit*>(QObject::sender());
    if(l == ui->field_macro_name){
        QString name = l->text();
        t->SetName(name);
        ui->field_macro_name->setPlaceholderText(name);
        //QLabel* name = selectedTask->GetName();
        //name->setText(l->text());
    }
    else if(l == ui->field_delay){
        t->SetDelay(l->text().toDouble());
        QLabel* delay = selectedTask->GetDelay();
        delay->setText(l->text() + QString("s"));
        ui->field_delay->setPlaceholderText(l->text());
    }
    else if(l == ui->field_repeats){
        t->SetRepeats(l->text().toInt());
        ui->field_repeats->setPlaceholderText(l->text());
    }
    else if(l == ui->field_keystroke){
        switch(t->type){
            case(Keyboard):
                {
                    KeyboardPress* p = dynamic_cast<KeyboardPress*>(t);
                    SHORT key = VkKeyScanExA(l->text().toStdString()[0], p->myKL);
                    p->SetKeystroke(key);
                    ui->field_keystroke->setPlaceholderText(l->text());
                }
                break;
            case(Mouse):
                {
                    //MouseClick* p = dynamic_cast<MouseClick*>(t);
                    //SHORT key = VkKeyScanExA(l->text().toStdString()[0], p->myKL);
                    //p->SetKeystroke(key);
                }
                break;
        }
    }
    else if(l == ui->field_x){
        MouseClick* p = dynamic_cast<MouseClick*>(t);
        p->SetMouse(l->text().toInt(), p->y);
        l->setPlaceholderText(l->text());
    }
    else if(l == ui->field_y){
        MouseClick* p = dynamic_cast<MouseClick*>(t);
        p->SetMouse(p->x, l->text().toInt());
        l->setPlaceholderText(l->text());
    }
    else if(l == ui->field_text){
        switch(t->type){
            case(Text):
                {
                    KeyboardType* p = dynamic_cast<KeyboardType*>(t);
                    p->text = l->text();
                    l->setPlaceholderText(l->text());
                }
                break;
            case(Cmd):
                {
                    CmdTask* p = dynamic_cast<CmdTask*>(t);
                    p->cmd = l->text();
                    l->setPlaceholderText(l->text());
                }
                break;
        }
    }
    l->clear();
}

void MainWindow::Sequence_more_pressed(){
    MacroMenuWidget->setVisible(true);
    POINT p;
    if (GetCursorPos(&p))
    {
       MacroMenuWidget->move(p.x,p.y);
    }
    MacroMenuWidget->SetMacroFrame(selectedMacro);
}

void MainWindow::MacroFrameDestroyed(){
    selectedMacro = nullptr;

    emit UnRegisterKey();

    Util::clearWidgets(ui->editor_list->layout());
    selectedTask = nullptr;
    ui->prop_body->setVisible(false);

    if(ui->mlist->layout()->count() == 2){
        ui->selected_error->setVisible(true);
    }

    qDebug(std::to_string(MacroFrame::deletedMacro).c_str());

    for(int i = 0; i< macrosJson.count(); i++){
        QJsonObject d = macrosJson[i].toObject();
        foreach(const QString& key, d.keys()) {
              QJsonValue value = d.value(key);
              if(key.toInt() == MacroFrame::deletedMacro){
                  macrosJson.removeAt(i);
              }
        }
    }

    SaveAllMacros();
}

void MainWindow::ChangeMacroName(){
    selectedMacro->name = selectedMacro->name_change->text();
    selectedMacro->name_label->setVisible(true);
    selectedMacro->name_label->setText(selectedMacro->name);
    selectedMacro->name_change->setVisible(false);
}

void MainWindow::DeleteTaskClicked(){
    if(selectedTask->GetTask()->type == Loop){
        LoopTaskFrame* l = dynamic_cast<LoopTaskFrame*>(selectedTask);
        Util::clearWidgets(l->GetTaskList()->layout());
    }
    if(selectedTask == nullptr)
        return;
    selectedMacro->GetSequence()->RemoveTask(selectedTask->GetTask());
    selectedTask->DeleteFromLoop();
    selectedTask->deleteLater();
    selectedTask = nullptr;
    ui->prop_body->setVisible(false);

    SaveMacro(selectedMacro, macropath);
}

void MainWindow::ExploreMacro(){
    MarketMacroFrame* m = dynamic_cast<MarketMacroFrame*>(sender());

    marketMacro = m;
    Macro* macro = m->macro;

    ui->comment_avatar->setPixmap(ui->user_avatar->icon().pixmap(ui->comment_avatar->size()));
    ui->macro_author->setIcon(m->avatar->icon());
    ui->macro_author->setObjectName(macro->user);

    ui->macro_downloads->setText(macro->downloads);
    ui->macro_desc->setText(macro->description);
    ui->macro_name->setText(macro->name);

    ui->macro_explore_prop->setVisible(false);

    QJsonObject macro_obj = QJsonDocument::fromJson(macro->macro.toUtf8()).object();

    QLayout* l =  ui->macro_explore_list->layout();
    Util::clearWidgets(l);

    m->seq = Sequence::Deserialize(macro_obj, true);

    foreach (auto task, m->seq->tasks) {
        AddTask(task, ui->macro_explore_list, true);
    }

    QJsonDocument comments = client->GetComments(macro->id, QString::number(commentPage));
    QJsonArray jsonArray = comments.array();

    Util::clearWidgets(ui->comments_frame->layout());

    foreach (const QJsonValue & value, jsonArray) {
        QJsonObject obj = value.toObject();

        Comment comment((obj["id"].toString()), (obj["user"].toString()),(obj["content"].toString()));
        AddCommentFrame(comment);
    }

    ui->stackedWidget->setCurrentIndex(6);
}

void MainWindow::ClickedUserAvatar(){
    QPushButton* b = dynamic_cast<QPushButton*>(sender());
    b->disconnect();

    User u = client->GetUser(b->objectName());

    QSize s = ui->macro_profile_avatar->size();
    ui->macro_profile_avatar->setPixmap(client->GetIcon(u.avatar, s).pixmap(s));
    ui->macro_profile_name->setText(u.name);
    ui->macro_profile_about->setText(u.about);

    QJsonDocument macros = client->GetUserMacros(u.id);
    QJsonArray jsonArray = macros.array();

    QListWidget* list = ui->user_macros;
    list->clear();
    list->setSelectionMode(QAbstractItemView::NoSelection);

    foreach (const QJsonValue & value, jsonArray) {
        QJsonObject obj = value.toObject();

        QListWidgetItem* item = new QListWidgetItem(list);
        list->addItem(item);

        Macro* macro = new Macro((obj["id"].toString()), QString::number(obj["downloads"].toInt()),(obj["name"].toString()),(obj["description"].toString()),(obj["user"].toString()),(obj["macro"].toString()));
        MarketMacroFrame* m = new MarketMacroFrame(macro, client, list);

        m->avatar->setVisible(false);

        connect(m, SIGNAL(ExplorePressed()), this, SLOT(ExploreMacro()));

        item->setSizeHint(m->minimumSizeHint());
        //item->setSizeHint(QSize(m->width(),m->height()));
        list->setCurrentIndex(QModelIndex());

        list->setItemWidget(item, m);
    }


    ui->stackedWidget->setCurrentIndex(7);
    connect(b, SIGNAL(clicked(bool)), this, SLOT(ClickedUserAvatar()));
}

void MainWindow::OpenKeybinds(){
    overlay->deleteLater();
    overlay = new OverlayWidget(250, OverlayWidget::Keybinds, client, ui->library_page->size(), selectedMacro->GetSequence(), ui->library_page);
    overlay->show(true);
    connect(overlay, SIGNAL(ChangedKeybind(int)), this, SLOT(ChangedKeybind()));
}

void MainWindow::ExportMacro(){
    if(ui->stackedWidget->currentIndex() != 1)
        return;

    WCHAR* path;
    auto get_folder_path_ret = SHGetKnownFolderPath(FOLDERID_Downloads, 0, nullptr, &path);

    if (get_folder_path_ret != S_OK) {
        CoTaskMemFree(path);
    }

    QJsonObject root_macro_obj;
    root_macro_obj["id"] = selectedMacro->id;
    root_macro_obj["name"] = selectedMacro->name;

    Sequence* s = selectedMacro->GetSequence();
    QJsonObject sequence_obj = s->Serialize();

    root_macro_obj.insert("sequence",sequence_obj);

    QJsonDocument json_doc(root_macro_obj);
    QString json_string = json_doc.toJson();

    QString filename;
    filename = QFileDialog::getSaveFileName(this, "Export Macro", QString("") + "\\" + selectedMacro->name +".json", "JSON Files (*.json)");

    QFile f(filename);
    if(!f.open(QIODevice::WriteOnly)){
        qDebug("failed to open save file");
    }
    f.write(json_string.toLocal8Bit());
    f.close();

    CoTaskMemFree(path);
}

void MainWindow::ChangedKeybind(){
    RegisterSequenceKey(selectedMacro->GetSequence());
    SaveMacro(selectedMacro, macropath);
}

void MainWindow::SwitchedPage(int page){
    switch(currentPage){
        case 1:
            emit UnRegisterKey();
            qDebug("unregistered");
        break;
        case 4:
            ui->log_email->clear();
            ui->log_password->clear();
        break;
        case 5:
            ui->reg_email->clear();
            ui->reg_username->clear();
            ui->reg_password->clear();
        break;
    }

    currentPage = page;
}

void MainWindow::SearchMacro(){
    QString s = ui->search_macros->text();
    if(!isSearching)
        searchPage = 0;

    if(s.isEmpty()){
        macroPage = 0;
        isSearching = false;
        AddMarketMacros(client->GetMacros(QString::number(macroPage)), ui->market_macros->layout());
    }
    else{
        isSearching = true;
        AddMarketMacros(client->SearchMacros(s,QString::number(searchPage)), ui->market_macros->layout());
    }
}

//----------------PRIVATE SLOTS------------------

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if(selectedMacro != nullptr && obj == selectedMacro->name_change){
        if(event->type() == QEvent::FocusOut)
            {
               selectedMacro->name_label->setVisible(true);
               selectedMacro->name_change->setVisible(false);
            }
        return QObject::eventFilter(obj, event);
    }
    else if (event->type() == QEvent::MouseButtonRelease) {
            QString type = obj->metaObject()->className();
            if(type == "MacroFrame"){
                MacroFrame* qf = (MacroFrame*)obj;
                if(isChoosing){
                    ui->stackedWidget->setCurrentIndex(8);
                    chosenMacro = qf;
                    ui->upload_name->setText(chosenMacro->name);
                    isChoosing = false;
                    return true;
                }
                if(selectedMacro != qf){
                   if(selectedMacro!=nullptr){
                       selectedMacro->setStyleSheet(QString::fromUtf8("QFrame{\n"
                       "	border-radius:0;\n"
                       "}\n"
                       "QFrame:hover{\n"
                       "	background-color: rgb(44, 44, 44);\n"
                       "}"));
                       selectedMacro->GetOptionsBtn()->setVisible(false);

                       SaveMacro(selectedMacro, macropath);
                    }
                    qf->setStyleSheet(QString::fromUtf8("QFrame{\n"
                    "	border-radius:0; background-color: rgb(44, 44, 44);\n"
                    "}"));
                    selectedMacro = qf;
                    selectedMacro->GetOptionsBtn()->setVisible(true);

                    Util::clearWidgets(ui->editor_list->layout());

                    selectedTask = nullptr;
                    ui->prop_body->setVisible(false);

                    foreach (auto task, selectedMacro->GetSequence()->tasks) {
                        AddTask(task, ui->editor_list);
                    }

                    QSpacerItem *verticalSpacer;
                    verticalSpacer = new QSpacerItem(20, 438, QSizePolicy::Minimum, QSizePolicy::Expanding);

                    ui->editor_list->layout()->addItem(verticalSpacer);

                    RegisterSequenceKey(selectedMacro->GetSequence());
                 }
            }
            else if(type == "TaskFrame"){
                if(ui->stackedWidget->currentIndex() == 6){
                    Util::HandleTaskProp(ui, ((TaskFrame*)obj)->GetTask(), true);
                    ui->macro_explore_prop->setVisible(true);
                    return true;
                }
                TaskFrame* qf = (TaskFrame*)obj;
                if(selectedTask != qf){
                    if(selectedTask == nullptr){
                        selectedTask = qf;
                        selectedTask->setStyleSheet(QString::fromUtf8("QFrame{\n"
                        "	border-radius:0;background-color: rgb(44, 44, 44);"
                        "}"));
                    }
                    else{
                        selectedTask->setStyleSheet(QString::fromUtf8("QFrame{\n"
                        "	border-radius:0;\n"
                        "}\n"
                        "QFrame:hover{\n"
                        "	background-color: rgb(44, 44, 44);\n"
                        "}"));
                        QPushButton* btn = selectedTask->findChild<QPushButton*>(QString("deleteBtn"));
                        btn->setVisible(false);

                        qf->setStyleSheet(QString::fromUtf8("QFrame{\n"
                        "	border-radius:0;background-color: rgb(44, 44, 44);"
                        "}"));
                        selectedTask = qf;
                    }
                    QPushButton* btn = selectedTask->findChild<QPushButton*>(QString("deleteBtn"));
                    btn->setVisible(true);

                    Task* t = selectedTask->GetTask();
                    ui->prop_body->setVisible(true);

                    Util::HandleTaskProp(ui, t);
                }
            }
            return true;
    }
    else {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }
}

void MainWindow::on_mouseclick_clicked()
{
    MacroActionClick(new MouseClick(0,0, 2, "mouseclick", 1, 1));
}

void MainWindow::on_home_btn_pressed()
{
    CheckChoosing();
    Util::NavbarBtnPressed(ui, 0);
    ui->windowBar->setVisible(false);
}

void MainWindow::on_library_btn_pressed()
{
    CheckChoosing();
    Util::NavbarBtnPressed(ui, 1);
    ui->windowBar->setVisible(true);
    if(selectedMacro!=nullptr)
        RegisterSequenceKey(selectedMacro->GetSequence());
}

void MainWindow::on_market_btn_pressed()
{
    if(!client->logged){
        ui->windowBar->setVisible(false);
        ui->stackedWidget->setCurrentIndex(4);
        Util::ResetErrors(ui);
        return;
    }
    CheckChoosing();

    if(ui->stackedWidget->currentIndex() == 2)
        return;

    SetUpMarket();
    Util::NavbarBtnPressed(ui, 2);
    ui->windowBar->setVisible(false);
}

void MainWindow::on_add_macro_pressed()
{
    Util::clearSpacers(ui->verticalLayout_11);
    if(selectedMacro != nullptr){
        selectedMacro->setStyleSheet(QString::fromUtf8("QFrame{\n"
        "	border-radius:0;\n"
        "}\n"
        "QFrame:hover{\n"
        "	background-color: rgb(44, 44, 44);\n"
        "}"));
        selectedMacro->GetOptionsBtn()->setVisible(false);

        SaveMacro(selectedMacro, macropath);
    }
    int id = GetMacroPos();
    string name = "Macro " + to_string(id+1);
    selectedMacro = AddMacroFrame(QString::fromStdString(name));
    selectedMacro->setStyleSheet(QString::fromUtf8("QFrame{\n"
     "	border-radius:0; background-color: rgb(44, 44, 44);\n"
     "}"));

    selectedMacro->GetOptionsBtn()->setVisible(true);

    QSpacerItem* mlist_spacer;
    mlist_spacer = new QSpacerItem(20, 310, QSizePolicy::Minimum, QSizePolicy::Expanding);
    ui->verticalLayout_11->addItem(mlist_spacer);

    ui->selected_error->setVisible(false);
    Util::clearWidgets(ui->editor_list->layout());
    selectedTask = nullptr;
    ui->prop_body->setVisible(false);

    QSpacerItem *verticalSpacer;
    verticalSpacer = new QSpacerItem(20, 438, QSizePolicy::Minimum, QSizePolicy::Expanding);

    ui->editor_list->layout()->addItem(verticalSpacer);

}

void MainWindow::on_keyboardpress_clicked()
{
    MacroActionClick(new KeyboardPress('0x41', "keyboardpress", 1,1));
}

void MainWindow::on_text_clicked()
{
    MacroActionClick(new KeyboardType("","keyboardtype", 1,1));
}

void MainWindow::on_loop_clicked()
{
    MacroActionClick(new LoopTask("loop", 1,1));
}

void MainWindow::on_executeMacro_clicked()
{
    if(recording)
        return;
    QIcon icon;
    std::string filepath;

    if(!selectedMacro->GetSequence()->IsRunning())
        running = false;

    running = !running;

    if(running){
        filepath = std::string(":/assets/icons/stop.svg");
        std::transform(filepath.begin(), filepath.end(), filepath.begin(), ::tolower);
        icon.addFile(QString::fromUtf8(filepath), QSize(), QIcon::Normal, QIcon::Off);
        ui->executeMacro->setIcon(icon);
        executeTray->setIcon(QIcon(":/assets/icons/stop.svg"));
        executeTray->setText("Stop Macro");
        selectedMacro->GetSequence()->StartSequence();
    }
    else{
        filepath = std::string(":/assets/icons/play.svg");
        std::transform(filepath.begin(), filepath.end(), filepath.begin(), ::tolower);
        icon.addFile(QString::fromUtf8(filepath), QSize(), QIcon::Normal, QIcon::Off);
        ui->executeMacro->setIcon(icon);
        executeTray->setIcon(QIcon(":/assets/icons/play.svg"));
        executeTray->setText("Play Macro");
    }
}

void MainWindow::on_import_macro_clicked()
{
    WCHAR* path;
    auto get_folder_path_ret = SHGetKnownFolderPath(FOLDERID_Downloads, 0, nullptr, &path);

    if (get_folder_path_ret != S_OK) {
        CoTaskMemFree(path);
    }

    QString filename = QFileDialog::getOpenFileName(this, "Import Macro", QString::fromStdWString(path), "JSON Files (*.json)");
    QFile f(filename);
    if(!f.open(QIODevice::ReadOnly)){
        qDebug("failed to open save file");
    }
    QTextStream file_text(&f);
    QString json_string;
    json_string = file_text.readAll();
    QByteArray json_bytes = json_string.toLocal8Bit();
    f.close();
    CoTaskMemFree(path);

    auto json_doc=QJsonDocument::fromJson(json_bytes);
    if(json_doc.isNull()){
        qDebug()<<"Failed to create JSON doc.";
        return;
    }
    if(!json_doc.isObject()){
        qDebug()<<"JSON is not an object.";
        return;
    }
    QJsonObject json_obj=json_doc.object();
    if(json_obj.isEmpty()){
        qDebug()<<"JSON object is empty.";
        return;
    }
    QVariantMap json_map = json_obj.toVariantMap();

    Util::clearSpacers(ui->verticalLayout_11);
    if(selectedMacro != nullptr){
        selectedMacro->setStyleSheet(QString::fromUtf8("QFrame{\n"
        "	border-radius:0;\n"
        "}\n"
        "QFrame:hover{\n"
        "	background-color: rgb(44, 44, 44);\n"
        "}"));
        selectedMacro->GetOptionsBtn()->setVisible(false);

        SaveMacro(selectedMacro, macropath);

    }
    selectedMacro = AddMacroFrame(json_map["name"].toString());
    selectedMacro->setStyleSheet(QString::fromUtf8("QFrame{\n"
     "	border-radius:0; background-color: rgb(44, 44, 44);\n"
     "}"));

    selectedMacro->GetOptionsBtn()->setVisible(true);

    Sequence* s = Sequence::Deserialize(json_obj);
    selectedMacro->SetSequence(s);

    QSpacerItem* mlist_spacer;
    mlist_spacer = new QSpacerItem(20, 310, QSizePolicy::Minimum, QSizePolicy::Expanding);
    ui->verticalLayout_11->addItem(mlist_spacer);

    ui->selected_error->setVisible(false);
    Util::clearWidgets(ui->editor_list->layout());
    selectedTask = nullptr;
    ui->prop_body->setVisible(false);

    foreach (auto task, selectedMacro->GetSequence()->tasks) {
        AddTask(task, ui->editor_list);
    }

    QSpacerItem *verticalSpacer;
    verticalSpacer = new QSpacerItem(20, 438, QSizePolicy::Minimum, QSizePolicy::Expanding);

    ui->editor_list->layout()->addItem(verticalSpacer);
}

void MainWindow::on_user_avatar_clicked()
{
    QString filepath = QFileDialog::getOpenFileName(
                   nullptr,
                   QObject::tr("Open"),
                   QDir::currentPath(),
                   QObject::tr("Image files (*.png *.svg *jpg);"));
    if(filepath.isEmpty())
        return;

    client->UploadAvatar(filepath, filepath.mid(filepath.lastIndexOf("/")+1));

    QFile *file = new QFile(filepath);
    file->open(QIODevice::ReadOnly);

    QSize s = ui->user_avatar->size();
    ui->user_avatar->setIcon(Util::RoundImage(file->readAll(), s, filepath.mid(filepath.lastIndexOf(".")+1)));
}

void MainWindow::on_upload_macro_clicked()
{
    ui->stackedWidget->setCurrentIndex(8);
    ui->upload_error_name->setVisible(false);
    ui->upload_name->clear();
    ui->upload_desc->clear();

    QObject::connect(ui->upload_choose, &QPushButton::clicked, [=]() {
            on_gotomacros_clicked();
            isChoosing = true;
       });

}

void MainWindow::on_upload_submit_clicked()
{
    if(chosenMacro == nullptr){
        ui->upload_error_name->setText("no macro was chosen");
        ui->upload_error_name->setVisible(true);
        return;
    }
    QRegularExpression rx("^[a-zA-Z0-9_]+( [a-zA-Z0-9_]+)*$");
    if(Util::ValidateInput(ui->upload_name,ui->upload_error_name, &rx)){
        QString res = client->UploadMacro(ui->upload_name->text(), ui->upload_desc->toPlainText(), chosenMacro);
        if(res == QString("")){
            qDebug("success");
            ui->market_user_profile->animateClick();
        }
        else{
            ui->upload_error_name->setText(res);
            ui->upload_error_name->setVisible(true);
        }
    }
    chosenMacro = nullptr;
}

void MainWindow::on_macroPageBack_clicked()
{
    if(!isSearching){
        if(macroPage == 0)
            return;
        macroPage -=1;

        SetUpMarket();
    }
    else{
        if(searchPage == 0)
            return;
        searchPage -=1;

        SearchMacro();
    }

}

void MainWindow::on_macroPageNext_clicked()
{
    if(ui->market_macros->layout()->count() != 11)
        return;

    if(!isSearching){
        macroPage+=1;
        SetUpMarket();
    }
    else{
        searchPage+=1;
        SearchMacro();
    }
}

void MainWindow::on_close_prop_clicked()
{
    ui->macro_explore_prop->setVisible(false);
}

void MainWindow::on_recordMacro_clicked(bool keybind)
{
    if(selectedMacro == nullptr){
        return;
    }
    CheckChoosing();
    if(ui->stackedWidget->currentIndex() != 1)
        return;
    QIcon icon;
    std::string filepath;
    recording = !recording;
    if(recording){
        filepath = std::string(":/assets/icons/stoprec.svg");
        std::transform(filepath.begin(), filepath.end(), filepath.begin(), ::tolower);
        icon.addFile(QString::fromUtf8(filepath), QSize(), QIcon::Normal, QIcon::Off);
        ui->recordMacro->setIcon(icon);
        kHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, NULL);
        mHook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, NULL, NULL);

        recordMS = high_resolution_clock::now();
    }
    else{
        filepath = std::string(":/assets/icons/record.svg");
        std::transform(filepath.begin(), filepath.end(), filepath.begin(), ::tolower);
        icon.addFile(QString::fromUtf8(filepath), QSize(), QIcon::Normal, QIcon::Off);
        ui->recordMacro->setIcon(icon);
        UnhookWindowsHookEx(kHook);
        UnhookWindowsHookEx(mHook);

        int count = ui->editor_list->layout()->count();
        qDebug()<<count;
        if(selectedTask != nullptr){
            selectedTask->setStyleSheet(QString::fromUtf8("QFrame{\n"
            "	border-radius:0;\n"
            "}\n"
            "QFrame:hover{\n"
            "	background-color: rgb(44, 44, 44);\n"
            "}"));
            QPushButton* btn = selectedTask->findChild<QPushButton*>(QString("deleteBtn"));
            btn->setVisible(false);
        }
        if(keybind){
            selectedTask = dynamic_cast<TaskFrame*>(ui->editor_list->layout()->itemAt(count-2)->widget());
            DeleteTaskClicked();
            selectedTask = dynamic_cast<TaskFrame*>(ui->editor_list->layout()->itemAt(count-3)->widget());
            DeleteTaskClicked();
        }
        else{
            selectedTask = dynamic_cast<TaskFrame*>(ui->editor_list->layout()->itemAt(count-2)->widget());
            DeleteTaskClicked();
        }
    }
}

void MainWindow::on_gotomarket_clicked()
{
    on_market_btn_pressed();
}

void MainWindow::on_gotomacros_clicked()
{
    Util::NavbarBtnPressed(ui, 1);
    ui->windowBar->setVisible(false);
}

void MainWindow::on_switch_register_clicked()
{
    ui->stackedWidget->setCurrentIndex(5);
    Util::ResetErrors(ui);
}

void MainWindow::on_switch_login_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
    Util::ResetErrors(ui);
}

void MainWindow::on_reg_submit_clicked()
{
    if(Util::InputCheck(ui)){
        ui->reg_error_server->setVisible(false);
        qDebug("signup");
        QString res = client->Register(ui->reg_email->text(),ui->reg_username->text(),ui->reg_password->text());
        if(res.startsWith("<")){
            qDebug("success");

            QSize s = ui->user_avatar->size();
            ui->user_avatar->setIcon(Util::RoundImage(res.toLocal8Bit(), s, QString("SVG")));

            on_gotomarket_clicked();
            ui->verify_email_frame->setVisible(true);


        }
        else{
            ui->reg_error_server->setText(res);
            ui->reg_error_server->setVisible(true);
        }
    }
}

void MainWindow::on_log_submit_clicked()
{
    if(!overlay->isEnabled() && Util::InputCheckLogin(ui)){
        ui->log_error_server->setVisible(false);
        client->avatar = ui->user_avatar;
        QString res = client->Login(ui->log_email->text(),ui->log_password->text());
        if(res == QString("")){
            qDebug("success");
            on_gotomarket_clicked();
            if(client->isVerified)
                ui->verify_email_frame->setVisible(false);
        }
        else{
            ui->log_error_server->setText(res);
            ui->log_error_server->setVisible(true);
        }
    }
}

void MainWindow::on_user_btn_clicked()
{
    CheckChoosing();
    Util::NavbarBtnPressed(ui, 3);
    ui->windowBar->setVisible(false);
    if(!client->logged){
        ui->stackedWidget->setCurrentIndex(4);
        Util::ResetErrors(ui);
    }
    else{
        ui->stackedWidget->setCurrentIndex(3);
        User* user = client->user;
        ui->user_name->setText(user->name);
        ui->user_email->setText(user->email);
        ui->user_id->setText(user->id);
        ui->user_about->setText(user->about);
    }
}

void MainWindow::on_log_forgot_clicked()
{
    overlay->show(true);
}

void MainWindow::on_user_verify_email_clicked()
{
    overlay->deleteLater();
    overlay = new OverlayWidget(250, OverlayWidget::EmailValid, client, ui->user_page->size(),nullptr, ui->user_page);
    overlay->show(true);
    overlay->validemailframe = ui->verify_email_frame;
}

void MainWindow::on_user_logoutbtn_clicked()
{
    client->Logout(client->tokens->REFRESH);
    ui->stackedWidget->setCurrentIndex(4);
    ui->verify_email_frame->setVisible(true);
}

void MainWindow::on_download_macro_explore_clicked()
{
    on_library_btn_pressed();
    client->DownloadMacro(marketMacro->macro->id);
    NewMacroFrame(marketMacro->macro->name, marketMacro->seq);
    SaveMacro(selectedMacro,macropath);
}

void MainWindow::on_submit_comment_clicked()
{
    QString t = ui->macro_comment->toPlainText();

    if(client->UploadComment(marketMacro->macro->id, t)){
        Comment c("none", client->user->id, t);
        AddCommentFrame(c);
    }
}

void MainWindow::on_cmd_clicked()
{
    MacroActionClick(new CmdTask("","command", 1,1));
}

void MainWindow::on_macro_save_clicked()
{
    ExportMacro();
}

