#include "keyboardpress.h"

void KeyboardPress::SetKeystroke(int keystroke){
    this->keystroke = keystroke;
}

void KeyboardPress::Execute(){
    for(int i = 0; i < repeats; i++){
        Sleep(delay*1000);
        PressKey();
    }
}
void KeyboardPress::PressKey(){
   INPUT ip;

   // Set up a keyboard event.
   ip.type = INPUT_KEYBOARD;
   ip.ki.wScan = keystroke; // hardware scan code for key
   ip.ki.time = 0;
   ip.ki.dwExtraInfo = 0;

   ip.ki.dwFlags = KEYEVENTF_SCANCODE;
   SendInput(1, &ip, sizeof(INPUT));

   ip.ki.dwFlags = KEYEVENTF_KEYUP;//release key
   SendInput(1, &ip, sizeof(INPUT));
}

void KeyboardPress::HoldKey(){
    INPUT ip;
    // Set up a generic keyboard event.
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = keystroke; // hardware scan code for key
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;

    // Press the "A" key
    ip.ki.wVk = MapVirtualKeyA(keystroke, 1);; // virtual-key code for the "a" key
    ip.ki.dwFlags = 0; // 0 for key press
    SendInput(1, &ip, sizeof(INPUT));
}

QJsonObject KeyboardPress::Serialize(){
    QJsonObject task_obj;
    task_obj["name"] = name;
    task_obj["delay"] = delay;
    task_obj["repeats"] = repeats;
    task_obj["type"] = type;
    task_obj["keystroke"] = keystroke;

    return task_obj;
}

Task* KeyboardPress::Deserialize(QVariantMap task_map){
    return new KeyboardPress(task_map["keystroke"].toInt(), task_map["name"].toString(), task_map["delay"].toDouble(), task_map["repeats"].toInt());
}
