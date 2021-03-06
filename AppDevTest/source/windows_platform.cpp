#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <stdio.h>
#include <stdint.h>

#include "my_math.h"

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

internal uint8_t checkSuperPixel(float x, float y, StateInfo *state, int superPixelSize, float thickness)
{
    // We check the minimum distance between the center of the super pixel and the curve
    // We calculate t in Q = P + CP*t such that |CP*t| = d
    float center_x = x*superPixelSize + superPixelSize/2.0f;
    float center_y = y*superPixelSize + superPixelSize/2.0f;
    float x_mindist = newtonFindZero(center_x, center_y, state);

    v2f P = {x_mindist, parabola(x_mindist, state)};
    v2f C = {center_x, center_y};

    if (vecLen(vecAdd(C, vecNeg(P))) < superPixelSize/2.0f)
    {
        return 1;
    }

    float t = (thickness) / sqrtf(sqr(C.x - P.x) + sqr(C.y - P.y));

    v2f Q = vecAdd(P, vecMult(vecAdd(C, vecNeg(P)), t));

    // Then check if Q is within the bounds of the super pixel
    float margin = 4.0f;
    if (fabsf(Q.x - center_x) - margin <= superPixelSize/2.0f &&
            fabsf(Q.y - center_y) - margin <= superPixelSize/2.0f)
    {
        return 1;
    }
    else 
    {
        return 0;
    }
}

internal uint8_t checkNewton(float x, float y, StateInfo *state, int superPixelSize, float thickness)
{
    float x_mindist = newtonFindZero(x, y, state);
    float minDistSqr = squareDistancePointCurve(x_mindist, state, x, y);
    if (minDistSqr <= sqr(thickness))
    {
        return 1;
    }
    else
    {

        return 0;
    }
}

internal void renderGraphics(OffscreenBuffer *buffer, StateInfo *state, int superPixelSize)
{
    // Zero the frame buffer
    memset(buffer->memory, 0, (buffer->height*buffer->width)*buffer->bytesPerPixel);
    // First we compute the super grid according to the superPixelSize
    int superGridWidth = buffer->width/superPixelSize;
    if (buffer->width % superPixelSize != 0)
    {
        superGridWidth++;
    }

    int superGridHeight = buffer->height/superPixelSize;
    if (buffer->height % superPixelSize != 0)
    {
        superGridHeight++;
    }

    float thickness = 8.0f;

    // Now we loop through the super grid
    for (int super_y = 0; super_y < superGridHeight; super_y++)
    {
        for (int super_x = 0; super_x < superGridWidth; super_x++)
        {
            if (checkSuperPixel(super_x, super_y, state, superPixelSize, thickness))
            {
                // Compute the starting pixel of the super grid cell
                uint32_t *row = (uint32_t *)buffer->memory + (super_y*superPixelSize*buffer->width) + super_x*superPixelSize;
                // Then loop through all the pixels in the cell
                for (int y = super_y*superPixelSize; 
                        y < super_y*superPixelSize+superPixelSize && y < buffer->height; 
                        y++)
                {
                    uint32_t *pixel = row;

                    for (int x = super_x*superPixelSize; 
                            x < super_x*superPixelSize+superPixelSize && x < buffer->width; 
                            x++)
                    {
                        if (x == 640 && y == 600)
                        {
                            int z = 0;
                        }

                        float factor = 0.0f;
                        float center_x = x + 0.5f;
                        float center_y = y + 0.5f;
                        float x_mindist = newtonFindZero(center_x, center_y, state);

                        v2f P = {x_mindist, parabola(x_mindist, state)};
                        v2f C = {center_x, center_y};

                        float t = (thickness) / sqrtf(sqr(C.x - P.x) + sqr(C.y - P.y));

                        v2f Q = vecAdd(P, vecMult(vecAdd(C, vecNeg(P)), t));

                        if (t >= 1.0f && (fabsf(C.x - Q.x) >= 0.5f || fabsf(C.y - Q.y) >= 0.5f))
                        {
                            factor = 1.0f;
                        }
                        else if (fabsf(C.x - Q.x) < 0.5f && fabsf(C.y - Q.y) < 0.5f)
                        {
                            float distQC = vecDist(Q, C);
                            if (distQC < 0.2f)
                            {
                                factor = 0.5f;
                            }
                            else
                            {
                                if (t >= 1)
                                {
                                    factor = 0.75f;
                                }
                                else {
                                    factor = 0.25f;
                                }
                            }
                        }

                        // TODO: fix this. Currently doesn't work cause if factor = 0, we get black instead of red.
                        if (state->showGrid)
                        {
                            *pixel++ = (uint32_t)((int)(200 + factor*0) << 16 | (int)(0 + factor*200) << 8 | (int)(0 + factor*200));
                        }
                        else 
                        {
                            *pixel++ = (uint32_t)((int)(0 + factor*200) << 16 | (int)(0 + factor*200) << 8 | (int)(0 + factor*200));
                        }
                    }

                    row += buffer->width;
                }
            }
        }
    }
}

// Legacy render code
internal void render(OffscreenBuffer *buffer, StateInfo *state, int superSample)
{
    float factor = 0.0f;
    float thickness = 8.0f*superSample;
    uint8_t *row = (uint8_t *)buffer->memory;
    for (int y = 0; y < buffer->height*superSample; y+=superSample)
    {
        uint32_t *pixel = (uint32_t *)row;
        for (int x = 0; x < buffer->width*superSample; x+=superSample)
        {
            factor = 0.0f;
            for (int i = 0; i < superSample; i++)
            {
                for (int k = 0; k < superSample; k++)
                {
                    if (fabsf(parabola(x+k, state) - y+i) < 4 * thickness)
                    {
                        float x_mindist = newtonFindZero(x+k, y+i, state); 
                        float minDistSqr = squareDistancePointCurve(x_mindist, state, x+k, y+i);

                        if (minDistSqr <= sqr(thickness))
                        {
                            factor += 0.25f;
                        }
                    //float x_mindist = newtonFindZero(x, y, state); 
                    //float minDistSqr = squareDistancePointCurve(x_mindist, state, x, y);
                    //if (minDistSqr <= 16)
                    //{
                        //factor += 1.0f/(superSample*superSample);
                    }

                }

            }
            *pixel++ = (uint32_t)(factor*(200 << 16 | 200 << 8 | 200));
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
    pState->parab_h = 640.0f;
    pState->parab_k = 600.0f;
    pState->parab_a = -0.005f;

    renderGraphics(&globalBackBuffer, pState, 20);

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
            renderGraphics(&globalBackBuffer, pState, 20);
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
            pState->parab_a += 0.0005f;
        }
        if (wParam == 'S')
        {
            pState->parab_a -= 0.0005f;
        }
        if (wParam == 'A')
        {
            pState->showGrid = !(pState->showGrid);
        }
        RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE|RDW_INTERNALPAINT);
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
