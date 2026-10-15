#include "platform.h"

namespace platform {

static HWND g_hwnd = nullptr;
static HDC g_hdc = nullptr;
static HGLRC g_glrc = nullptr;
static int g_w = 0, g_h = 0;
static bool g_running = true;
static bool g_keys[256] = {};
static bool g_pressed[256] = {};

static LRESULT CALLBACK WndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    switch (m) {
    case WM_CLOSE:
        DestroyWindow(h);
        return 0;
    case WM_DESTROY:
        g_running = false;
        PostQuitMessage(0);
        return 0;
    case WM_KEYDOWN:
        if (!(l & (1 << 30))) g_pressed[w & 0xff] = true;
        g_keys[w & 0xff] = true;
        return 0;
    case WM_KEYUP:
        g_keys[w & 0xff] = false;
        return 0;
    }
    return DefWindowProc(h, m, w, l);
}

bool CreateGameWindow(const wchar_t* title, int width, int height) {
    g_w = width;
    g_h = height;

    HINSTANCE inst = GetModuleHandle(nullptr);
    WNDCLASS wc = {};
    wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = inst;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = L"DIYQME_GL";
    RegisterClass(&wc);

    RECT r = { 0, 0, width, height };
    AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW, FALSE);
    g_hwnd = CreateWindowEx(0, L"DIYQME_GL", title, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, r.right - r.left, r.bottom - r.top,
        nullptr, nullptr, inst, nullptr);
    if (!g_hwnd) return false;

    g_hdc = GetDC(g_hwnd);

    PIXELFORMATDESCRIPTOR pfd = { sizeof(PIXELFORMATDESCRIPTOR) };
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int pf = ChoosePixelFormat(g_hdc, &pfd);
    if (!pf) return false;
    if (!SetPixelFormat(g_hdc, pf, &pfd)) return false;

    g_glrc = wglCreateContext(g_hdc);
    if (!g_glrc) return false;
    if (!wglMakeCurrent(g_hdc, g_glrc)) return false;

    ShowWindow(g_hwnd, SW_SHOW);
    UpdateWindow(g_hwnd);
    return true;
}

HWND Window() { return g_hwnd; }
HDC DeviceContext() { return g_hdc; }
int Width() { return g_w; }
int Height() { return g_h; }
void SwapBuffers() { ::SwapBuffers(g_hdc); }

bool PumpMessages() {
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return g_running;
}

bool KeyDown(int vk) { return g_keys[vk & 0xff]; }
bool KeyPressed(int vk) { return g_pressed[vk & 0xff]; }
void ClearPressed() { for (int i = 0; i < 256; ++i) g_pressed[i] = false; }

static bool g_capture = true;

void SetMouseCapture(bool on) {
    g_capture = on;
    ShowCursor(on ? FALSE : TRUE);
}

bool MouseCaptured() { return g_capture; }

void PollMouseDelta(float& dx, float& dy) {
    dx = 0.0f; dy = 0.0f;
    if (!g_capture) return;
    POINT p;
    GetCursorPos(&p);
    POINT c = { g_w / 2, g_h / 2 };
    ClientToScreen(g_hwnd, &c);
    dx = (float)(p.x - c.x);
    dy = (float)(p.y - c.y);
    SetCursorPos(c.x, c.y);
}

}
