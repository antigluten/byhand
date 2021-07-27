#include <Windows.h>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    GetModuleHandle(0);
    MessageBox(0, "Just keep going...", "antigluten", MB_OK);
    return 0;
}
