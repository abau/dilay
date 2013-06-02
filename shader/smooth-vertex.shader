#version 120

uniform   mat4  mvp;
attribute vec3  position;
attribute vec3  normal;

uniform   vec3  ambient;
uniform   vec3  light1Position;
uniform   vec3  light1Color;
uniform   float light1Irradiance;
uniform   vec3  light2Position;    
uniform   vec3  light2Color;
uniform   float light2Irradiance;

varying   vec3  light;

void main () {
  gl_Position        = mvp * vec4 (position,1); 
  float light1Factor = light1Irradiance * max (0.0, dot (light1Position, normal));
  float light2Factor = light2Irradiance * max (0.0, dot (light2Position, normal));
  vec3  light1       = light1Color * vec3 (light1Factor);
  vec3  light2       = light2Color * vec3 (light2Factor);
        light        = ambient + light1 + light2;
}
