#include "uiUtils.h"
#include <iostream>
#include "Macro/keyboardpress.h"
#include "Macro/mouseclick.h"
#include "Macro/keyboardtype.h"
#include "Macro/cmdtask.h"
#include "Macro/looptask.h"
#include "windows.h"
#include "Filters/keyboard_field.h"
#include "qregularexpression.h"


void Util::ToggleBtn(QPushButton* btn){
    btn->setStyleSheet(QString::fromUtf8("background-color:#323232;"));
}

void Util::UpdateButtonIcon(QPushButton* btn, bool pressed, int index){
QIcon icon;
QString iconPath = ":/assets/final/";
switch (index){
    case 0:
        iconPath += "home";
    break;
    case 1:
        iconPath += "library";
    break;
    case 2:
        iconPath += "market";
    break;
    case 3:
        iconPath += "user";
    break;
}
if(pressed)
    iconPath += "-pressed.svg";
else
    iconPath += ".svg";

icon.addFile(iconPath, QSize(), QIcon::Normal, QIcon::Off);
btn->setIcon(icon);
}

void Util::ResetButton(QLabel* label, QPushButton* btn, int index){
    btn->setStyleSheet(QString::fromUtf8("QPushButton{\n"
     "		padding-bottom:15px;\n"
     "    }\n"
     "    QPushButton:hover{\n"
     "        background-color: #252525;\n"
     "       border: 0px;\n"
     "    }\n"
     "    QPushButton:pressed{\n"
     "    background-color: #292929;\n"
     "        border: 0px;\n"
     "    }"));
    UpdateButtonIcon(btn, false, index);
    label->setVisible(true);

}

void Util::NavbarBtnPressed(Ui::MainWindow* ui, int index){
    QPushButton* btns[4] = {ui->home_btn, ui->library_btn, ui->market_btn, ui->user_btn};
    QLabel* labels[4] = {ui->home_label, ui->library_label, ui->market_label, ui->user_label};
    for(int i = 0; i < 4; i++)
        ResetButton(labels[i], btns[i], i);
    ui->stackedWidget->setCurrentIndex(index);
    ToggleBtn(btns[index]);
    UpdateButtonIcon(btns[index], true, index);
    labels[index]->setVisible(false);
}

void Util::clearWidgets(QLayout * layout) {
    if (! layout)
      return;
    while (auto item = layout->takeAt(0)) {
      delete item->widget();
      Util::clearWidgets(item->layout());
    }
}

void Util::clearSpacers(QLayout * layout){
    for (int i = 0; i < layout->count(); ++i) {
        QLayoutItem *layoutItem = layout->itemAt(i);
        if (layoutItem->spacerItem()) {
            layout->removeItem(layoutItem);
            // You could also use: layout->takeAt(i);
            delete layoutItem;
            --i;
        }
    }
}

uint Util::ScanToVirtual(int scan){
    HKL layout = GetKeyboardLayout(0);
    return MapVirtualKeyEx(scan,1,layout);
}

char Util::ScanToChar(int scan){
    HKL layout = GetKeyboardLayout(0);
    uint vk;

    vk=MapVirtualKeyEx(scan,1,layout);
    return MapVirtualKeyExW(vk, MAPVK_VK_TO_CHAR, layout);
}

