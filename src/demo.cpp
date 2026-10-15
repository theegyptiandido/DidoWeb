#include "didoweb.h"
#include "platform.h"
#include <cmath>
#include <cstring>

static dw::Program g_prog = 0;
static dw::Buffer  g_vbo = 0;
static dw::Buffer  g_ibo = 0;
static int g_posLoc = -1, g_colLoc = -1, g_mvpLoc = -1;
static float g_angle = 0.0f;

static const char* VS =
"attribute vec3 aPos;\n"
"attribute vec3 aCol;\n"
"uniform mat4 uMVP;\n"
"varying vec3 vCol;\n"
"void main() {\n"
"    vCol = aCol;\n"
"    gl_Position = uMVP * vec4(aPos, 1.0);\n"
"}\n";

static const char* FS =
"varying vec3 vCol;\n"
"void main() {\n"
"    gl_FragColor = vec4(vCol, 1.0);\n"
"}\n";

/* interleaved: x,y,z, r,g,b  (24 bytes/vertex), a unit cube */
static const float VERTS[] = {
    -1,-1, 1,  1,0,0,   1,-1, 1,  1,0,0,   1, 1, 1,  1,0,0,  -1, 1, 1,  1,0,0,
     1,-1,-1,  0,1,0,  -1,-1,-1,  0,1,0,  -1, 1,-1,  0,1,0,   1, 1,-1,  0,1,0,
    -1,-1,-1,  0,0,1,  -1,-1, 1,  0,0,1,  -1, 1, 1,  0,0,1,  -1, 1,-1,  0,0,1,
     1,-1, 1,  1,1,0,   1,-1,-1,  1,1,0,   1, 1,-1,  1,1,0,   1, 1, 1,  1,1,0,
    -1, 1, 1,  1,0,1,   1, 1, 1,  1,0,1,   1, 1,-1,  1,0,1,  -1, 1,-1,  1,0,1,
    -1,-1,-1,  0,1,1,   1,-1,-1,  0,1,1,   1,-1, 1,  0,1,1,  -1,-1, 1,  0,1,1,
};

static const unsigned short INDICES[] = {
     0, 1, 2,  0, 2, 3,
     4, 5, 6,  4, 6, 7,
     8, 9,10,  8,10,11,
    12,13,14, 12,14,15,
    16,17,18, 16,18,19,
    20,21,22, 20,22,23,
};

static void Mat4Identity(float* m) {
    for (int i = 0; i < 16; i++) m[i] = 0.0f;
    m[0] = m[5] = m[10] = m[15] = 1.0f;
}

static void Mat4Mul(float* out, const float* a, const float* b) {
    float r[16];
    for (int c = 0; c < 4; c++)
        for (int row = 0; row < 4; row++) {
            float s = 0.0f;
            for (int k = 0; k < 4; k++) s += a[k * 4 + row] * b[c * 4 + k];
            r[c * 4 + row] = s;
        }
    memcpy(out, r, sizeof(r));
}

static void Mat4Perspective(float* m, float fovy, float aspect, float n, float f) {
    Mat4Identity(m);
    float t = 1.0f / tanf(fovy * 0.5f);
    m[0] = t / aspect;
    m[5] = t;
    m[10] = (f + n) / (n - f);
    m[11] = -1.0f;
    m[14] = (2.0f * f * n) / (n - f);
    m[15] = 0.0f;
}

static void Mat4LookAt(float* m, float ex, float ey, float ez) {
    float fx = -ex, fy = -ey, fz = -ez;
    float fl = sqrtf(fx * fx + fy * fy + fz * fz); fx /= fl; fy /= fl; fz /= fl;
    float sx = fy * 0 - fz * 1, sy = fz * 0 - fx * 0, sz = fx * 1 - fy * 0;
    float sl = sqrtf(sx * sx + sy * sy + sz * sz); sx /= sl; sy /= sl; sz /= sl;
    float ux = sy * fz - sz * fy, uy = sz * fx - sx * fz, uz = sx * fy - sy * fx;

    m[0] = sx; m[1] = ux; m[2] = -fx; m[3] = 0;
    m[4] = sy; m[5] = uy; m[6] = -fy; m[7] = 0;
    m[8] = sz; m[9] = uz; m[10] = -fz; m[11] = 0;
    m[12] = -(sx * ex + sy * ey + sz * ez);
    m[13] = -(ux * ex + uy * ey + uz * ez);
    m[14] = (fx * ex + fy * ey + fz * ez);
    m[15] = 1.0f;
}

