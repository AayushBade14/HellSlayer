#version 460 core

out vec4 fragColor;

in vec3 Normal;
in vec2 TC;
in vec3 FragPos;

uniform bool isShot;

void main(){
  float ambient = 0.1;
  vec3 color = vec3(0.8);

  if(isShot){ 
    ambient = 0.2;
    color = vec3(1.0,0.91,0.78);
  }
  else {
    ambient = 0.1;
    color = vec3(0.8);
  }

  vec3 l = normalize(vec3(2.0,2.0,2.0)-FragPos);

  float diff = max(dot(Normal,l),0.0);
  vec3 diffuse = diff * color + ambient;

  fragColor = vec4(diffuse,1.0);
}
