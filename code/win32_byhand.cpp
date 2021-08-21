#include <Windows.h>

#define internal static
#define local_persist static
#define global_variable static

//TODO: global for the time
global_variable bool running;

global_variable BITMAPINFO bitmapInfo;
global_variable void *bitmapMemory;
global_variable HBITMAP bitmapHandle;
global_variable HDC bitmapDeviceContext;


internal void win32resizeDIBSelection(int width, int height)
{

    //don't free first, free after, then free if that fails
    //free our DIBSection
    
    if (bitmapHandle)
    {
        DeleteObject(bitmapHandle);
    }
    else
    {
        bitmapDeviceContext = CreateCompatibleDC(0);
    }

    bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
    bitmapInfo.bmiHeader.biWidth = width;
    bitmapInfo.bmiHeader.biHeight = height;
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 32;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;

    // if it's already a global variable the parameters below are set to zeros automatically

    // bitmapInfo.bmiHeader.biSizeImage = 0;
    // bitmapInfo.bmiHeader.biXPelsPerMeter = 0;
    // bitmapInfo.bmiHeader.biYPelsPerMeter = 0;
    // bitmapInfo.bmiHeader.biClrUsed = 0;
    // bitmapInfo.bmiHeader.biClrImportant = 0;

    


    bitmapHandle = CreateDIBSection(bitmapDeviceContext, &bitmapInfo, DIB_RGB_COLORS, &bitmapMemory, 0, 0);


}

//hdc -> deviceContext

internal void win32UpdateWindow(HDC hdc, int x, int y, int width, int height)
{
    StretchDIBits(hdc, x, y, width, height, x, y, width, height, bitmapMemory, &bitmapInfo, DIB_RGB_COLORS, SRCCOPY);
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
                win32UpdateWindow(deviceContext, x, y, width, height);
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
    //TODO
    WNDCLASS WindowClass = {};

    //TODO does style options matter? 
    
    WindowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
    WindowClass.lpfnWndProc = win32MainWindowCallback;
    WindowClass.hInstance = hInstance;
    //WindowClass.hIcon = ;
    WindowClass.lpszClassName = "ByHandWindowClass";
    
    if (RegisterClass(&WindowClass))
    {
        HWND WindowHandle = CreateWindowEx(0, WindowClass.lpszClassName, "Byhand Hero",
                WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                CW_USEDEFAULT, 0, 0, WindowClass.hInstance, 0);
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
