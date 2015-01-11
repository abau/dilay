#include "shader.hpp"

#define SMOOTH_VERTEX_SHADER                                                                   \
  "#version 120                                                                            \n" \
  "                                                                                        \n" \
  "uniform   mat4  mvp;                                                                    \n" \
  "attribute vec3  position;                                                               \n" \
  "attribute vec3  normal;                                                                 \n" \
  "uniform   vec3  ambient;                                                                \n" \
  "uniform   vec3  light1Position;                                                         \n" \
  "uniform   vec3  light1Color;                                                            \n" \
  "uniform   float light1Irradiance;                                                       \n" \
  "uniform   vec3  light2Position;                                                         \n" \
  "uniform   vec3  light2Color;                                                            \n" \
  "uniform   float light2Irradiance;                                                       \n" \
  "                                                                                        \n" \
  "varying vec3 vsOut;                                                                     \n" \
  "                                                                                        \n" \
  "void main () {                                                                          \n" \
  "  gl_Position        = mvp * vec4 (position,1);                                         \n" \
  "  float light1Factor = light1Irradiance * max (0.0, dot (light1Position, normal));      \n" \
  "  float light2Factor = light2Irradiance * max (0.0, dot (light2Position, normal));      \n" \
  "  vec3  light1       = light1Color * vec3 (light1Factor);                               \n" \
  "  vec3  light2       = light2Color * vec3 (light2Factor);                               \n" \
  "        vsOut        = ambient + light1 + light2;                                       \n" \
  "}                                                                                       \n"

#define SMOOTH_FRAGMENT_SHADER(OUT,FINAL)                                                      \
  "#version 120                                                                            \n" \
  "                                                                                        \n" \
  "uniform vec3 color;                                                                     \n" \
  "uniform vec3 wireframeColor;                                                            \n" \
  "                                                                                        \n" \
  "varying vec3 " OUT ";                                                                   \n" \
  "varying vec3 barycentric;                                                               \n" \
  "                                                                                        \n" \
  "void main () {                                                                          \n" \
  "  gl_FragColor = vec4 (color * " OUT ", 1.0);                                           \n" \
     FINAL                                                                                     \
  "}                                                                                       \n"

#define FLAT_VERTEX_SHADER                                                                     \
  "#version 120                                                                            \n" \
  "                                                                                        \n" \
  "uniform   mat4 mvp;                                                                     \n" \
  "uniform   mat4 model;                                                                   \n" \
  "attribute vec3 position;                                                                \n" \
  "attribute vec3 normal;                                                                  \n" \
  "                                                                                        \n" \
  "varying vec3 vsOut;                                                                     \n" \
  "                                                                                        \n" \
  "void main () {                                                                          \n" \
  "  gl_Position = mvp * vec4 (position,1);                                                \n" \
  "  vsOut       = vec3 (model * vec4 (position, 1));                                      \n" \
  "}                                                                                       \n"

#define FLAT_FRAGMENT_SHADER(OUT,FINAL)                                                        \
  "#version 120                                                                            \n" \
  "                                                                                        \n" \
  "uniform vec3  color;                                                                    \n" \
  "uniform vec3  wireframeColor;                                                           \n" \
  "uniform vec3  ambient;                                                                  \n" \
  "uniform vec3  light1Position;                                                           \n" \
  "uniform vec3  light1Color;                                                              \n" \
  "uniform float light1Irradiance;                                                         \n" \
  "uniform vec3  light2Position;                                                           \n" \
  "uniform vec3  light2Color;                                                              \n" \
  "uniform float light2Irradiance;                                                         \n" \
  "                                                                                        \n" \
  "varying vec3 " OUT ";                                                                   \n" \
  "varying vec3 barycentric;                                                               \n" \
  "                                                                                        \n" \
  "void main () {                                                                          \n" \
  "  vec3  normal       = normalize(cross(dFdx(" OUT "),dFdy(" OUT ")));                   \n" \
  "                                                                                        \n" \
  "  float light1Factor = light1Irradiance * max (0.0, dot  (light1Position, normal));     \n" \
  "  float light2Factor = light2Irradiance * max (0.0, dot  (light2Position, normal));     \n" \
  "  vec3  light1       = light1Color * vec3 (light1Factor);                               \n" \
  "  vec3  light2       = light2Color * vec3 (light2Factor);                               \n" \
  "                                                                                        \n" \
  "  vec3  light        = ambient + light1 + light2;                                       \n" \
  "  gl_FragColor       = vec4 (color * light, 1.0);                                       \n" \
     FINAL                                                                                     \
  "}                                                                                       \n"

#define CONSTANT_VERTEX_SHADER                                                                 \
  "#version 120                                                                            \n" \
  "                                                                                        \n" \
  "uniform   mat4 mvp;                                                                     \n" \
  "attribute vec3 position;                                                                \n" \
  "                                                                                        \n" \
  "void main(){                                                                            \n" \
  "  gl_Position = mvp * vec4 (position,1);                                                \n" \
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

const std::string Shader::smoothVertexShader () {
  return SMOOTH_VERTEX_SHADER;
}

const std::string Shader::smoothFragmentShader () {
  return SMOOTH_FRAGMENT_SHADER ("vsOut","");
}

const std::string Shader::smoothWireframeFragmentShader () {
  return SMOOTH_FRAGMENT_SHADER ("gsOut",ADD_WIREFRAME);
}

const std::string Shader::flatVertexShader () {
  return FLAT_VERTEX_SHADER;
}

const std::string Shader::flatFragmentShader () {
  return FLAT_FRAGMENT_SHADER ("vsOut","");
}

const std::string Shader::flatWireframeFragmentShader () {
  return FLAT_FRAGMENT_SHADER ("gsOut",ADD_WIREFRAME);
}

const std::string Shader::constantVertexShader () {
  return CONSTANT_VERTEX_SHADER;
}

const std::string Shader::constantFragmentShader () {
  return CONSTANT_FRAGMENT_SHADER ("");
}

const std::string Shader::constantWireframeFragmentShader () {
  return CONSTANT_FRAGMENT_SHADER (ADD_WIREFRAME);
}

const std::string Shader::geometryShader () {
  return GEOMETRY_SHADER;
}
