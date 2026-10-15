#pragma once
#include <windows.h>

namespace platform {
bool CreateGameWindow(const wchar_t* title, int width, int height);
HWND Window();
HDC DeviceContext();
int Width();
int Height();
void SwapBuffers();
bool PumpMessages();
bool KeyDown(int vk);
bool KeyPressed(int vk);
void ClearPressed();
void SetMouseCapture(bool on);
bool MouseCaptured();
void PollMouseDelta(float& dx, float& dy);
}
