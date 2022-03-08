#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <stdio.h>
#include <stdint.h>

#include "windows_platform.h"

#define global_variable static
#define internal static

global_variable OffscreenBuffer globalBackBuffer;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

struct StateInfo
{
    int rect_pos_x;
    int rect_pos_y;
    int rect_width;
    int rect_height;
};

inline StateInfo* GetAppState(HWND hwnd)
{
    LONG_PTR ptr = GetWindowLongPtr(hwnd, GWLP_USERDATA);
    StateInfo *pState = (StateInfo*) ptr;
    return pState;
};

internal void resizeDIBSection(OffscreenBuffer *buffer, int width, int height)
{
    if (buffer->memory)
    {
        VirtualFree(buffer->memory, 0, MEM_RELEASE);
    }

    buffer->width = width;
    buffer->height = height;

    int bytesPerPixel = 4;
    buffer->bytesPerPixel = bytesPerPixel;

    buffer->info.bmiHeader.biSize = sizeof(buffer->info.bmiHeader);
    buffer->info.bmiHeader.biWidth = buffer->width;
    buffer->info.bmiHeader.biHeight = -buffer->height;
    buffer->info.bmiHeader.biPlanes = 1;
    buffer->info.bmiHeader.biBitCount = 32;
    buffer->info.bmiHeader.biCompression = BI_RGB;


    int bitmapMemorySize = (buffer->width * buffer->height) * bytesPerPixel;
    buffer->memory = VirtualAlloc(0, bitmapMemorySize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    buffer->pitch = width*bytesPerPixel;
}

internal void displayBufferInWindow(OffscreenBuffer *buffer, HDC deviceContext, int windowWidth, int windowHeight)
{
    StretchDIBits(
            deviceContext,
            0, 0, windowWidth, windowHeight,
            0, 0, buffer->width, buffer->height,
            buffer->memory,
            &buffer->info,
            DIB_RGB_COLORS, SRCCOPY
            );
}

internal Dimensions getWindowDimensions(HWND window)
{
    Dimensions result;
    
    RECT clientRect;
    GetClientRect(window, &clientRect);
    result.width = clientRect.right - clientRect.left;
    result.height = clientRect.bottom - clientRect.top;

    return(result);
}


internal void renderGradient(OffscreenBuffer *buffer, StateInfo *state)
{
    uint8_t *row = (uint8_t *)buffer->memory;
    for (int y = 0; y < buffer->height; y++)
    {
        uint32_t *pixel = (uint32_t *)row;
        for (int x = 0; x < buffer->width; x++)
        {
            if (    x > state->rect_pos_x - state->rect_width/2 && 
                    x < state->rect_pos_x + state->rect_width/2 &&
                    y > state->rect_pos_y - state->rect_height/2 && 
                    y < state->rect_pos_y + state->rect_height/2)
            {
                *pixel++ = (150 << 16) | (150 << 8) | 150;
            }
            else {
                *pixel++ = (200 << 16) | (200 << 8) | 200;
            }
            //uint8_t blue = (uint8_t)x;
            //uint8_t green = (uint8_t)y;

            //*pixel++ = ((green << 8) | blue);
        }

        row += buffer->pitch;
    }
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    // Registering the window class
    WNDCLASS wc = { };
    
    resizeDIBSection(&globalBackBuffer, 1280, 720);

    wc.lpfnWndProc      = WindowProc;
    wc.hInstance        = hInstance;
    wc.lpszClassName    = L"Windows test";

    RegisterClass(&wc);

    StateInfo *pState = (StateInfo*) VirtualAlloc(NULL, sizeof(StateInfo*), MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);

    pState->rect_pos_y = 100;
    pState->rect_pos_x = 100;
    pState->rect_width = 100;
    pState->rect_height = 100;

    renderGradient(&globalBackBuffer, pState);

    if (pState == NULL)
    {
        return 0;
    }

    HWND hwnd = CreateWindowEx(
            0,                              // Optional window styles
            L"Windows test",                     // Window class
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

            Dimensions dimension = getWindowDimensions(hwnd);
            renderGradient(&globalBackBuffer, pState);
            displayBufferInWindow(&globalBackBuffer, hdc, dimension.width, dimension.height);

            EndPaint(hwnd, &ps);
        }
        return 0;

    case WM_KEYDOWN:
        if (wParam == VK_UP) 
        {
            pState->rect_pos_y -= 5;
        }
        else if (wParam == VK_DOWN)
        {
            pState->rect_pos_y += 5;
        }
        else if (wParam == VK_RIGHT)
        {
            pState->rect_pos_x += 5;
        }
        else if (wParam == VK_LEFT)
        {
            pState->rect_pos_x -= 5;
        }
        else if (wParam == 'W')
        {
            pState->rect_height += 2;
        }
        else if (wParam == 'S')
        {
            pState->rect_height -= 2;
        }
        else if (wParam == 'A')
        {
            pState->rect_width -= 2;
        }
        else if (wParam == 'D')
        {
            pState->rect_width += 2;
        }
        RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE|RDW_INTERNALPAINT);
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
