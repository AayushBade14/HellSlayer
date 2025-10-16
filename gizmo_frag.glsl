#version 460 core

out vec4 fragColor;

in vec3 Normal;
in vec2 TC;
in vec3 FragPos;

uniform int gizmo_type;

void main(){
  vec3 color = vec3(0.0);
  if(gizmo_type == 0) color = vec3(1.0,0.0,0.0);
  else if(gizmo_type == 1) color = vec3(0.0,1.0,0.0);
  else if(gizmo_type == 2) color = vec3(0.0,0.0,1.0);
  fragColor = vec4(color,1.0);
}

