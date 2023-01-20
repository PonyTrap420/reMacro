#include "macromenuoptions.h"
#include "ui_macromenuoptions.h"
#include "QLayout"

MacroMenuOptions::MacroMenuOptions(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MacroMenuOptions)
{
    ui->setupUi(this);
    setWindowFlag(Qt::Popup);
    layout()->setContentsMargins(0,0,0,0);
    setWindowFlags(windowFlags() | Qt::NoDropShadowWindowHint);
    ui->deleteBtn->setAutoFillBackground(true);
    ui->exportBtn->setAutoFillBackground(true);
}

MacroMenuOptions::~MacroMenuOptions()
{
    delete ui;
}

void MacroMenuOptions::on_deleteBtn_clicked()
{
    if(macroframe != nullptr){
        MacroFrame::deletedMacro = macroframe->id;
        macroframe->deleteLater();
        macroframe = nullptr;
        this->setVisible(false);
    }
}

void MacroMenuOptions::on_exportBtn_clicked(){
    emit ExportMacro();
}

void MacroMenuOptions::on_changename_clicked()
{
    this->close();
    macroframe->name_label->setVisible(false);
    macroframe->name_change->setVisible(true);
    macroframe->name_change->setText(macroframe->name);
    macroframe->name_change->setFocus();
}

void MacroMenuOptions::on_keybinds_clicked()
{
    this->close();
    emit OpenKeybinds();
}

