#version 120

uniform vec4 color;
varying vec3 light;

void main(){
  gl_FragColor = color * vec4 (light, 1.0);
}
