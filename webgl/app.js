const canvas = document.getElementById("glCanvas");
const fileInput = document.getElementById("fileInput");
const loadSampleBtn = document.getElementById("loadSample");
const sampleSelect = document.getElementById("sampleSelect");
const eyeDistanceInput = document.getElementById("eyeDistance");
const viewDistanceInput = document.getElementById("viewDistance");
const eyeValue = document.getElementById("eyeValue");
const viewValue = document.getElementById("viewValue");
const redColorInput = document.getElementById("redColor");
const cyanColorInput = document.getElementById("cyanColor");
const originAxesInput = document.getElementById("originAxes");
const rotationAxesInput = document.getElementById("rotationAxes");
const resetViewBtn = document.getElementById("resetView");
const zoomInBtn = document.getElementById("zoomIn");
const zoomOutBtn = document.getElementById("zoomOut");

const gl = canvas.getContext("webgl", { antialias: true, preserveDrawingBuffer: true });
if (!gl) {
  alert("WebGL is not supported in this browser.");
}

document.body.addEventListener("dragover", (event) => event.preventDefault());
document.body.addEventListener("drop", (event) => {
  event.preventDefault();
  if (event.dataTransfer.files.length > 0) {
    handleFile(event.dataTransfer.files[0]);
  }
});

const state = {
  model: null,
  rotation: { x: 0, y: 0 },
  eyeDistance: parseFloat(eyeDistanceInput.value),
  viewDistance: parseFloat(viewDistanceInput.value),
  redColor: hexToRgb(redColorInput.value),
  cyanColor: hexToRgb(cyanColorInput.value),
  showOriginAxes: false,
  showRotationAxes: false,
  dragging: false,
  lastPos: null,
};

const vertexShaderSource = `
attribute vec3 aPosition;

uniform mat3 uRotation;
uniform vec3 uCenter;
uniform vec2 uWorldMin;
uniform vec2 uWorldMax;
uniform float uViewDistance;
uniform float uEyeOffset;
uniform float uPointSize;

void main() {
  vec3 centered = aPosition - uCenter;
  vec3 rotated = uRotation * centered;

  float denom = uViewDistance - rotated.z;
  if (abs(denom) < 0.0001) {
    denom = (denom < 0.0) ? -0.0001 : 0.0001;
  }

  float x = (uViewDistance * rotated.x + uEyeOffset * rotated.z) / denom;
  float y = (uViewDistance * rotated.y) / denom;

  float xRange = uWorldMax.x - uWorldMin.x;
  float yRange = uWorldMax.y - uWorldMin.y;
  if (abs(xRange) < 0.0001) {
    xRange = (xRange < 0.0) ? -0.0001 : 0.0001;
  }
  if (abs(yRange) < 0.0001) {
    yRange = (yRange < 0.0) ? -0.0001 : 0.0001;
  }

  float ndcX = ((x - uWorldMin.x) / xRange) * 2.0 - 1.0;
  float ndcY = ((y - uWorldMin.y) / yRange) * 2.0 - 1.0;

  gl_Position = vec4(ndcX, ndcY, 0.0, 1.0);
  gl_PointSize = uPointSize;
}
`;

const fragmentShaderSource = `
precision mediump float;

uniform vec4 uColor;

void main() {
  gl_FragColor = uColor;
}
`;

const overlayVertexShader = `
attribute vec2 aPosition;
void main() {
  gl_Position = vec4(aPosition, 0.0, 1.0);
}
`;

const overlayFragmentShader = `
precision mediump float;
uniform vec4 uColor;
void main() {
  gl_FragColor = uColor;
}
`;

const program = createProgram(gl, vertexShaderSource, fragmentShaderSource);
const overlayProgram = createProgram(gl, overlayVertexShader, overlayFragmentShader);

const attribs = {
  position: gl.getAttribLocation(program, "aPosition"),
};

