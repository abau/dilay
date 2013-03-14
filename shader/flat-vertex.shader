#version 120

uniform   mat4 mvp;
attribute vec3 position;
attribute vec3 normal;

varying   vec3 light;
varying   vec3 p;

void main(){
  gl_Position       = mvp * vec4 (position,1); 
  float lightFactor = 2.0 * max (0.0, dot  (vec3 (0.0,1.0,0.0), normal));
  light             = vec3 (lightFactor);
  p                 = position; //vec3(gl_Position.x,gl_Position.y,gl_Position.z);
}
