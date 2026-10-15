/* DidoWeb shader-cube demo (browser). Mirrors the compiled demo.cpp. */
(function () {
    var VERTS = new Float32Array([
        -1, -1, 1, 1, 0, 0, 1, -1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, -1, 1, 1, 1, 0, 0,
        1, -1, -1, 0, 1, 0, -1, -1, -1, 0, 1, 0, -1, 1, -1, 0, 1, 0, 1, 1, -1, 0, 1, 0,
        -1, -1, -1, 0, 0, 1, -1, -1, 1, 0, 0, 1, -1, 1, 1, 0, 0, 1, -1, 1, -1, 0, 0, 1,
        1, -1, 1, 1, 1, 0, 1, -1, -1, 1, 1, 0, 1, 1, -1, 1, 1, 0, 1, 1, 1, 1, 1, 0,
        -1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, -1, 1, 0, 1, -1, 1, -1, 1, 0, 1,
        -1, -1, -1, 0, 1, 1, 1, -1, -1, 0, 1, 1, 1, -1, 1, 0, 1, 1, -1, -1, 1, 0, 1, 1
    ]);

    var INDICES = new Uint16Array([
        0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11,
        12, 13, 14, 12, 14, 15, 16, 17, 18, 16, 18, 19, 20, 21, 22, 20, 22, 23
    ]);

    var VS =
        "attribute vec3 aPos;\n" +
        "attribute vec3 aCol;\n" +
        "uniform mat4 uMVP;\n" +
        "varying vec3 vCol;\n" +
        "void main() {\n" +
        "    vCol = aCol;\n" +
        "    gl_Position = uMVP * vec4(aPos, 1.0);\n" +
        "}\n";

    var FS =
        "precision mediump float;\n" +
        "varying vec3 vCol;\n" +
        "void main() {\n" +
        "    gl_FragColor = vec4(vCol, 1.0);\n" +
        "}\n";

    var prog, vbo, ibo, posLoc, colLoc, mvpLoc, angle = 0, last = 0;

    function mul(out, a, b) {
        for (var c = 0; c < 4; c++)
            for (var r = 0; r < 4; r++) {
                var s = 0;
                for (var k = 0; k < 4; k++) s += a[k * 4 + r] * b[c * 4 + k];
                out[c * 4 + r] = s;
            }
    }

    function perspective(fovy, aspect, n, f) {
        var m = new Float32Array(16);
        var t = 1.0 / Math.tan(fovy * 0.5);
        m[0] = t / aspect; m[5] = t;
        m[10] = (f + n) / (n - f); m[11] = -1;
        m[14] = (2 * f * n) / (n - f);
        return m;
    }

    function lookAt(ex, ey, ez) {
        var fx = -ex, fy = -ey, fz = -ez;
        var fl = Math.hypot(fx, fy, fz); fx /= fl; fy /= fl; fz /= fl;
        var sx = fy * 0 - fz * 1, sy = fz * 0 - fx * 0, sz = fx * 1 - fy * 0;
        var sl = Math.hypot(sx, sy, sz); sx /= sl; sy /= sl; sz /= sl;
        var ux = sy * fz - sz * fy, uy = sz * fx - sx * fz, uz = sx * fy - sy * fx;
        var m = new Float32Array(16);
        m[0] = sx; m[1] = ux; m[2] = -fx;
        m[4] = sy; m[5] = uy; m[6] = -fy;
        m[8] = sz; m[9] = uz; m[10] = -fz;
        m[12] = -(sx * ex + sy * ey + sz * ez);
        m[13] = -(ux * ex + uy * ey + uz * ez);
        m[14] = (fx * ex + fy * ey + fz * ez);
        m[15] = 1;
        return m;
    }

    function rotY(a) { var m = new Float32Array(16); m[0] = Math.cos(a); m[8] = Math.sin(a); m[2] = -Math.sin(a); m[10] = Math.cos(a); m[5] = 1; m[15] = 1; return m; }
    function rotX(a) { var m = new Float32Array(16); m[5] = Math.cos(a); m[9] = -Math.sin(a); m[6] = Math.sin(a); m[10] = Math.cos(a); m[0] = 1; m[15] = 1; return m; }

    function compile(type, src) {
        var s = DidoWeb.createShader(type);
        DidoWeb.shaderSource(s, src);
        DidoWeb.compileShader(s);
        if (!DidoWeb.getShaderStatus(s)) alert("Shader error: " + DidoWeb.getShaderInfoLog(s));
        return s;
    }

    function init() {
        var canvas = document.getElementById('c');
        if (!DidoWeb.init(canvas)) { alert("No WebGL available"); return; }
        DidoWeb.enable(DidoWeb.DEPTH_TEST);

        prog = DidoWeb.createProgram();
        DidoWeb.attachShader(prog, compile(DidoWeb.VERTEX_SHADER, VS));
        DidoWeb.attachShader(prog, compile(DidoWeb.FRAGMENT_SHADER, FS));
        DidoWeb.linkProgram(prog);
        if (!DidoWeb.getProgramStatus(prog)) alert("Program link failed");

        posLoc = DidoWeb.getAttribLocation(prog, "aPos");
        colLoc = DidoWeb.getAttribLocation(prog, "aCol");
        mvpLoc = DidoWeb.getUniformLocation(prog, "uMVP");

        vbo = DidoWeb.createBuffer();
        DidoWeb.bindBuffer(DidoWeb.ARRAY_BUFFER, vbo);
        DidoWeb.bufferData(DidoWeb.ARRAY_BUFFER, VERTS, DidoWeb.STATIC_DRAW);

        ibo = DidoWeb.createBuffer();
        DidoWeb.bindBuffer(DidoWeb.ELEMENT_ARRAY_BUFFER, ibo);
        DidoWeb.bufferData(DidoWeb.ELEMENT_ARRAY_BUFFER, INDICES, DidoWeb.STATIC_DRAW);

        requestAnimationFrame(frame);
    }

    function frame(t) {
        if (last) angle += (t - last) * 0.001 * 0.7;
        last = t;

        var canvas = document.getElementById('c');
        DidoWeb.viewport(0, 0, canvas.width, canvas.height);
        DidoWeb.clearColor(0.06, 0.07, 0.10, 1);
        DidoWeb.clear(DidoWeb.COLOR_BUFFER_BIT | DidoWeb.DEPTH_BUFFER_BIT);

        var aspect = canvas.width / canvas.height;
        var proj = perspective(45 * Math.PI / 180, aspect, 0.1, 100);
        var view = lookAt(2.6, 2.2, 3.4);
        var model = new Float32Array(16);
        mul(model, rotY(angle), rotX(angle * 0.5));
        var tmp = new Float32Array(16); mul(tmp, view, model);
        var mvp = new Float32Array(16); mul(mvp, proj, tmp);

        DidoWeb.useProgram(prog);
        DidoWeb.bindBuffer(DidoWeb.ARRAY_BUFFER, vbo);
        DidoWeb.enableVertexAttribArray(posLoc);
        DidoWeb.vertexAttribPointer(posLoc, 3, DidoWeb.FLOAT, false, 24, 0);
        DidoWeb.enableVertexAttribArray(colLoc);
        DidoWeb.vertexAttribPointer(colLoc, 3, DidoWeb.FLOAT, false, 24, 12);
        DidoWeb.bindBuffer(DidoWeb.ELEMENT_ARRAY_BUFFER, ibo);
        DidoWeb.uniformMatrix4fv(mvpLoc, mvp);
        DidoWeb.drawElements(DidoWeb.TRIANGLES, 36, DidoWeb.UNSIGNED_SHORT, 0);

        requestAnimationFrame(frame);
    }

    window.addEventListener('load', init);
})();
