#version 460 core

out vec4 fragColor;

in vec2 TC;

uniform sampler2D flash;

void main(){
  fragColor = texture(flash,TC);;
}
