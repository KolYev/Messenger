#define UNICODE
#define _UNICODE
#include <windows.h>
#include <tchar.h>

LRESULT CALLBACK SoftwareMainProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
WNDCLASS NewWindowClass(HBRUSH BGColor, HCURSOR Cursor, HINSTANCE hInst, HICON Icon, LPCTSTR Name, WNDPROC Procedure);

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
        _T("First c++ window"), 
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
    case WM_CREATE:
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default: return DefWindowProc(hWnd, msg, wp, lp);
    }

    return 0;
}