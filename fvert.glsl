#version 460 core

layout(location=0)in vec2 aPos;
layout(location=1)in vec2 aTC;

out vec2 TC;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float z;

void main(){
  gl_Position = projection * view * model * vec4(aPos, z,1.0);
  TC = aTC;
}
