#version 120

uniform vec3  color;
uniform vec3  ambient;
uniform vec3  light1Position;
uniform vec3  light1Color;
uniform float light1Irradiance;
uniform vec3  light2Position;    
uniform vec3  light2Color;
uniform float light2Irradiance;

varying vec3 varPosition;

void main () {
  vec3  normal       = normalize(cross(dFdx(varPosition),dFdy(varPosition)));

  float light1Factor = light1Irradiance * max (0.0, dot  (light1Position, normal));
  float light2Factor = light2Irradiance * max (0.0, dot  (light2Position, normal));
  vec3  light1       = light1Color * vec3 (light1Factor);
  vec3  light2       = light2Color * vec3 (light2Factor);

  vec3  light        = ambient + light1 + light2;
  gl_FragColor       = vec4 (color * light, 1.0);
}