const uniforms = {
  rotation: gl.getUniformLocation(program, "uRotation"),
  center: gl.getUniformLocation(program, "uCenter"),
  worldMin: gl.getUniformLocation(program, "uWorldMin"),
  worldMax: gl.getUniformLocation(program, "uWorldMax"),
  viewDistance: gl.getUniformLocation(program, "uViewDistance"),
  eyeOffset: gl.getUniformLocation(program, "uEyeOffset"),
  color: gl.getUniformLocation(program, "uColor"),
  pointSize: gl.getUniformLocation(program, "uPointSize"),
};

const overlayAttribs = {
  position: gl.getAttribLocation(overlayProgram, "aPosition"),
};
const overlayUniforms = {
  color: gl.getUniformLocation(overlayProgram, "uColor"),
};

const buffers = {
  vertex: gl.createBuffer(),
  lineIndex: gl.createBuffer(),
  pointIndex: gl.createBuffer(),
  axisVertex: gl.createBuffer(),
};

let lineCount = 0;
let pointCount = 0;

function resizeCanvas() {
  const { clientWidth, clientHeight } = canvas;
  const ratio = window.devicePixelRatio || 1;
  canvas.width = Math.floor(clientWidth * ratio);
  canvas.height = Math.floor(clientHeight * ratio);
  gl.viewport(0, 0, canvas.width, canvas.height);
}

window.addEventListener("resize", () => {
  resizeCanvas();
  draw();
});
resizeCanvas();

canvas.addEventListener("mousedown", (event) => {
  state.dragging = true;
  state.lastPos = { x: event.clientX, y: event.clientY };
});

canvas.addEventListener("mousemove", (event) => {
  if (!state.dragging) return;
  const dx = state.lastPos.x - event.clientX;
  const dy = state.lastPos.y - event.clientY;
  state.lastPos = { x: event.clientX, y: event.clientY };
  const width = canvas.clientWidth || 1;
  const height = canvas.clientHeight || 1;
  state.rotation.y += dx * (360 / height);
  state.rotation.x += dy * (360 / width);
  draw();
});

canvas.addEventListener("mouseup", () => {
  state.dragging = false;
});

canvas.addEventListener("mouseleave", () => {
  state.dragging = false;
});

canvas.addEventListener("wheel", (event) => {
  event.preventDefault();
  if (!state.model) return;
  if (event.deltaY < 0) zoom(0.25);
  if (event.deltaY > 0) zoom(-0.25);
  draw();
});

fileInput.addEventListener("change", (event) => {
  const file = event.target.files[0];
  if (file) {
    handleFile(file);
  }
});

loadSampleBtn.addEventListener("click", async () => {
  const selected = sampleSelect.value || "cube.obj";
  const response = await fetch(`samples/${selected}`);
  const text = await response.text();
  const model = parseOBJ(text);
  if (model) applyModel(model);
});

eyeDistanceInput.addEventListener("input", () => {
  state.eyeDistance = parseFloat(eyeDistanceInput.value);
  eyeValue.textContent = state.eyeDistance.toFixed(1);
  draw();
});

viewDistanceInput.addEventListener("input", () => {
  state.viewDistance = parseFloat(viewDistanceInput.value);
  viewValue.textContent = state.viewDistance.toFixed(0);
  draw();
});

redColorInput.addEventListener("input", () => {
  state.redColor = hexToRgb(redColorInput.value);
  draw();
});

cyanColorInput.addEventListener("input", () => {
  state.cyanColor = hexToRgb(cyanColorInput.value);
  draw();
});

originAxesInput.addEventListener("change", () => {
  state.showOriginAxes = originAxesInput.checked;
  draw();
});

rotationAxesInput.addEventListener("change", () => {
  state.showRotationAxes = rotationAxesInput.checked;
  draw();
});

resetViewBtn.addEventListener("click", () => {
  if (!state.model) return;
  state.rotation.x = 0;
  state.rotation.y = 0;
  state.model.world = computeWorldBounds(state.model.bounds);
  draw();
});

zoomInBtn.addEventListener("click", () => {
  zoom(0.25);
  draw();
});