void Util::HandleTaskProp(Ui::MainWindow* ui, Task* task, bool readOnly){
    map<int, QString> codeNames;
    int scancode;


    QFrame* temp1;
    QFrame* temp2;
    QFrame* temp3;
    QFrame* temp4;
    QLineEdit* temp5;
    QLineEdit* temp6;
    QLineEdit* temp7;
    QLineEdit* temp8;

    if(readOnly)
    {
        temp1 = ui->prop_field_text;
        temp2 = ui->prop_field_keystroke;
        temp3 = ui->prop_field_x;
        temp4 = ui->prop_field_y;
        temp5 = ui->field_text;
        temp6 = ui->field_keystroke;
        temp7 = ui->field_x;
        temp8 = ui->field_y;

        ui->prop_field_text = ui->prop_field_text_read;
        ui->prop_field_keystroke = ui->prop_field_keystroke_read;
        ui->prop_field_x = ui->prop_field_x_read;
        ui->prop_field_y = ui->prop_field_y_read;

        ui->field_text = ui->field_text_read;
        ui->field_keystroke = ui->field_keystroke_read;
        ui->field_x = ui->field_x_read;
        ui->field_y = ui->field_y_read;

    }
    switch(task->type){
        case(Mouse):
            codeNames[MOUSEEVENTF_MOVE] = "Move";
            codeNames[MOUSEEVENTF_LEFTDOWN] = "Left Button";
            codeNames[MOUSEEVENTF_RIGHTDOWN] = "Right Button";
            codeNames[MOUSEEVENTF_MIDDLEDOWN] = "Middle Button";
            codeNames[MOUSEEVENTF_XDOWN] = "X Button";


            ui->prop_field_text->setVisible(false);
            ui->prop_field_x->setVisible(true);
            ui->prop_field_y->setVisible(true);
            ui->prop_field_keystroke->setVisible(true);
            ui->field_keystroke->setPlaceholderText(codeNames[dynamic_cast<MouseClick*>(task)->keystroke]);
            ui->field_x->setPlaceholderText(QString::number(dynamic_cast<MouseClick*>(task)->x));
            ui->field_y->setPlaceholderText(QString::number(dynamic_cast<MouseClick*>(task)->y));

            ui->field_x->clear();
            ui->field_y->clear();
            ui->field_keystroke->clear();
        break;
        case(Keyboard):
            ui->prop_field_text->setVisible(false);
            ui->prop_field_x->setVisible(false);
            ui->prop_field_y->setVisible(false);
            ui->prop_field_keystroke->setVisible(true);

            scancode = dynamic_cast<KeyboardPress*>(task)->keystroke;


            ui->field_keystroke->setPlaceholderText(QString(ScanToChar(scancode)));
            ui->field_keystroke->clear();

            if(!readOnly)
                dynamic_cast<KeyboardField*>(ui->field_keystroke)->SetTask(task);
        break;
        case(Text):
            ui->prop_field_x->setVisible(false);
            ui->prop_field_y->setVisible(false);
            ui->prop_field_keystroke->setVisible(false);
            ui->prop_field_text->setVisible(true);

            ui->field_text->setPlaceholderText(dynamic_cast<KeyboardType*>(task)->text);
            ui->field_text->clear();
        break;
        case(Cmd):
            ui->prop_field_x->setVisible(false);
            ui->prop_field_y->setVisible(false);
            ui->prop_field_keystroke->setVisible(false);
            ui->prop_field_text->setVisible(true);

            ui->field_text->setPlaceholderText(dynamic_cast<CmdTask*>(task)->cmd);
            ui->field_text->clear();
        break;
        case(Loop):
            ui->prop_field_x->setVisible(false);
            ui->prop_field_y->setVisible(false);
            ui->prop_field_keystroke->setVisible(false);
            ui->prop_field_text->setVisible(false);
        break;
    }
    ui->field_delay->setPlaceholderText(QString::number(task->delay));
    ui->field_repeats->setPlaceholderText(QString::number(task->repeats));
    ui->field_macro_name->setPlaceholderText(task->name);
    ui->field_delay->clear();
    ui->field_repeats->clear();
    ui->field_macro_name->clear();

    if(readOnly)
    {
        ui->field_delay_read->setPlaceholderText(QString::number(task->delay));
        ui->field_repeats_read->setPlaceholderText(QString::number(task->repeats));
        ui->field_macro_name_read->setPlaceholderText(task->name);
        ui->field_delay_read->clear();
        ui->field_repeats_read->clear();
        ui->field_macro_name_read->clear();

        ui->prop_field_text = temp1;
        ui->prop_field_keystroke = temp2;
        ui->prop_field_x = temp3;
        ui->prop_field_y = temp4;
        ui->field_text = temp5;
        ui->field_keystroke = temp6;
        ui->field_x = temp7;
        ui->field_y = temp8;

    }
}

bool Util::ValidateInput(QLineEdit* le, QLabel* err, QRegularExpression* rx){
    if(rx->match(le->text()).hasMatch()){
        err->setVisible(false);
        return true;
    }
    err->setVisible(true);
    return false;
}

bool EmailCheck(Ui::MainWindow* ui){
    QRegularExpression rx("^[A-Z0-9a-z._-]{1,}@(\\w+)(\\.(\\w+))(\\.(\\w+))?(\\.(\\w+))?$");
    return Util::ValidateInput(ui->reg_email,ui->reg_error_email, &rx);
}

bool UsernameCheck(Ui::MainWindow* ui){
    QRegularExpression rx("^[a-zA-Z0-9]{2,}$");
    return Util::ValidateInput(ui->reg_username,ui->reg_error_username, &rx);

}

bool PasswordCheck(Ui::MainWindow* ui){
    QRegularExpression rx("^(?=.*?[A-Z])(?=.*?[a-z])(?=.*?[0-9])(?=.*?[#?!@$%^&*-]).{8,}$");
    return Util::ValidateInput(ui->reg_password,ui->reg_error_password, &rx);
}

bool Util::InputCheck(Ui::MainWindow* ui){
    if(EmailCheck(ui) & UsernameCheck(ui) & PasswordCheck(ui))
        return true;
    return false;
}

bool EmailLoginCheck(Ui::MainWindow* ui){
    QRegularExpression rx("^[A-Z0-9a-z._-]{1,}@(\\w+)(\\.(\\w+))(\\.(\\w+))?(\\.(\\w+))?$");
    return Util::ValidateInput(ui->log_email,ui->log_error_email, &rx);
}

bool PasswordLoginCheck(Ui::MainWindow* ui){
    if(ui->log_password->text().length()<2){
        ui->log_error_password->setVisible(true);
        return false;
    }
    ui->log_error_password->setVisible(false);
    return true;
}

bool Util::InputCheckLogin(Ui::MainWindow* ui){
    if(EmailLoginCheck(ui) & PasswordLoginCheck(ui))
        return true;
    return false;
}

void Util::ResetErrors(Ui::MainWindow* ui){
   ui->reg_error_email->setVisible(false);
   ui->reg_error_password->setVisible(false);
   ui->reg_error_username->setVisible(false);
   ui->reg_error_server->setVisible(false);
   ui->log_error_email->setVisible(false);
   ui->log_error_password->setVisible(false);
   ui->log_error_server->setVisible(false);
}

QIcon Util::RoundImage(QByteArray b, QSize s, QString type){
    QPixmap target = QPixmap(100,100);
    target.fill(Qt::transparent);

    QPixmap p = QPixmap();
    p.loadFromData(b, type.toLocal8Bit());
    p.scaled(s.width(), s.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QPainterPath path;
    path.addEllipse(target.rect());

    QPainter painter = QPainter(&target);
    painter.setRenderHint(QPainter::Antialiasing, true);


    painter.setClipPath(path);
    painter.drawPixmap(0, 0, 100,100, p);

    return QIcon(target);
}
