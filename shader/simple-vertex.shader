#version 120

uniform   mat4 mvp;
attribute vec3 position;

void main(){
  gl_Position = mvp * vec4 (position,1); 
}