zoomOutBtn.addEventListener("click", () => {
  zoom(-0.25);
  draw();
});

function zoom(scaleDelta) {
  if (!state.model) return;
  const world = state.model.world;
  const rangeX = world.xmax - world.xmin;
  const rangeY = world.ymax - world.ymin;
  const deltaX = rangeX * scaleDelta;
  const deltaY = rangeY * scaleDelta;
  world.xmin += deltaX;
  world.xmax -= deltaX;
  world.ymin += deltaY;
  world.ymax -= deltaY;
}

function handleFile(file) {
  const reader = new FileReader();
  reader.onload = () => {
    const text = reader.result;
    const ext = file.name.split(".").pop().toLowerCase();
    let model = null;
    if (ext === "obj") model = parseOBJ(text);
    if (ext === "pdb") model = parsePDB(text);
    if (ext === "csv") model = parseCSV(text);
    if (model) {
      applyModel(model);
    }
  };
  reader.readAsText(file);
}

function applyModel(model) {
  state.model = model;
  state.rotation.x = 0;
  state.rotation.y = 0;
  uploadModel(model);
  updateHint();
  draw();
}

function uploadModel(model) {
  gl.bindBuffer(gl.ARRAY_BUFFER, buffers.vertex);
  gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(model.vertices.flat()), gl.STATIC_DRAW);

  gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, buffers.lineIndex);
  gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(model.lines.flat()), gl.STATIC_DRAW);
  lineCount = model.lines.length * 2;

  gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, buffers.pointIndex);
  gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(model.pointIndices), gl.STATIC_DRAW);
  pointCount = model.pointIndices.length;
}

function draw() {
  resizeCanvas();
  gl.clearColor(0, 0, 0, 1);
  gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

  if (!state.model) {
    return;
  }

  gl.useProgram(program);
  gl.bindBuffer(gl.ARRAY_BUFFER, buffers.vertex);
  gl.enableVertexAttribArray(attribs.position);
  gl.vertexAttribPointer(attribs.position, 3, gl.FLOAT, false, 0, 0);

  const rotationMatrix = makeRotationMatrix(state.rotation.x, state.rotation.y);
  gl.uniformMatrix3fv(uniforms.rotation, false, rotationMatrix);
  gl.uniform3f(
    uniforms.center,
    state.model.center.x,
    state.model.center.y,
    state.model.center.z
  );
  gl.uniform2f(uniforms.worldMin, state.model.world.xmin, state.model.world.ymin);
  gl.uniform2f(uniforms.worldMax, state.model.world.xmax, state.model.world.ymax);
  gl.uniform1f(uniforms.viewDistance, state.viewDistance);
  gl.uniform1f(uniforms.pointSize, 3.0);

  drawAnaglyphPass(-state.eyeDistance / 2, state.redColor, pointCount > 0);
  drawAnaglyphPass(state.eyeDistance / 2, state.cyanColor, pointCount > 0);

  if (state.showOriginAxes) {
    drawOriginAxes();
  }

  if (state.showRotationAxes) {
    drawRotationAxes();
  }
}

function drawAnaglyphPass(eyeOffset, color, drawPoints) {
  gl.uniform1f(uniforms.eyeOffset, eyeOffset);
  gl.uniform4f(uniforms.color, color.r, color.g, color.b, 1.0);

  gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, buffers.lineIndex);
  gl.drawElements(gl.LINES, lineCount, gl.UNSIGNED_SHORT, 0);

  if (drawPoints) {
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, buffers.pointIndex);
    gl.drawElements(gl.POINTS, pointCount, gl.UNSIGNED_SHORT, 0);
  }
}