static void Mat4RotY(float* m, float a) {
    Mat4Identity(m);
    float c = cosf(a), s = sinf(a);
    m[0] = c; m[8] = s; m[2] = -s; m[10] = c;
}

static void Mat4RotX(float* m, float a) {
    Mat4Identity(m);
    float c = cosf(a), s = sinf(a);
    m[5] = c; m[9] = -s; m[6] = s; m[10] = c;
}

static dw::Shader Compile(int type, const char* src) {
    dw::Shader s = dw::CreateShader(type);
    dw::ShaderSource(s, src);
    dw::CompileShader(s);
    if (!dw::GetShaderStatus(s)) MessageBoxA(0, dw::GetShaderInfoLog(s), "Shader error", MB_OK);
    return s;
}

void DemoInit() {
    dw::Shader vs = Compile(dw::VERTEX_SHADER, VS);
    dw::Shader fs = Compile(dw::FRAGMENT_SHADER, FS);
    g_prog = dw::CreateProgram();
    dw::AttachShader(g_prog, vs);
    dw::AttachShader(g_prog, fs);
    dw::LinkProgram(g_prog);
    if (!dw::GetProgramStatus(g_prog)) MessageBoxA(0, "Program link failed", "Error", MB_OK);

    g_posLoc = dw::GetAttribLocation(g_prog, "aPos");
    g_colLoc = dw::GetAttribLocation(g_prog, "aCol");
    g_mvpLoc = dw::GetUniformLocation(g_prog, "uMVP");

    g_vbo = dw::CreateBuffer();
    dw::BindBuffer(dw::ARRAY_BUFFER, g_vbo);
    dw::BufferData(dw::ARRAY_BUFFER, VERTS, sizeof(VERTS), dw::STATIC_DRAW);

    g_ibo = dw::CreateBuffer();
    dw::BindBuffer(dw::ELEMENT_ARRAY_BUFFER, g_ibo);
    dw::BufferData(dw::ELEMENT_ARRAY_BUFFER, INDICES, sizeof(INDICES), dw::STATIC_DRAW);
}

void DemoUpdate(float dt) {
    g_angle += dt * 40.0f * 3.14159265f / 180.0f;
}

void DemoRender() {
    dw::ClearColor(0.06f, 0.07f, 0.10f, 1.0f);
    dw::Clear(dw::COLOR_BUFFER_BIT | dw::DEPTH_BUFFER_BIT);
    dw::Enable(dw::DEPTH_TEST);

    float aspect = (float)platform::Width() / (float)platform::Height();
    float proj[16], view[16], model[16], ry[16], rx[16], tmp[16], mvp[16];
    Mat4Perspective(proj, 45.0f * 3.14159265f / 180.0f, aspect, 0.1f, 100.0f);
    Mat4LookAt(view, 2.6f, 2.2f, 3.4f);
    Mat4RotY(ry, g_angle);
    Mat4RotX(rx, g_angle * 0.5f);
    Mat4Mul(model, ry, rx);
    Mat4Mul(tmp, view, model);
    Mat4Mul(mvp, proj, tmp);

    dw::UseProgram(g_prog);
    dw::BindBuffer(dw::ARRAY_BUFFER, g_vbo);
    dw::EnableVertexAttribArray(g_posLoc);
    dw::VertexAttribPointer(g_posLoc, 3, dw::FLOAT, false, 24, (const void*)0);
    dw::EnableVertexAttribArray(g_colLoc);
    dw::VertexAttribPointer(g_colLoc, 3, dw::FLOAT, false, 24, (const void*)12);
    dw::BindBuffer(dw::ELEMENT_ARRAY_BUFFER, g_ibo);
    dw::UniformMatrix4fv(g_mvpLoc, mvp);
    dw::DrawElements(dw::TRIANGLES, 36, dw::UNSIGNED_SHORT, (const void*)0);
}
