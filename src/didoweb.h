#pragma once

/* DidoWeb - a compiled, WebGL-style API built on top of DidoGL.
   Mimics the WebGL object model: buffers, shaders, programs, attributes,
   uniforms, and draw calls. Renders to a desktop window through DidoGL. */

namespace dw {

enum { ARRAY_BUFFER = 0x8892, ELEMENT_ARRAY_BUFFER = 0x8893 };
enum { STATIC_DRAW = 0x88E4, DYNAMIC_DRAW = 0x88E8 };
enum { VERTEX_SHADER = 0x8B31, FRAGMENT_SHADER = 0x8B30 };
enum { FLOAT = 0x1406, UNSIGNED_SHORT = 0x1403 };
enum { POINTS = 0x0000, LINES = 0x0001, TRIANGLES = 0x0004 };
enum { DEPTH_TEST = 0x0B71 };
enum { COLOR_BUFFER_BIT = 0x4000, DEPTH_BUFFER_BIT = 0x0100 };
enum { LEQUAL = 0x0203 };

typedef unsigned int Buffer;
typedef unsigned int Shader;
typedef unsigned int Program;

bool Init(int width, int height);
void SwapBuffers();

void ClearColor(float r, float g, float b, float a);
void Clear(int mask);
void Enable(int cap);
void Disable(int cap);
void DepthFunc(int func);
void Viewport(int x, int y, int w, int h);

Buffer CreateBuffer();
void   BindBuffer(int target, Buffer b);
void   BufferData(int target, const void* data, int size, int usage);
void   DeleteBuffer(Buffer b);

Shader CreateShader(int type);
void   ShaderSource(Shader s, const char* src);
void   CompileShader(Shader s);
bool   GetShaderStatus(Shader s);
const char* GetShaderInfoLog(Shader s);
void   DeleteShader(Shader s);

Program CreateProgram();
void    AttachShader(Program p, Shader s);
void    LinkProgram(Program p);
bool    GetProgramStatus(Program p);
void    UseProgram(Program p);
int     GetAttribLocation(Program p, const char* name);
int     GetUniformLocation(Program p, const char* name);

void EnableVertexAttribArray(int loc);
void VertexAttribPointer(int loc, int size, int type, bool normalized, int stride, const void* offset);
void UniformMatrix4fv(int loc, const float* m);
void Uniform1f(int loc, float v);
void Uniform3f(int loc, float x, float y, float z);

void DrawArrays(int mode, int first, int count);
void DrawElements(int mode, int count, int type, const void* offset);
}
