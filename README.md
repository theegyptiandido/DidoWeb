# DidoWeb

**A mini WebGL** — a small, WebGL-style graphics API (buffers, shaders, programs, attributes, uniforms, draw calls), hand-written as a layer on top of [DidoGL](https://github.com/theegyptiandido/DidoGL).

## Two forms

**Compiled (C++)** — renders a shader cube to a desktop window via DidoGL → OpenGL.
- `src/didoweb.h` / `src/didoweb.cpp` — the API (namespace `dw`)
- `src/didogl_c.*` — the DidoGL C backend it builds on
- `src/demo.cpp` — GLSL shader cube demo
- `build.bat` → `build\DidoWeb.exe`

**Browser** — the same API in JavaScript over a `<canvas>` WebGL context.
- `web/didoweb.js` — the DidoWeb API (backend = WebGL)
- `web/demo.js` — the shader cube demo (calls only `DidoWeb.*`)
- `web/index.html` — open directly, or serve with `python -m http.server`

## API

```
init, clearColor, clear, enable, disable, depthFunc, viewport
createBuffer, bindBuffer, bufferData, deleteBuffer
createShader, shaderSource, compileShader, getShaderStatus, getShaderInfoLog
createProgram, attachShader, linkProgram, getProgramStatus, useProgram
getAttribLocation, getUniformLocation
enableVertexAttribArray, vertexAttribPointer
uniformMatrix4fv, uniform1f, uniform3f
drawArrays, drawElements, swapBuffers
```

## Note

DidoWeb never talks to a GPU directly. On desktop it sits on **DidoGL → OpenGL**; in the browser it sits on **WebGL**. The demo/game code only ever calls `DidoWeb`.
