#version 120

uniform vec4 color;
varying vec3 light;

varying   vec3 p;

void main(){
  gl_FragColor = color * vec4 (light, 1.0);

  vec3 n = normalize(cross(dFdx(p),dFdy(p)));
  float lightFactor = 2.0 * max (0.0, dot  (normalize (vec3 (1.0,1.0,1.0)), n));
  vec3 l             = vec3 (lightFactor);
  gl_FragColor = color * vec4 (l, 1.0);
}
