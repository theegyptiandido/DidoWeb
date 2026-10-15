#include "didoweb.h"
#include "didogl_c.h"
#include "platform.h"
#include <windows.h>
#include <GL/gl.h>

#ifndef GL_VERTEX_SHADER
#define GL_VERTEX_SHADER   0x8B31
#endif
#ifndef GL_FRAGMENT_SHADER
#define GL_FRAGMENT_SHADER 0x8B30
#endif
#ifndef GL_COMPILE_STATUS
#define GL_COMPILE_STATUS  0x8B81
#endif
#ifndef GL_LINK_STATUS
#define GL_LINK_STATUS     0x8B82
#endif

typedef unsigned int GLuint_t;
typedef int GLint_t;

typedef GLuint_t (APIENTRY *PFN_CreateShader)(unsigned);
typedef void (APIENTRY *PFN_ShaderSource)(GLuint_t, int, const char* const*, const int*);
typedef void (APIENTRY *PFN_CompileShader)(GLuint_t);
typedef void (APIENTRY *PFN_GetShaderiv)(GLuint_t, unsigned, int*);
typedef void (APIENTRY *PFN_GetShaderInfoLog)(GLuint_t, int, int*, char*);
typedef GLuint_t (APIENTRY *PFN_CreateProgram)(void);
typedef void (APIENTRY *PFN_AttachShader)(GLuint_t, GLuint_t);
typedef void (APIENTRY *PFN_LinkProgram)(GLuint_t);
typedef void (APIENTRY *PFN_GetProgramiv)(GLuint_t, unsigned, int*);
typedef void (APIENTRY *PFN_UseProgram)(GLuint_t);
typedef int (APIENTRY *PFN_GetAttribLocation)(GLuint_t, const char*);
typedef int (APIENTRY *PFN_GetUniformLocation)(GLuint_t, const char*);
typedef void (APIENTRY *PFN_EnableVertexAttribArray)(GLuint_t);
typedef void (APIENTRY *PFN_VertexAttribPointer)(GLuint_t, int, unsigned, unsigned char, int, const void*);
typedef void (APIENTRY *PFN_UniformMatrix4fv)(int, int, unsigned char, const float*);
typedef void (APIENTRY *PFN_Uniform1f)(int, float);
typedef void (APIENTRY *PFN_Uniform3f)(int, float, float, float);

static PFN_CreateShader p_CreateShader;
static PFN_ShaderSource p_ShaderSource;
static PFN_CompileShader p_CompileShader;
static PFN_GetShaderiv p_GetShaderiv;
static PFN_GetShaderInfoLog p_GetShaderInfoLog;
static PFN_CreateProgram p_CreateProgram;
static PFN_AttachShader p_AttachShader;
static PFN_LinkProgram p_LinkProgram;
static PFN_GetProgramiv p_GetProgramiv;
static PFN_UseProgram p_UseProgram;
static PFN_GetAttribLocation p_GetAttribLocation;
static PFN_GetUniformLocation p_GetUniformLocation;
static PFN_EnableVertexAttribArray p_EnableVertexAttribArray;
static PFN_VertexAttribPointer p_VertexAttribPointer;
static PFN_UniformMatrix4fv p_UniformMatrix4fv;
static PFN_Uniform1f p_Uniform1f;
static PFN_Uniform3f p_Uniform3f;

static void* Proc(const char* name) {
    void* p = (void*)wglGetProcAddress(name);
    if (!p) p = (void*)GetProcAddress(GetModuleHandleA("opengl32.dll"), name);
    return p;
}

