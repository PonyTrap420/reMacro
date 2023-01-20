#include "keyboardtype.h"


void KeyboardType::Execute(){
   std::string text = this->text.toStdString();
   for(int i = 0; i < repeats; i++){
       Sleep(delay*1000);
       for (std::string::size_type i = 0; i < text.length(); i++){
            PressKey(int_to_hex((VkKeyScanEx(text[i], myKL))));
       }
   }
}

void KeyboardType::PressKey(std::string key){
    // input event.
    INPUT ip;
    // Set up a generic keyboard event.
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0; // hardware scan code for key
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;

    // Press the "A" key
    ip.ki.wVk = (unsigned short)std::strtoul(key.c_str(), NULL, 0); // virtual-key code for the "a" key
    ip.ki.dwFlags = 0; // 0 for key press
    SendInput(1, &ip, sizeof(INPUT));

    ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
    SendInput(1, &ip, sizeof(INPUT));
}

std::string KeyboardType::int_to_hex(int i)
{
  std::stringstream stream;
  stream << "0x"
         << std::setfill ('0') << std::setw(sizeof(int)*2)
         << std::hex << i;
  return stream.str();
}

QJsonObject KeyboardType::Serialize(){
    QJsonObject task_obj;
    task_obj["name"] = name;
    task_obj["delay"] = delay;
    task_obj["repeats"] = repeats;
    task_obj["type"] = type;
    task_obj["text"] = text;

    return task_obj;
}

Task* KeyboardType::Deserialize(QVariantMap task_map){
    return new KeyboardType(task_map["text"].toString(), task_map["name"].toString(), task_map["delay"].toDouble(), task_map["repeats"].toInt());
}