function drawOriginAxes() {
  const axes = state.model.originAxes;
  if (!axes || axes.length < 6) return;

  const axisVertices = new Float32Array(axes.flat());
  gl.bindBuffer(gl.ARRAY_BUFFER, buffers.axisVertex);
  gl.bufferData(gl.ARRAY_BUFFER, axisVertices, gl.STATIC_DRAW);

  gl.bindBuffer(gl.ARRAY_BUFFER, buffers.axisVertex);
  gl.vertexAttribPointer(attribs.position, 3, gl.FLOAT, false, 0, 0);
  gl.enableVertexAttribArray(attribs.position);

  const axisLines = new Uint16Array([0, 1, 2, 3, 4, 5]);
  gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, buffers.lineIndex);
  gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, axisLines, gl.STATIC_DRAW);
  lineCount = axisLines.length;

  drawAnaglyphPass(-state.eyeDistance / 2, state.redColor, false);
  drawAnaglyphPass(state.eyeDistance / 2, state.cyanColor, false);

  uploadModel(state.model);
}

function drawRotationAxes() {
  gl.useProgram(overlayProgram);
  const overlayVertices = new Float32Array([
    0.0, -1.0,
    0.0, 1.0,
    -1.0, 0.0,
    1.0, 0.0,
  ]);

  gl.bindBuffer(gl.ARRAY_BUFFER, buffers.axisVertex);
  gl.bufferData(gl.ARRAY_BUFFER, overlayVertices, gl.STATIC_DRAW);
  gl.enableVertexAttribArray(overlayAttribs.position);
  gl.vertexAttribPointer(overlayAttribs.position, 2, gl.FLOAT, false, 0, 0);
  gl.uniform4f(overlayUniforms.color, 1, 1, 1, 0.6);
  gl.drawArrays(gl.LINES, 0, 4);
  gl.useProgram(program);
}

function parseOBJ(text) {
  const vertices = [];
  const lines = [];
  const pointIndices = [];

  text.split(/\r?\n/).forEach((raw) => {
    const line = raw.trim();
    if (!line || line.startsWith("#")) return;
    if (line.startsWith("v ")) {
      const parts = line.split(/\s+/).slice(1);
      if (parts.length >= 3) {
        vertices.push([parseFloat(parts[0]), parseFloat(parts[1]), parseFloat(parts[2])]);
      }
    } else if (line.startsWith("f ") || line.startsWith("fo ")) {
      const parts = line.split(/\s+/).slice(1);
      const indices = parts
        .map((part) => part.split("/")[0])
        .map((val) => parseInt(val, 10) - 1)
        .filter((val) => !Number.isNaN(val));
      if (indices.length >= 2) {
        for (let i = 1; i < indices.length; i++) {
          lines.push([indices[i - 1], indices[i]]);
        }
        lines.push([indices[indices.length - 1], indices[0]]);
      }
    } else if (line.startsWith("l ")) {
      const parts = line.split(/\s+/).slice(1);
      if (parts.length >= 2) {
        lines.push([parseInt(parts[0], 10) - 1, parseInt(parts[1], 10) - 1]);
      }
    } else if (line.startsWith("t ")) {
      const parts = line.split(/\s+/, 3);
      if (parts.length >= 3) {
        const idx = parseInt(parts[1], 10) - 1;
        if (!Number.isNaN(idx)) {
          // labels not rendered in WebGL yet
        }
      }
    }
  });

  return buildModel(vertices, lines, pointIndices);
}

function parsePDB(text) {
  const vertices = [];
  const lines = [];
  const pointIndices = [];

  text.split(/\r?\n/).forEach((line) => {
    if (line.startsWith("ATOM")) {
      const x = parseFloat(line.slice(30, 38));
      const y = parseFloat(line.slice(38, 46));
      const z = parseFloat(line.slice(46, 54));
      if (!Number.isNaN(x) && !Number.isNaN(y) && !Number.isNaN(z)) {
        vertices.push([x, y, z]);
        pointIndices.push(vertices.length - 1);
      }
    } else if (line.startsWith("CONECT")) {
      const parts = line.trim().split(/\s+/);
      const start = parseInt(parts[1], 10) - 1;
      for (let i = 2; i < parts.length; i++) {
        const end = parseInt(parts[i], 10) - 1;
        if (!Number.isNaN(start) && !Number.isNaN(end)) {
          lines.push([start, end]);
        }
      }
    }
  });

  return buildModel(vertices, lines, pointIndices);
}

