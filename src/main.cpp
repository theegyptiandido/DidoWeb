#include <windows.h>
#include "platform.h"
#include "didoweb.h"

void DemoInit();
void DemoUpdate(float dt);
void DemoRender();

int main() {
    if (!platform::CreateGameWindow(L"DidoWeb - WebGL-style Demo", 800, 600))
        return 1;

    if (!dw::Init(800, 600)) {
        MessageBoxA(nullptr, "DidoWeb: init failed (need OpenGL 2.0 + VBO support).", "Error", MB_OK);
        return 1;
    }

    DemoInit();

    LARGE_INTEGER freq, prev, now;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&prev);

    while (platform::PumpMessages()) {
        QueryPerformanceCounter(&now);
        float dt = (float)(now.QuadPart - prev.QuadPart) / (float)freq.QuadPart;
        prev = now;
        if (dt > 0.1f) dt = 0.1f;

        DemoUpdate(dt);
        DemoRender();
        dw::SwapBuffers();
        platform::ClearPressed();
    }
    return 0;
}
