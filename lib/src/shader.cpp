#include "shader.hpp"

const std::string Shader::smoothVertexShader () {
  return
    "#version 120                                                                            \n"
    "                                                                                        \n"
    "uniform   mat4  mvp;                                                                    \n"
    "attribute vec3  position;                                                               \n"
    "attribute vec3  normal;                                                                 \n"
    "uniform   vec3  ambient;                                                                \n"
    "uniform   vec3  light1Position;                                                         \n"
    "uniform   vec3  light1Color;                                                            \n"
    "uniform   float light1Irradiance;                                                       \n"
    "uniform   vec3  light2Position;                                                         \n"
    "uniform   vec3  light2Color;                                                            \n"
    "uniform   float light2Irradiance;                                                       \n"
    "                                                                                        \n"
    "varying   vec3  light;                                                                  \n"
    "                                                                                        \n"
    "void main () {                                                                          \n"
    "  gl_Position        = mvp * vec4 (position,1);                                         \n"
    "  float light1Factor = light1Irradiance * max (0.0, dot (light1Position, normal));      \n"
    "  float light2Factor = light2Irradiance * max (0.0, dot (light2Position, normal));      \n"
    "  vec3  light1       = light1Color * vec3 (light1Factor);                               \n"
    "  vec3  light2       = light2Color * vec3 (light2Factor);                               \n"
    "        light        = ambient + light1 + light2;                                       \n"
    "}                                                                                       \n"
    ;
}

const std::string Shader::smoothFragmentShader () {
  return
    "#version 120                                                                            \n"
    "                                                                                        \n"
    "uniform vec3 color;                                                                     \n"
    "varying vec3 light;                                                                     \n"
    "                                                                                        \n"
    "void main () {                                                                          \n"
    "  gl_FragColor = vec4 (color * light, 1.0);                                             \n"
    "}                                                                                       \n"
    ;
}

const std::string Shader::simpleVertexShader () {
  return
    "#version 120                                                                            \n"
    "                                                                                        \n"
    "uniform   mat4 mvp;                                                                     \n"
    "attribute vec3 position;                                                                \n"
    "                                                                                        \n"
    "void main(){                                                                            \n"
    "  gl_Position = mvp * vec4 (position,1);                                                \n"
    "}                                                                                       \n"
    ;
}

const std::string Shader::simpleFragmentShader () {
  return
    "#version 120                                                                            \n"
    "                                                                                        \n"
    "uniform vec3 color;                                                                     \n"
    "                                                                                        \n"
    "void main(){                                                                            \n"
    "  gl_FragColor = vec4 (color, 1.0);                                                     \n"
    "}                                                                                       \n"
    ;
}

/*
const std::string Shader::flatVertexShader () {
  return
    "#version 120                                                                            \n"
    "                                                                                        \n"
    "uniform   mat4 mvp;                                                                     \n"
    "attribute vec3 position;                                                                \n"
    "attribute vec3 normal;                                                                  \n"
    "                                                                                        \n"
    "varying   vec3 varPosition;                                                             \n"
    "                                                                                        \n"
    "void main () {                                                                          \n"
    "  gl_Position = mvp * vec4 (position,1);                                                \n"
    "  varPosition = position;                                                               \n"
    "}                                                                                       \n"
    ;
}

const std::string Shader::flatFragmentShader () {
  return
    "#version 120                                                                            \n"
    "                                                                                        \n"
    "uniform vec3  color;                                                                    \n"
    "uniform vec3  ambient;                                                                  \n"
    "uniform vec3  light1Position;                                                           \n"
    "uniform vec3  light1Color;                                                              \n"
    "uniform float light1Irradiance;                                                         \n"
    "uniform vec3  light2Position;                                                           \n"
    "uniform vec3  light2Color;                                                              \n"
    "uniform float light2Irradiance;                                                         \n"
    "                                                                                        \n"
    "varying vec3 varPosition;                                                               \n"
    "                                                                                        \n"
    "void main () {                                                                          \n"
    "  vec3  normal       = normalize(cross(dFdx(varPosition),dFdy(varPosition)));           \n"
    "                                                                                        \n"
    "  float light1Factor = light1Irradiance * max (0.0, dot  (light1Position, normal));     \n"
    "  float light2Factor = light2Irradiance * max (0.0, dot  (light2Position, normal));     \n"
    "  vec3  light1       = light1Color * vec3 (light1Factor);                               \n"
    "  vec3  light2       = light2Color * vec3 (light2Factor);                               \n"
    "                                                                                        \n"
    "  vec3  light        = ambient + light1 + light2;                                       \n"
    "  gl_FragColor       = vec4 (color * light, 1.0);                                       \n"
    "}                                                                                       \n"
    ;
}
*/

const std::string Shader::flatVertexShader () {
  return
    "#version 120                                                                            \n"
    "                                                                                        \n"
    "uniform   mat4 mvp;                                                                     \n"
    "attribute vec3 position;                                                                \n"
    "attribute vec3 normal;                                                                  \n"
    "                                                                                        \n"
    "varying   vec3 varPosition;                                                             \n"
    "varying   vec3 varNormal;                                                               \n"
    "                                                                                        \n"
    "void main () {                                                                          \n"
    "  gl_Position = mvp * vec4 (position,1);                                                \n"
    "  varPosition = position;                                                               \n"
    "  varNormal   = normal;                                                                 \n"
    "}                                                                                       \n"
    ;
}

const std::string Shader::flatFragmentShader () {
  return
    "#version 120                                                                            \n"
    "                                                                                        \n"
    "uniform vec3  color;                                                                    \n"
    "uniform vec3  ambient;                                                                  \n"
    "uniform vec3  eyePoint;                                                                 \n"
    "uniform vec3  light1Position;                                                           \n"
    "uniform vec3  light1Color;                                                              \n"
    "uniform float light1Irradiance;                                                         \n"
    "uniform vec3  light2Position;                                                           \n"
    "uniform vec3  light2Color;                                                              \n"
    "uniform float light2Irradiance;                                                         \n"
    "                                                                                        \n"
    "varying vec3 varPosition;                                                               \n"
    "varying vec3 varNormal;                                                                 \n"
    "                                                                                        \n"
    "void main () {                                                                          \n"
    //"  vec3  normal       = normalize(cross(dFdx(varPosition),dFdy(varPosition)));           \n"
    "  vec3  toE          = normalize (eyePoint - varPosition); \n"
    "  float f = dot (varNormal, toE); \n"
    "  if (f < 0.2) { \n"
    "    gl_FragColor       = vec4 (1.0,0.0,0.0, 1.0); \n"
    "  } else { \n"
    "    gl_FragColor       = vec4 (color * vec3 (f), 1.0);                                       \n"
    "  }                                                                                     \n"
    "}                                                                                       \n"
    ;
}
