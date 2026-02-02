#define UNICODE
#define _UNICODE
#include <windows.h>
#include <tchar.h>
#include "SoftwareDefinitions.h"



int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow) {

    WNDCLASS SoftwareMainClass = NewWindowClass(
        (HBRUSH)COLOR_WINDOW, 
        LoadCursor(NULL, IDC_ARROW), 
        hInst, 
        LoadIcon(NULL, IDI_QUESTION), 
        _T("MainWndClass"), 
        SoftwareMainProcedure
    );

    if(!RegisterClass(&SoftwareMainClass)) {return -1;}
    MSG SoftwareMainMessage = {0};

    CreateWindow(
        _T("MainWndClass"), 
        _T("Messenger"), 
        WS_OVERLAPPEDWINDOW | WS_VISIBLE, 
        100, 100, 500, 250, 
        NULL, NULL, NULL, NULL
    );

    while (GetMessage(&SoftwareMainMessage, NULL, 0, 0))
    {
        TranslateMessage(&SoftwareMainMessage);
        DispatchMessage(&SoftwareMainMessage);
    }
    
    return 0;
}

WNDCLASS NewWindowClass(HBRUSH BGColor, HCURSOR Cursor, HINSTANCE hInst, HICON Icon, LPCTSTR Name, WNDPROC Procedure) {
    WNDCLASS NWC = {0};

    NWC.hIcon = Icon;
    NWC.hCursor = Cursor;
    NWC.hInstance = hInst;
    NWC.lpszClassName = Name;
    NWC.hbrBackground = BGColor;
    NWC.lpfnWndProc = Procedure;
    NWC.style = CS_HREDRAW | CS_VREDRAW;
    NWC.lpszMenuName = NULL;
    NWC.cbClsExtra = 0;
    NWC.cbWndExtra = 0;

    return NWC;
}

LRESULT CALLBACK SoftwareMainProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg)
    {
    case WM_COMMAND:
        switch (wp)
        {
        case OnMenuAction1:
            MessageBoxA(hWnd, "Your username is ...", "Information", MB_OK);
            break;
        case OnMenuAction2:
            MessageBoxA(hWnd, "Create group...", "Information", MB_OK);
            break;
        case OnMenuAction3:
            MessageBoxA(hWnd, "Create channel...", "Information", MB_OK);
            break;
        default: break;
        }
        break;
    case WM_CREATE:
        MainWndAddMenus(hWnd);
        MainWndAddWidgets(hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default: return DefWindowProc(hWnd, msg, wp, lp);
    }

    return 0;
}

void MainWndAddMenus(HWND hWnd) {
    HMENU RootMenu = CreateMenu();
    HMENU SubMenu = CreateMenu();

    AppendMenu(SubMenu, MF_STRING, OnMenuAction1, L"My profile");
    AppendMenu(SubMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(SubMenu, MF_STRING, OnMenuAction2, L"New group");
    AppendMenu(SubMenu, MF_STRING, OnMenuAction3, L"New channel");

    AppendMenu(RootMenu, MF_POPUP, (UINT_PTR)SubMenu, L"Profile");

    SetMenu(hWnd, RootMenu);
}

void MainWndAddWidgets(HWND hWnd) {

    CreateWindow(L"static", L"Send message", WS_VISIBLE | WS_CHILD | ES_CENTER, 5, 5, 490, 20, hWnd, NULL, NULL, NULL);

    CreateWindowA("edit", "Write a message...", WS_VISIBLE | WS_CHILD, 5, 30, 490, 20, hWnd, NULL, NULL, NULL);

    CreateWindowA("button", "Send message", WS_VISIBLE | WS_CHILD | ES_CENTER, 5, 60, 120, 30, hWnd, NULL, NULL, NULL);
}