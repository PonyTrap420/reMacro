#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QToolBar>
#include <QFrame>
#include <QLineEdit>
#include "Macro/sequence.h"
#include "Macro/macro_frame.h"
#include "Macro/taskframe.h"
#include "QMenu"
#include "macromenuoptions.h"
#include "QListWidgetItem"
#include "HTTPclient.h"
#include "Macro/marketmacroframe.h"
#include "Macro/looptaskframe.h"
#include "QSystemTrayIcon"

#include <chrono>
using namespace std::chrono;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);

    //If you want to have Max/Min/Close buttons, look at how QWinWidget uses these
    QPushButton* maximizeButton = nullptr;
    QPushButton* minimizeButton = nullptr;
    QPushButton* closeButton = nullptr;

    //If you want to enable dragging the window when the mouse is over top of, say, a QToolBar,
    //then look at how QWinWidget uses this
    QToolBar* toolBar = nullptr;
    QSystemTrayIcon* sysTrayIcon = nullptr;

    int currentPage = 0;

    std::wstring macropath;

    OverlayWidget* overlay = nullptr;

    MacroFrame* selectedMacro = nullptr;
    TaskFrame* selectedTask = nullptr;
    QJsonArray macrosJson;

    bool recording = false;
    high_resolution_clock::time_point recordMS;

    QAction* executeTray = nullptr;
    bool running = false;

    MacroFrame* chosenMacro = nullptr;
    bool isChoosing = false;

    HttpClient* client = nullptr;

    MarketMacroFrame* marketMacro = nullptr;
    bool isSearching = false;
    int macroPage = 0;
    int searchPage = 0;
    int commentPage = 0;


    //HOOKED FUNCTIONS
    static LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam);
    HHOOK kHook;
    HHOOK mHook;

    void MacroActionClick(Task* task);

    MacroFrame* AddMacroFrame(QString name);
    TaskFrame* AddTaskFrame(Task* task, QWidget* parent, bool readOnly = false);
    LoopTaskFrame* AddLoopTaskFrame(Task* task, QWidget* parent, bool readOnly = false);
    bool SaveMacro(MacroFrame* m, std::wstring path);
    bool SaveAllMacros();
    int GetMacroPos();
    void SetUpMarket();
    void AddMarketMacros(QJsonDocument macros, QLayout* l);
    void NewMacroFrame(QString name, Sequence* seq);
    void AddCommentFrame(Comment comment);
    TaskFrame* AddTask(Task* task, QWidget* parent, bool readOnly = false);
    void RegisterSequenceKey(Sequence* seq);

    int registeredKey = 0;
    void PressedHotkey(WORD key);

signals:
    void RegisterKey(int mod, int key);
    void UnRegisterKey();

public slots:
    void UpdateTaskProp();
    void Sequence_more_pressed();
    void MacroFrameDestroyed();
    void ChangeMacroName();
    void DeleteTaskClicked();
    void ExploreMacro();
    void ClickedUserAvatar();
    void OpenKeybinds();
    void ExportMacro();
    void ChangedKeybind();
    void SwitchedPage(int page);
    void SearchMacro();

private slots:
    bool eventFilter(QObject *obj, QEvent *event);

    void on_home_btn_pressed();

    void on_library_btn_pressed();

    void on_market_btn_pressed();

    void on_add_macro_pressed();

    void on_mouseclick_clicked();

    void on_keyboardpress_clicked();

    void on_text_clicked();

    void on_loop_clicked();

    void on_executeMacro_clicked();

    void on_import_macro_clicked();

    void on_recordMacro_clicked(bool keybind = false);

    void on_gotomarket_clicked();

    void on_gotomacros_clicked();

    void on_switch_register_clicked();

    void on_switch_login_clicked();

    void on_reg_submit_clicked();

    void on_log_submit_clicked();

    void on_user_btn_clicked();

    void on_log_forgot_clicked();

    void on_user_verify_email_clicked();

    void on_user_logoutbtn_clicked();

    void on_user_avatar_clicked();

    void on_upload_macro_clicked();

    void on_upload_submit_clicked();

    void on_macroPageBack_clicked();

    void on_macroPageNext_clicked();

    void on_close_prop_clicked();

    void on_download_macro_explore_clicked();

    void on_submit_comment_clicked();

    void on_cmd_clicked();

    void on_macro_save_clicked();

private:
    Ui::MainWindow *ui;

    QMenu* macromenu = nullptr;
    MacroMenuOptions* MacroMenuWidget = nullptr;

    void SetupAppdata();
    void SetupGUI();

    void CheckChoosing();
};

#endif // MAINWINDOW_H
