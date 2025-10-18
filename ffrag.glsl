#version 460 core

out vec4 fragColor;

in vec2 TC;

uniform sampler2D flash;

void main(){
  vec4 color = texture(flash,TC);
  if(color.a < 0.1) 
    discard;
  fragColor = color;
}
