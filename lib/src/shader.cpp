#include "shader.hpp"

#define SMOOTH_VERTEX_SHADER                                                                   \
  "#version 120                                                                            \n" \
  "                                                                                        \n" \
  "uniform   mat4  model;                                                                  \n" \
  "uniform   mat4  view;                                                                   \n" \
  "uniform   mat4  projection;                                                             \n" \
  "attribute vec3  position;                                                               \n" \
  "attribute vec3  normal;                                                                 \n" \
  "uniform   vec3  color;                                                                  \n" \
  "uniform   vec3  light1Direction;                                                        \n" \
  "uniform   vec3  light1Color;                                                            \n" \
  "uniform   float light1Irradiance;                                                       \n" \
  "uniform   vec3  light2Direction;                                                        \n" \
  "uniform   vec3  light2Color;                                                            \n" \
  "uniform   float light2Irradiance;                                                       \n" \
  "                                                                                        \n" \
  "varying vec3 vsOut;                                                                     \n" \
  "                                                                                        \n" \
  "void main () {                                                                          \n" \
  "  gl_Position      = (projection * view * model) * vec4 (position,1.0);                 \n" \
  "  vec3  viewNormal = vec3 (view * model * vec4 (normal,0.0));                           \n" \
  "  float light1Diff = max (0.0, dot (light1Direction, viewNormal));                      \n" \
  "  float light2Diff = max (0.0, dot (light2Direction, viewNormal));                      \n" \
  "  vec3  light1     = light1Irradiance * light1Color * light1Diff;                       \n" \
  "  vec3  light2     = light2Irradiance * light2Color * light2Diff;                       \n" \
  "        vsOut      = color * (light1 + light2);                                         \n" \
  "}                                                                                       \n"

#define SMOOTH_FRAGMENT_SHADER(OUT,FINAL)                                                      \
  "#version 120                                                                            \n" \
  "                                                                                        \n" \
  "uniform vec3 wireframeColor;                                                            \n" \
  "                                                                                        \n" \
  "varying vec3 " OUT ";                                                                   \n" \
  "varying vec3 barycentric;                                                               \n" \
  "                                                                                        \n" \
  "void main () {                                                                          \n" \
  "  gl_FragColor = vec4 (" OUT ", 1.0);                                                   \n" \
     FINAL                                                                                     \
  "}                                                                                       \n"

#define FLAT_VERTEX_SHADER                                                                     \
  "#version 120                                                                            \n" \
  "                                                                                        \n" \
  "uniform   mat4 model;                                                                   \n" \
  "uniform   mat4 view;                                                                    \n" \
  "uniform   mat4 projection;                                                              \n" \
  "attribute vec3 position;                                                                \n" \
  "                                                                                        \n" \
  "varying vec3 vsOut;                                                                     \n" \
  "                                                                                        \n" \
  "void main () {                                                                          \n" \
  "  gl_Position = (projection * view * model) * vec4 (position,1.0);                      \n" \
  "  vsOut       = vec3 (model * vec4 (position, 1.0));                                    \n" \
  "}                                                                                       \n"

#define FLAT_FRAGMENT_SHADER(OUT,FINAL)                                                        \
  "#version 120                                                                            \n" \
  "                                                                                        \n" \
  "uniform mat4  view;                                                                     \n" \
  "uniform vec3  color;                                                                    \n" \
  "uniform vec3  wireframeColor;                                                           \n" \
  "uniform vec3  light1Direction;                                                          \n" \
  "uniform vec3  light1Color;                                                              \n" \
  "uniform float light1Irradiance;                                                         \n" \
  "uniform vec3  light2Direction;                                                          \n" \
  "uniform vec3  light2Color;                                                              \n" \
  "uniform float light2Irradiance;                                                         \n" \
  "                                                                                        \n" \
  "varying vec3 " OUT ";                                                                   \n" \
  "varying vec3 barycentric;                                                               \n" \
  "                                                                                        \n" \
  "void main () {                                                                          \n" \
  "  vec3  normal     = normalize(cross(dFdx(" OUT "),dFdy(" OUT ")));                     \n" \
  "  vec3  viewNormal = vec3 (view * vec4 (normal,0.0));                                   \n" \
  "                                                                                        \n" \
  "  float light1Diff = max (0.0, dot (light1Direction, viewNormal));                      \n" \
  "  float light2Diff = max (0.0, dot (light2Direction, viewNormal));                      \n" \
  "  vec3  light1     = light1Irradiance * light1Color * vec3 (light1Diff);                \n" \
  "  vec3  light2     = light2Irradiance * light2Color * vec3 (light2Diff);                \n" \
  "                                                                                        \n" \
  "  gl_FragColor     = vec4 (color * (light1 + light2), 1.0);                             \n" \
     FINAL                                                                                     \
  "}                                                                                       \n"

