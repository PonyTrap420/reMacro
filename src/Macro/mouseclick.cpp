#include "mouseclick.h"


//#define WM_MOUSEFIRST 0x0200
//#define WM_MOUSEMOVE 0x0200
//#define WM_LBUTTONDOWN 0x0201
//#define WM_LBUTTONUP 0x0202
//#define WM_LBUTTONDBLCLK 0x0203
//#define WM_RBUTTONDOWN 0x0204
//#define WM_RBUTTONUP 0x0205
//#define WM_RBUTTONDBLCLK 0x0206
//#define WM_MBUTTONDOWN 0x0207
//#define WM_MBUTTONUP 0x0208
//#define WM_MBUTTONDBLCLK 0x0209
//#define WM_MOUSEWHEEL 0x020A
//#define WM_XBUTTONDOWN 0x020B
//#define WM_XBUTTONUP 0x020C
//#define WM_XBUTTONDBLCLK


MouseClick::MouseClick() : Task(){
    POINT p = WaitForMouseClick(); //NOT WORKING FOR ALL MOUSE BUTTONS
    this->x = p.x;
    this->y = p.y;
    this->keystroke = MOUSEEVENTF_LEFTDOWN;
}


void MouseClick::Execute(){
    for(int i = 0; i < repeats; i++){
        Sleep(delay*1000);
        MoveMouse(x,y);
        Sleep(20);
        ClickMouse();
    }
}

void MouseClick::SetMouse(int x, int y){
    this->x = x;
    this->y = y;
}

void MouseClick::SetKeystroke(int keystroke){
    this->keystroke = keystroke;
}

void MouseClick::MoveMouse(int x, int y){
    cout << "Moving mouse to "<< x << "," << y<< endl;
    SetCursorPos(x, y);
    Sleep(20);
}
void MouseClick::ClickMouse(){
    INPUT Inputs[2] = {};

    Inputs[0].type = INPUT_MOUSE;

    Inputs[0].mi.dwFlags = keystroke;

    Inputs[1].type = INPUT_MOUSE;
    Inputs[1].mi.dwFlags = keystroke << 1;

//    switch(Inputs[0].mi.dwFlags){
//        case MOUSEEVENTF_LEFTDOWN:
//             Inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
//        break;

//        case MOUSEEVENTF_MIDDLEDOWN:
//            Inputs[1].mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
//        break;

//        case MOUSEEVENTF_RIGHTDOWN:
//            Inputs[1].mi.dwFlags = MOUSEEVENTF_RIGHTUP;
//        break;

//        default:
//        break;
//    }
    SendInput(2, Inputs, sizeof(INPUT));
}
POINT MouseClick::WaitForMouseClick(){
    bool Clicked = false;
    while(!Clicked){
        Sleep(10);
        if (GetAsyncKeyState(VK_LBUTTON)<0)
        {
            cout << "left button pressed" << endl;
            bool pressed = TRUE;

            while (pressed) {
                if (GetAsyncKeyState(VK_LBUTTON) == 0)
                {
                    POINT p;
                    if (GetCursorPos(&p))
                    {
                        return p;
                    }
                }
            }
        }
    }
    POINT p;
    return p;
}

QJsonObject MouseClick::Serialize(){
    QJsonObject task_obj;
    task_obj["name"] = name;
    task_obj["delay"] = delay;
    task_obj["repeats"] = repeats;
    task_obj["type"] = type;
    task_obj["x"] = x;
    task_obj["y"] = y;
    task_obj["keystroke"] = keystroke;

    return task_obj;
}

Task* MouseClick::Deserialize(QVariantMap task_map){
    return new MouseClick(task_map["x"].toInt(), task_map["y"].toInt(), task_map["keystroke"].toInt(), task_map["name"].toString(), task_map["delay"].toDouble(), task_map["repeats"].toInt());
}

