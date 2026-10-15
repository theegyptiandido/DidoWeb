#include <windows.h>
#include <GL/gl.h>

#pragma comment(lib, "opengl32.lib")

typedef void (APIENTRY *PFN_glGenBuffers)(int, unsigned*);
typedef void (APIENTRY *PFN_glDeleteBuffers)(int, const unsigned*);
typedef void (APIENTRY *PFN_glBindBuffer)(unsigned, unsigned);
typedef void (APIENTRY *PFN_glBufferData)(unsigned, long long, const void*, unsigned);
typedef void (APIENTRY *PFN_glBufferSubData)(unsigned, long long, long long, const void*);

static PFN_glGenBuffers    p_glGenBuffers;
static PFN_glDeleteBuffers p_glDeleteBuffers;
static PFN_glBindBuffer    p_glBindBuffer;
static PFN_glBufferData    p_glBufferData;
static PFN_glBufferSubData p_glBufferSubData;

static void* GetGLProc(const char* core, const char* arb) {
    void* p = (void*)wglGetProcAddress(core);
    if (!p && arb) p = (void*)wglGetProcAddress(arb);
    if (!p) p = (void*)GetProcAddress(GetModuleHandleA("opengl32.dll"), core);
    return p;
}

extern "C" int dgc_LoadBuffers(void) {
    p_glGenBuffers    = (PFN_glGenBuffers)   GetGLProc("glGenBuffers",    "glGenBuffersARB");
    p_glDeleteBuffers = (PFN_glDeleteBuffers)GetGLProc("glDeleteBuffers", "glDeleteBuffersARB");
    p_glBindBuffer    = (PFN_glBindBuffer)   GetGLProc("glBindBuffer",    "glBindBufferARB");
    p_glBufferData    = (PFN_glBufferData)   GetGLProc("glBufferData",    "glBufferDataARB");
    p_glBufferSubData = (PFN_glBufferSubData)GetGLProc("glBufferSubData", "glBufferSubDataARB");
    return p_glGenBuffers && p_glDeleteBuffers && p_glBindBuffer && p_glBufferData && p_glBufferSubData;
}

extern "C" {

void dgc_Enable(int cap) { glEnable(cap); }
void dgc_Disable(int cap) { glDisable(cap); }
void dgc_EnableClientState(int cap) { glEnableClientState(cap); }
void dgc_DisableClientState(int cap) { glDisableClientState(cap); }
void dgc_AlphaFunc(int func, float ref) { glAlphaFunc(func, ref); }
void dgc_BlendFunc(int sfactor, int dfactor) { glBlendFunc(sfactor, dfactor); }
void dgc_DepthFunc(int func) { glDepthFunc(func); }
void dgc_DepthMask(int flag) { glDepthMask(flag); }
void dgc_ColorMask(int r, int g, int b, int a) { glColorMask(r, g, b, a); }
void dgc_ClearColor(float r, float g, float b, float a) { glClearColor(r, g, b, a); }
void dgc_Clear(int mask) { glClear(mask); }
void dgc_Viewport(int x, int y, int w, int h) { glViewport(x, y, w, h); }
void dgc_Scissor(int x, int y, int w, int h) { glScissor(x, y, w, h); }
void dgc_Hint(int target, int mode) { glHint(target, mode); }
void dgc_MatrixMode(int mode) { glMatrixMode(mode); }
void dgc_LoadIdentity(void) { glLoadIdentity(); }
void dgc_LoadMatrixf(const float* m) { glLoadMatrixf(m); }
void dgc_Fogf(int pname, float param) { glFogf(pname, param); }
void dgc_Fogi(int pname, int param) { glFogi(pname, param); }
void dgc_Fogfv(int pname, const float* params) { glFogfv(pname, params); }
void dgc_GetIntegerv(int pname, int* data) { glGetIntegerv(pname, data); }
const char* dgc_GetString(int name) { return (const char*)glGetString(name); }
void dgc_ReadPixels(int x, int y, int w, int h, int format, int type, void* pixels) {
    glReadPixels(x, y, w, h, format, type, pixels);
}

unsigned int dgc_GenTexture(void) { GLuint id = 0; glGenTextures(1, &id); return id; }
void dgc_DeleteTexture(unsigned int id) { GLuint v = id; glDeleteTextures(1, &v); }
void dgc_BindTexture(unsigned int id) { glBindTexture(GL_TEXTURE_2D, id); }
void dgc_TexParameteri(int target, int pname, int param) { glTexParameteri(target, pname, param); }
void dgc_TexImage2D(int level, int internalformat, int width, int height, int border, int format, int type, const void* pixels) {
    glTexImage2D(GL_TEXTURE_2D, level, internalformat, width, height, border, format, type, pixels);
}
void dgc_TexSubImage2D(int level, int xoffset, int yoffset, int width, int height, int format, int type, const void* pixels) {
    glTexSubImage2D(GL_TEXTURE_2D, level, xoffset, yoffset, width, height, format, type, pixels);
}

void dgc_VertexPointer(int size, int type, int stride, const void* ptr) {
    glVertexPointer(size, type, stride, ptr);
}
void dgc_ColorPointer(int size, int type, int stride, const void* ptr) {
    glColorPointer(size, type, stride, ptr);
}
void dgc_TexCoordPointer(int size, int type, int stride, const void* ptr) {
    glTexCoordPointer(size, type, stride, ptr);
}
void dgc_DrawArrays(int mode, int first, int count) { glDrawArrays(mode, first, count); }
void dgc_DrawElements(int mode, int count, int type, const void* indices) {
    glDrawElements(mode, count, type, indices);
}

void dgc_GenBuffers(int n, unsigned* ids) { if (p_glGenBuffers) p_glGenBuffers(n, ids); }
void dgc_DeleteBuffers(int n, const unsigned* ids) { if (p_glDeleteBuffers) p_glDeleteBuffers(n, ids); }
void dgc_BindBuffer(int target, unsigned id) { if (p_glBindBuffer) p_glBindBuffer((unsigned)target, id); }
void dgc_BufferData(int target, unsigned size, const void* data, int usage) {
    if (p_glBufferData) p_glBufferData((unsigned)target, (long long)size, data, (unsigned)usage);
}
void dgc_BufferSubData(int target, unsigned offset, unsigned size, const void* data) {
    if (p_glBufferSubData) p_glBufferSubData((unsigned)target, (long long)offset, (long long)size, data);
}

int dgc_GetError(void) { return (int)glGetError(); }

}
