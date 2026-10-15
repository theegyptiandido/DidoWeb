/* DidoWeb for the browser - a small WebGL-style API.
   Same shape as the compiled DidoWeb, but implemented over canvas WebGL. */
(function (global) {
    var dw = {};

    dw.ARRAY_BUFFER = 0x8892;
    dw.ELEMENT_ARRAY_BUFFER = 0x8893;
    dw.STATIC_DRAW = 0x88E4;
    dw.DYNAMIC_DRAW = 0x88E8;
    dw.VERTEX_SHADER = 0x8B31;
    dw.FRAGMENT_SHADER = 0x8B30;
    dw.FLOAT = 0x1406;
    dw.UNSIGNED_SHORT = 0x1403;
    dw.POINTS = 0x0000;
    dw.LINES = 0x0001;
    dw.TRIANGLES = 0x0004;
    dw.DEPTH_TEST = 0x0B71;
    dw.COLOR_BUFFER_BIT = 0x4000;
    dw.DEPTH_BUFFER_BIT = 0x0100;

    var gl = null;

    dw.init = function (canvas) {
        gl = canvas.getContext('webgl') || canvas.getContext('experimental-webgl');
        return !!gl;
    };

    dw.clearColor = function (r, g, b, a) { gl.clearColor(r, g, b, a); };
    dw.clear = function (mask) { gl.clear(mask); };
    dw.enable = function (cap) { gl.enable(cap); };
    dw.disable = function (cap) { gl.disable(cap); };
    dw.depthFunc = function (f) { gl.depthFunc(f); };
    dw.viewport = function (x, y, w, h) { gl.viewport(x, y, w, h); };

    dw.createBuffer = function () { return gl.createBuffer(); };
    dw.bindBuffer = function (t, b) { gl.bindBuffer(t, b); };
    dw.bufferData = function (t, data, usage) { gl.bufferData(t, data, usage); };
    dw.deleteBuffer = function (b) { gl.deleteBuffer(b); };

    dw.createShader = function (type) { return gl.createShader(type); };
    dw.shaderSource = function (s, src) { gl.shaderSource(s, src); };
    dw.compileShader = function (s) { gl.compileShader(s); };
    dw.getShaderStatus = function (s) { return gl.getShaderParameter(s, gl.COMPILE_STATUS); };
    dw.getShaderInfoLog = function (s) { return gl.getShaderInfoLog(s); };

    dw.createProgram = function () { return gl.createProgram(); };
    dw.attachShader = function (p, s) { gl.attachShader(p, s); };
    dw.linkProgram = function (p) { gl.linkProgram(p); };
    dw.getProgramStatus = function (p) { return gl.getProgramParameter(p, gl.LINK_STATUS); };
    dw.useProgram = function (p) { gl.useProgram(p); };
    dw.getAttribLocation = function (p, n) { return gl.getAttribLocation(p, n); };
    dw.getUniformLocation = function (p, n) { return gl.getUniformLocation(p, n); };

    dw.enableVertexAttribArray = function (loc) { gl.enableVertexAttribArray(loc); };
    dw.vertexAttribPointer = function (loc, size, type, norm, stride, off) {
        gl.vertexAttribPointer(loc, size, type, norm, stride, off);
    };
    dw.uniformMatrix4fv = function (loc, m) { gl.uniformMatrix4fv(loc, false, m); };
    dw.uniform1f = function (loc, v) { gl.uniform1f(loc, v); };
    dw.uniform3f = function (loc, x, y, z) { gl.uniform3f(loc, x, y, z); };

    dw.drawArrays = function (mode, first, count) { gl.drawArrays(mode, first, count); };
    dw.drawElements = function (mode, count, type, off) { gl.drawElements(mode, count, type, off); };

    /* The browser presents automatically each animation frame. */
    dw.swapBuffers = function () { };

    global.DidoWeb = dw;
})(window);
