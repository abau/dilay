#version 120

uniform   mat4 mvp;
attribute vec3 position;
attribute vec3 normal;

varying   vec3 varPosition;

void main () {
  gl_Position = mvp * vec4 (position,1); 
  varPosition = position; 
}
