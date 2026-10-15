#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void dgc_Enable(int cap);
void dgc_Disable(int cap);
void dgc_EnableClientState(int cap);
void dgc_DisableClientState(int cap);
void dgc_AlphaFunc(int func, float ref);
void dgc_BlendFunc(int sfactor, int dfactor);
void dgc_DepthFunc(int func);
void dgc_DepthMask(int flag);
void dgc_ColorMask(int r, int g, int b, int a);
void dgc_ClearColor(float r, float g, float b, float a);
void dgc_Clear(int mask);
void dgc_Viewport(int x, int y, int w, int h);
void dgc_Scissor(int x, int y, int w, int h);
void dgc_Hint(int target, int mode);
void dgc_MatrixMode(int mode);
void dgc_LoadIdentity(void);
void dgc_LoadMatrixf(const float* m);
void dgc_Fogf(int pname, float param);
void dgc_Fogi(int pname, int param);
void dgc_Fogfv(int pname, const float* params);
void dgc_GetIntegerv(int pname, int* data);
const char* dgc_GetString(int name);
void dgc_ReadPixels(int x, int y, int w, int h, int format, int type, void* pixels);

unsigned int dgc_GenTexture(void);
void dgc_DeleteTexture(unsigned int id);
void dgc_BindTexture(unsigned int id);
void dgc_TexParameteri(int target, int pname, int param);
void dgc_TexImage2D(int level, int internalformat, int width, int height, int border, int format, int type, const void* pixels);
void dgc_TexSubImage2D(int level, int xoffset, int yoffset, int width, int height, int format, int type, const void* pixels);

void dgc_VertexPointer(int size, int type, int stride, const void* ptr);
void dgc_ColorPointer(int size, int type, int stride, const void* ptr);
void dgc_TexCoordPointer(int size, int type, int stride, const void* ptr);
void dgc_DrawArrays(int mode, int first, int count);
void dgc_DrawElements(int mode, int count, int type, const void* indices);

/* Vertex/index buffer objects (loaded via wglGetProcAddress) */
int  dgc_LoadBuffers(void);
void dgc_GenBuffers(int n, unsigned int* ids);
void dgc_DeleteBuffers(int n, const unsigned int* ids);
void dgc_BindBuffer(int target, unsigned int id);
void dgc_BufferData(int target, unsigned int size, const void* data, int usage);
void dgc_BufferSubData(int target, unsigned int offset, unsigned int size, const void* data);

int dgc_GetError(void);

#ifdef __cplusplus
}
#endif