function parseCSV(text) {
  const lines = [];
  const vertices = [];
  const pointIndices = [];
  const rows = text.split(/\r?\n/).filter((row) => row.includes(","));
  if (rows.length === 0) return null;

  rows.slice(1).forEach((row) => {
    const parts = row.split(",");
    if (parts.length < 3) return;
    const x = parseFloat(parts[0]);
    const y = parseFloat(parts[1]);
    const z = parseFloat(parts[2]);
    if (!Number.isNaN(x) && !Number.isNaN(y) && !Number.isNaN(z)) {
      vertices.push([x, y, z]);
      const idx = vertices.length - 1;
      pointIndices.push(idx);
    }
  });

  const model = buildModel(vertices, lines, pointIndices);
  if (!model) return null;
  const { xmin, xmax, ymin, ymax, zmin, zmax } = model.bounds;

  const xMinIdx = vertices.push([xmin, 0, 0]) - 1;
  const xMaxIdx = vertices.push([xmax, 0, 0]) - 1;
  lines.push([xMinIdx, xMaxIdx]);

  const yMinIdx = vertices.push([0, ymin, 0]) - 1;
  const yMaxIdx = vertices.push([0, ymax, 0]) - 1;
  lines.push([yMinIdx, yMaxIdx]);

  const zMinIdx = vertices.push([0, 0, zmin]) - 1;
  const zMaxIdx = vertices.push([0, 0, zmax]) - 1;
  lines.push([zMinIdx, zMaxIdx]);

  return buildModel(vertices, lines, pointIndices);
}

function buildModel(vertices, lines, pointIndices) {
  if (!vertices.length) return null;
  const bounds = computeBounds(vertices);
  const world = computeWorldBounds(bounds);
  const center = {
    x: (bounds.xmax + bounds.xmin) / 2,
    y: (bounds.ymax + bounds.ymin) / 2,
    z: (bounds.zmax + bounds.zmin) / 2,
  };

  const originAxes = buildOriginAxes(bounds);

  return {
    vertices,
    lines,
    pointIndices,
    bounds,
    world,
    center,
    originAxes,
  };
}

function computeBounds(vertices) {
  const [first] = vertices;
  let xmin = first[0];
  let xmax = first[0];
  let ymin = first[1];
  let ymax = first[1];
  let zmin = first[2];
  let zmax = first[2];

  vertices.forEach(([x, y, z]) => {
    xmin = Math.min(xmin, x);
    xmax = Math.max(xmax, x);
    ymin = Math.min(ymin, y);
    ymax = Math.max(ymax, y);
    zmin = Math.min(zmin, z);
    zmax = Math.max(zmax, z);
  });

  return { xmin, xmax, ymin, ymax, zmin, zmax };
}

function computeWorldBounds(bounds) {
  const xrange = bounds.xmax - bounds.xmin;
  const yrange = bounds.ymax - bounds.ymin;
  const zrange = bounds.zmax - bounds.zmin;
  let maxrange = Math.max(xrange, Math.max(yrange, zrange));
  maxrange *= 1.5;
  maxrange /= 2;
  const xmid = (bounds.xmax + bounds.xmin) / 2;
  const ymid = (bounds.ymax + bounds.ymin) / 2;
  return {
    xmin: xmid - maxrange,
    xmax: xmid + maxrange,
    ymin: ymid - maxrange,
    ymax: ymid + maxrange,
  };
}

function buildOriginAxes(bounds) {
  const xAbs = Math.max(Math.abs(bounds.xmin), Math.abs(bounds.xmax));
  const yAbs = Math.max(Math.abs(bounds.ymin), Math.abs(bounds.ymax));
  const zAbs = Math.max(Math.abs(bounds.zmin), Math.abs(bounds.zmax));

  return [
    [-xAbs * 2, 0, 0],
    [xAbs * 2, 0, 0],
    [0, -yAbs * 2, 0],
    [0, yAbs * 2, 0],
    [0, 0, -zAbs * 2],
    [0, 0, zAbs * 2],
  ];
}