namespace dw {

bool Init(int width, int height) {
    p_CreateShader     = (PFN_CreateShader)Proc("glCreateShader");
    p_ShaderSource     = (PFN_ShaderSource)Proc("glShaderSource");
    p_CompileShader    = (PFN_CompileShader)Proc("glCompileShader");
    p_GetShaderiv      = (PFN_GetShaderiv)Proc("glGetShaderiv");
    p_GetShaderInfoLog = (PFN_GetShaderInfoLog)Proc("glGetShaderInfoLog");
    p_CreateProgram    = (PFN_CreateProgram)Proc("glCreateProgram");
    p_AttachShader     = (PFN_AttachShader)Proc("glAttachShader");
    p_LinkProgram      = (PFN_LinkProgram)Proc("glLinkProgram");
    p_GetProgramiv     = (PFN_GetProgramiv)Proc("glGetProgramiv");
    p_UseProgram       = (PFN_UseProgram)Proc("glUseProgram");
    p_GetAttribLocation  = (PFN_GetAttribLocation)Proc("glGetAttribLocation");
    p_GetUniformLocation = (PFN_GetUniformLocation)Proc("glGetUniformLocation");
    p_EnableVertexAttribArray = (PFN_EnableVertexAttribArray)Proc("glEnableVertexAttribArray");
    p_VertexAttribPointer     = (PFN_VertexAttribPointer)Proc("glVertexAttribPointer");
    p_UniformMatrix4fv = (PFN_UniformMatrix4fv)Proc("glUniformMatrix4fv");
    p_Uniform1f        = (PFN_Uniform1f)Proc("glUniform1f");
    p_Uniform3f        = (PFN_Uniform3f)Proc("glUniform3f");

    if (!dgc_LoadBuffers()) return false; /* DidoGL's VBO functions */

    dgc_Enable(0x0B71); /* GL_DEPTH_TEST */
    dgc_Viewport(0, 0, width, height);
    return p_CreateShader && p_CreateProgram && p_UseProgram && p_VertexAttribPointer;
}

void SwapBuffers() { platform::SwapBuffers(); }

void ClearColor(float r, float g, float b, float a) { dgc_ClearColor(r, g, b, a); }
void Clear(int mask) { dgc_Clear(mask); }
void Enable(int cap) { dgc_Enable(cap); }
void Disable(int cap) { dgc_Disable(cap); }
void DepthFunc(int func) { dgc_DepthFunc(func); }
void Viewport(int x, int y, int w, int h) { dgc_Viewport(x, y, w, h); }

Buffer CreateBuffer() { unsigned id = 0; dgc_GenBuffers(1, &id); return id; }
void BindBuffer(int target, Buffer b) { dgc_BindBuffer(target, b); }
void BufferData(int target, const void* data, int size, int usage) { dgc_BufferData(target, size, data, usage); }
void DeleteBuffer(Buffer b) { dgc_DeleteBuffers(1, &b); }

Shader CreateShader(int type) { return p_CreateShader((unsigned)type); }

void ShaderSource(Shader s, const char* src) {
    const char* arr[1] = { src };
    p_ShaderSource(s, 1, arr, 0);
}

void CompileShader(Shader s) { p_CompileShader(s); }

bool GetShaderStatus(Shader s) {
    int ok = 0;
    p_GetShaderiv(s, GL_COMPILE_STATUS, &ok);
    return ok != 0;
}

const char* GetShaderInfoLog(Shader s) {
    static char log[2048];
    int len = 0;
    p_GetShaderInfoLog(s, sizeof(log) - 1, &len, log);
    log[len < 0 ? 0 : len] = 0;
    return log;
}

void DeleteShader(Shader s) { (void)s; }

Program CreateProgram() { return p_CreateProgram(); }
void AttachShader(Program p, Shader s) { p_AttachShader(p, s); }
void LinkProgram(Program p) { p_LinkProgram(p); }

bool GetProgramStatus(Program p) {
    int ok = 0;
    p_GetProgramiv(p, GL_LINK_STATUS, &ok);
    return ok != 0;
}

void UseProgram(Program p) { p_UseProgram(p); }
int GetAttribLocation(Program p, const char* name) { return p_GetAttribLocation(p, name); }
int GetUniformLocation(Program p, const char* name) { return p_GetUniformLocation(p, name); }

void EnableVertexAttribArray(int loc) { p_EnableVertexAttribArray((GLuint_t)loc); }

void VertexAttribPointer(int loc, int size, int type, bool normalized, int stride, const void* offset) {
    p_VertexAttribPointer((GLuint_t)loc, size, (unsigned)type, normalized ? 1 : 0, stride, offset);
}

void UniformMatrix4fv(int loc, const float* m) { p_UniformMatrix4fv(loc, 1, 0, m); }
void Uniform1f(int loc, float v) { p_Uniform1f(loc, v); }
void Uniform3f(int loc, float x, float y, float z) { p_Uniform3f(loc, x, y, z); }

void DrawArrays(int mode, int first, int count) { dgc_DrawArrays(mode, first, count); }
void DrawElements(int mode, int count, int type, const void* offset) { dgc_DrawElements(mode, count, type, offset); }

}
