#version 460 core

out vec4 fragColor;

in vec3 Normal;
in vec2 TC;
in vec3 FragPos;

void main(){
  vec3 l = normalize(vec3(2.0,2.0,2.0)-FragPos);

  float diff = max(dot(Normal,l),0.0);
  vec3 diffuse = diff * vec3(1.0,0.4,0.0);

  fragColor = vec4(diffuse,1.0);
}