function makeRotationMatrix(xDeg, yDeg) {
  const x = (xDeg * Math.PI) / 180;
  const y = (yDeg * Math.PI) / 180;
  const cx = Math.cos(x);
  const sx = Math.sin(x);
  const cy = Math.cos(y);
  const sy = Math.sin(y);

  const rotY = [
    cy, 0, sy,
    0, 1, 0,
    -sy, 0, cy,
  ];

  const rotX = [
    1, 0, 0,
    0, cx, -sx,
    0, sx, cx,
  ];

  return multiplyMat3(rotY, rotX);
}

function multiplyMat3(a, b) {
  return [
    a[0] * b[0] + a[1] * b[3] + a[2] * b[6],
    a[0] * b[1] + a[1] * b[4] + a[2] * b[7],
    a[0] * b[2] + a[1] * b[5] + a[2] * b[8],

    a[3] * b[0] + a[4] * b[3] + a[5] * b[6],
    a[3] * b[1] + a[4] * b[4] + a[5] * b[7],
    a[3] * b[2] + a[4] * b[5] + a[5] * b[8],

    a[6] * b[0] + a[7] * b[3] + a[8] * b[6],
    a[6] * b[1] + a[7] * b[4] + a[8] * b[7],
    a[6] * b[2] + a[7] * b[5] + a[8] * b[8],
  ];
}

function hexToRgb(hex) {
  const value = hex.replace("#", "");
  const intVal = parseInt(value, 16);
  const r = ((intVal >> 16) & 255) / 255;
  const g = ((intVal >> 8) & 255) / 255;
  const b = (intVal & 255) / 255;
  return { r, g, b };
}

function createShader(glContext, type, source) {
  const shader = glContext.createShader(type);
  glContext.shaderSource(shader, source);
  glContext.compileShader(shader);
  if (!glContext.getShaderParameter(shader, glContext.COMPILE_STATUS)) {
    const info = glContext.getShaderInfoLog(shader);
    glContext.deleteShader(shader);
    throw new Error(info);
  }
  return shader;
}

function createProgram(glContext, vertexSource, fragmentSource) {
  const vs = createShader(glContext, glContext.VERTEX_SHADER, vertexSource);
  const fs = createShader(glContext, glContext.FRAGMENT_SHADER, fragmentSource);
  const program = glContext.createProgram();
  glContext.attachShader(program, vs);
  glContext.attachShader(program, fs);
  glContext.linkProgram(program);
  if (!glContext.getProgramParameter(program, glContext.LINK_STATUS)) {
    const info = glContext.getProgramInfoLog(program);
    glContext.deleteProgram(program);
    throw new Error(info);
  }
  return program;
}

const hint = document.createElement("div");
hint.textContent = "Drop a file to view it.";
hint.style.position = "absolute";
hint.style.top = "50%";
hint.style.left = "50%";
hint.style.transform = "translate(-50%, -50%)";
hint.style.color = "#808b9a";
hint.style.fontSize = "14px";
canvas.parentElement.appendChild(hint);

function updateHint() {
  hint.style.display = state.model ? "none" : "block";
}

updateHint();

loadSamplesList();

async function loadSamplesList() {
  const fallback = ["cube.obj"];
  try {
    const response = await fetch("samples/manifest.json");
    if (!response.ok) {
      setSampleOptions(fallback);
      return;
    }
    const data = await response.json();
    if (!Array.isArray(data.samples)) {
      setSampleOptions(fallback);
      return;
    }
    setSampleOptions(data.samples.filter((name) => name.endsWith(".obj")));
  } catch (error) {
    setSampleOptions(fallback);
  }
}

function setSampleOptions(samples) {
  sampleSelect.innerHTML = "";
  samples.forEach((name) => {
    const option = document.createElement("option");
    option.value = name;
    option.textContent = name;
    sampleSelect.appendChild(option);
  });
}
