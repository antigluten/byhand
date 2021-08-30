#include <Windows.h>
#include <stdint.h>

#define internal static
#define local_persist static
#define global_variable static

// NOTE: WTF? WHAT IS THE SEQUENCE OF WORDS BELOW:
typedef unsigned char uint8;

// TODO: global for the time
global_variable bool running;

global_variable BITMAPINFO bitmapInfo;
global_variable void *bitmapMemory;

// NOTE don't forget to move it from global variables
global_variable int bitmapWidth;
global_variable int bitmapHeight;


internal void win32resizeDIBSelection(int width, int height)
{

    // TODO don't free first, free after, then free if that fails
    // free our DIBSection


    if (bitmapMemory)
    {
        // NOTE think about page protection to prevent making bugs
        
        VirtualFree(bitmapMemory, 0, MEM_RELEASE); 
    }

    bitmapWidth = width;
    bitmapHeight = height;

    bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
    bitmapInfo.bmiHeader.biWidth = bitmapWidth;
    // NOTE with negative `bitmapHeight` it goes top-down DIB
    bitmapInfo.bmiHeader.biHeight = -bitmapHeight;
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 32;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;
    
    // NOTE we need memory, but the question is "how much memory do we need?"

    int bytesPerPixel = 4;
    int bitmapMemorySize = (bitmapWidth * bitmapHeight) * bytesPerPixel;

    // NOTE DAY 004: VirtualAlloc function
    // What is a fucking page in memory allocation?
    // because of we don't know where the memory is, the first parameter is zero

    bitmapMemory = VirtualAlloc(0, bitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);

    int pitch = width * bytesPerPixel; 
    uint8_t *row = (uint8_t *) bitmapMemory;
    for (int y = 0; y < bitmapHeight; ++y)
    {
        uint8_t *pixel = (uint8_t *) row;
        for (int x = 0; x < bitmapWidth; ++x)
        {
            /*
                                       pixel +0 +1 +2 +3 
            NOTE: pixel structure in memory: 00 00 00 00

            */

           int half = bitmapWidth / 2;

           if (x < half)
            {
                *pixel = 0;
                ++pixel;

                *pixel = 0;
                ++pixel;
            
                *pixel = 255;
                ++pixel; 
            
                *pixel = 100;
                ++pixel;
            } 
            else 
            {
                *pixel = 0;
                ++pixel;

                *pixel = 255;
                ++pixel;
            
                *pixel = 0;
                ++pixel; 
            
                *pixel = 100;
                ++pixel;
            }
        }

        row += pitch;

    }
}

internal void win32UpdateWindow(HDC hdc, RECT *windowRect, int x, int y, int width, int height)
{
    int windowWidth = windowRect->right - windowRect->left;
    int windowHeight = windowRect->bottom - windowRect->top;

    StretchDIBits(hdc, 0, 0, bitmapWidth, bitmapHeight, 0, 0, windowWidth, windowHeight, bitmapMemory, &bitmapInfo, DIB_RGB_COLORS, SRCCOPY);
}


LRESULT CALLBACK win32MainWindowCallback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    switch(msg)
    {
        case WM_SIZE:
            {
                RECT clientRect;
                GetClientRect(hWnd, &clientRect);
                int width = clientRect.right - clientRect.left;
                int height = clientRect.bottom - clientRect.top;
                win32resizeDIBSelection(width, height);
                OutputDebugStringA("WM_SIZE\n");
            } break;
        case WM_DESTROY:
            {
                OutputDebugStringA("WM_DESTROY\n");
            } break;
        case WM_CLOSE:
            {
                running = false;
                OutputDebugStringA("WM_CLOSE\n");
            } break;
        case WM_ACTIVATEAPP:
            {
                OutputDebugStringA("WM_ACTIVATEAPP\n");
            } break;
        case WM_PAINT:
            {
                PAINTSTRUCT paint;
                HDC deviceContext = BeginPaint(hWnd, &paint);
                int x = paint.rcPaint.left;                 
                int y = paint.rcPaint.top;
                int width = paint.rcPaint.right - paint.rcPaint.left;
                int height = paint.rcPaint.bottom - paint.rcPaint.top;

                RECT clientRect;
                GetClientRect(hWnd, &clientRect);

                // TODO update arguments
                win32UpdateWindow(deviceContext, &clientRect, x, y, width, height);
                EndPaint(hWnd, &paint);
            } break; 
        default:
            {
              OutputDebugStringA("defautl\n");
              result = DefWindowProc(hWnd, msg, wParam, lParam);
            } break;
    }
    return (result);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    WNDCLASS windowClass = {};

    // TODO does style options matter? 
    
    windowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
    windowClass.lpfnWndProc = win32MainWindowCallback;
    windowClass.hInstance = hInstance;
    //WindowClass.hIcon = ;
    windowClass.lpszClassName = "ByHandWindowClass";
    
    if (RegisterClass(&windowClass))
    {
        HWND WindowHandle = CreateWindowEx(0, windowClass.lpszClassName, "Byhand Hero",
                WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                CW_USEDEFAULT, 0, 0, windowClass.hInstance, 0);
        if (WindowHandle)
        {
            MSG msg;
            running = true;
            while(running)
            {
                BOOL msgResult = GetMessageA(&msg, 0, 0, 0);
                if (msgResult > 0)
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
                else
                {
                    break;
                }
            }
        }
        else
        {
            //TODO
        }
    }
    else 
    {
        //TODO
    }

    return 0;
}
