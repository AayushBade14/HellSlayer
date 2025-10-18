#version 460 core

out vec4 fragColor;

in vec2 TC;

uniform sampler2D crosshair;

void main(){
  fragColor = texture(crosshair,TC);;
}

