#version 460 core

layout(location=0)in vec3 aPos;
layout(location=1)in vec3 aNormal;
layout(location=2)in vec2 aTextureCoord;
layout(location=3)in ivec4 boneIds;
layout(location=4)in vec4 weights;

out vec2 TextureCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
  gl_Position = projection * view * model * vec4(aPos,1.0);
  TextureCoord = aTextureCoord;
}