#define CONSTANT_VERTEX_SHADER                                                                 \
  "#version 120                                                                            \n" \
  "                                                                                        \n" \
  "uniform   mat4 model;                                                                   \n" \
  "uniform   mat4 view;                                                                    \n" \
  "uniform   mat4 projection;                                                              \n" \
  "attribute vec3 position;                                                                \n" \
  "                                                                                        \n" \
  "void main(){                                                                            \n" \
  "  gl_Position = (projection * view * model) * vec4 (position,1.0);                      \n" \
  "}                                                                                       \n"

#define CONSTANT_FRAGMENT_SHADER(FINAL)                                                        \
  "#version 120                                                                            \n" \
  "                                                                                        \n" \
  "uniform vec3 color;                                                                     \n" \
  "uniform vec3 wireframeColor;                                                            \n" \
  "                                                                                        \n" \
  "varying vec3 barycentric;                                                               \n" \
  "                                                                                        \n" \
  "void main(){                                                                            \n" \
  "  gl_FragColor = vec4 (color, 1.0);                                                     \n" \
     FINAL                                                                                     \
  "}                                                                                       \n"

#define ADD_WIREFRAME                                                                          \
  "vec3 barycDelta  = fwidth(barycentric);                                                 \n" \
  "vec3 barycFactor = smoothstep(vec3(0.0), barycDelta*1, barycentric);                    \n" \
  "float minF = min(min(barycFactor.x,barycFactor.y),barycFactor.z);                       \n" \
  "gl_FragColor.rgb = mix (wireframeColor,gl_FragColor.rgb,minF);                          \n"

#define GEOMETRY_SHADER                                                                        \
  "#extension GL_EXT_geometry_shader4: require                                             \n" \
  "                                                                                        \n" \
  "varying in  vec3 vsOut[3];                                                              \n" \
  "varying out vec3 gsOut;                                                                 \n" \
  "varying out vec3 barycentric;                                                           \n" \
  "                                                                                        \n" \
  "void main() {                                                                           \n" \
  "    gl_Position = gl_PositionIn[0];                                                     \n" \
  "    gsOut       = vsOut[0];                                                             \n" \
  "    barycentric = vec3 (1.0,0.0,0.0);                                                   \n" \
  "    EmitVertex();                                                                       \n" \
  "                                                                                        \n" \
  "    gl_Position = gl_PositionIn[1];                                                     \n" \
  "    gsOut       = vsOut[1];                                                             \n" \
  "    barycentric = vec3 (0.0,1.0,0.0);                                                   \n" \
  "    EmitVertex();                                                                       \n" \
  "                                                                                        \n" \
  "    gl_Position = gl_PositionIn[2];                                                     \n" \
  "    gsOut       = vsOut[2];                                                             \n" \
  "    barycentric = vec3 (0.0,0.0,1.0);                                                   \n" \
  "    EmitVertex();                                                                       \n" \
  "                                                                                        \n" \
  "    EndPrimitive();                                                                     \n" \
  "}                                                                                       \n"

const char* Shader::smoothVertexShader () {
  return SMOOTH_VERTEX_SHADER;
}

const char* Shader::smoothFragmentShader () {
  return SMOOTH_FRAGMENT_SHADER ("vsOut","");
}

const char* Shader::smoothWireframeFragmentShader () {
  return SMOOTH_FRAGMENT_SHADER ("gsOut",ADD_WIREFRAME);
}

const char* Shader::flatVertexShader () {
  return FLAT_VERTEX_SHADER;
}

const char* Shader::flatFragmentShader () {
  return FLAT_FRAGMENT_SHADER ("vsOut","");
}

const char* Shader::flatWireframeFragmentShader () {
  return FLAT_FRAGMENT_SHADER ("gsOut",ADD_WIREFRAME);
}

const char* Shader::constantVertexShader () {
  return CONSTANT_VERTEX_SHADER;
}

const char* Shader::constantFragmentShader () {
  return CONSTANT_FRAGMENT_SHADER ("");
}

const char* Shader::constantWireframeFragmentShader () {
  return CONSTANT_FRAGMENT_SHADER (ADD_WIREFRAME);
}

const char* Shader::geometryShader () {
  return GEOMETRY_SHADER;
}
