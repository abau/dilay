#version 120

uniform mat4 mvp;

void main(){
  gl_Position = mvp * gl_Vertex;
  gl_FrontColor = gl_Color;
}
