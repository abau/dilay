#version 120

uniform vec3 color;
varying vec3 light;

void main(){
  gl_FragColor = vec4 (color * light, 1.0);
}
