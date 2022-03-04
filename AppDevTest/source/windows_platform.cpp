#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <stdio.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

struct StateInfo
{
    int red;
    int green;
    int blue;
};

inline StateInfo* GetAppState(HWND hwnd)
{
    LONG_PTR ptr = GetWindowLongPtr(hwnd, GWLP_USERDATA);
    StateInfo *pState = (StateInfo*) ptr;
    return pState;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    // Registering the window class
    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASS wc = { };

    wc.lpfnWndProc      = WindowProc;
    wc.hInstance        = hInstance;
    wc.lpszClassName    = CLASS_NAME;

    RegisterClass(&wc);

    StateInfo *pState = (StateInfo*) VirtualAlloc(NULL, sizeof(StateInfo*), MEM_COMMIT, PAGE_READWRITE);

    if (pState == NULL)
    {
        return 0;
    }

    pState->red = 0;
    pState->green = 0;
    pState->blue = 0;

    HWND hwnd = CreateWindowEx(
            0,                              // Optional window styles
            CLASS_NAME,                     // Window class
            L"Learn to Program Windows",    // Window text
            WS_OVERLAPPEDWINDOW,

            // Size and position
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

            NULL,       // Parent window
            NULL,       // Menu
            hInstance,  // Instance handle
            pState      // Additional application data
            );

    if (hwnd == NULL)
    {
        return 0;
    }
    
    OutputDebugStringA("About to start mainloop");

    ShowWindow(hwnd, nCmdShow);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    StateInfo *pState;
    if (uMsg == WM_CREATE)
    {
        CREATESTRUCT *pCreate = (CREATESTRUCT*) lParam;
        pState = (StateInfo*) pCreate->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pState);
    }
    else
    {
        pState = GetAppState(hwnd);
    }

    switch(uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
        
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            FillRect(hdc, &ps.rcPaint, (HBRUSH) CreateSolidBrush(RGB(pState->red,pState->green,pState->blue)));

            EndPaint(hwnd, &ps);
        }
        return 0;

    case WM_KEYDOWN:
        if (wParam == VK_UP) 
        {
            pState->green += 10;
        }
        else if (wParam == VK_DOWN)
        {
            pState->green -= 10;
        }
        else if (wParam == VK_LEFT)
        {
            pState->red += 10;
        }
        else if (wParam == VK_RIGHT)
        {
            pState->red -= 10;
        }
        RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT);
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
