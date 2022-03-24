#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <stdio.h>
#include <stdint.h>

#include "my_math.h"

#define global_variable static
#define internal static

global_variable OffscreenBuffer globalBackBuffer;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

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


internal void render(OffscreenBuffer *buffer, StateInfo *state, int superSample)
{
    float factor = 0.0f;
    uint8_t *row = (uint8_t *)buffer->memory;
    for (int y = 0; y < buffer->height*superSample; y+=superSample)
    {
        uint32_t *pixel = (uint32_t *)row;
        for (int x = 0; x < buffer->width*superSample; x+=superSample)
        {
            //factor = 0.0f;
            //for (int i = 0; i < superSample; i++)
            //{
                //for (int k = 0; k < superSample; k++)
                //{
                    ////if (sqr(x+i - state->circ_pos_x) + sqr(y+k - state->circ_pos_y) >= sqr(state->circ_r) && 
                         ////sqr(x+i - state->circ_pos_x) + sqr(y+k - state->circ_pos_y) <= sqr(state->circ_r + 10))
                    
                    //float x_mindist = newtonFindZero(x, y, state); 
                    //float minDistSqr = squareDistancePointCurve(x_mindist, state, x, y);
                    //if (minDistSqr <= 16)
                    //{
                        //factor += 1.0f/(superSample*superSample);
                    //}

                //}

            //}
            float x_mindist = newtonFindZero(x, y, state); 
            float minDistSqr = squareDistancePointCurve(x_mindist, state, x, y);
            if (x == 844 && y == 391)
            {
                int x = 1;
            }
            if (minDistSqr <= 400)
            {
                *pixel++ = (uint32_t)((200 << 16 | 200 << 8 | 200));
            }
            else 
            {
                *pixel++ = 0;
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

    pState->circ_pos_x = 200.0f;
    pState->circ_pos_y = 200.0f;
    pState->circ_r = 50.0f;
    
    pState->parab_h = 640.0f;
    pState->parab_k = 600.0f;
    pState->parab_a = -0.005f;

    render(&globalBackBuffer, pState, 1);

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

    case WM_SIZE:
        Dimensions dimension = getWindowDimensions(hwnd);
        resizeDIBSection(&globalBackBuffer, dimension.width, dimension.height);
        RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE|RDW_INTERNALPAINT);
        
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            Dimensions dimension = getWindowDimensions(hwnd);
            render(&globalBackBuffer, pState, 1);
            displayBufferInWindow(&globalBackBuffer, hdc, dimension.width, dimension.height);

            EndPaint(hwnd, &ps);
        }
        return 0;

    case WM_KEYDOWN:
        if (wParam == VK_UP) 
        {
            pState->parab_k -= 5;
        }
        if (wParam == VK_DOWN)
        {
            pState->parab_k += 5;
        }
        if (wParam == VK_RIGHT)
        {
            pState->parab_h += 5;
        }
        if (wParam == VK_LEFT)
        {
            pState->parab_h -= 5;
        }
        if (wParam == 'W')
        {
            pState->parab_a += 2;
        }
        if (wParam == 'S')
        {
            pState->parab_a -= 2;
        }
        if (wParam == 'A')
        {
            pState->rect_width -= 2;
        }
        if (wParam == 'D')
        {
            pState->rect_width += 2;
        }
        RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE|RDW_INTERNALPAINT);
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
