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

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 skinnedMatrix[MAX_BONES];

void main(){
  
  vec4 totalTransform = vec4(0.0);
  for(int i = 0; i < MAX_BONE_INFLUENCE; i++){
    if(boneIds[i] == -1) continue;
    if(boneIds[i] >= MAX_BONES){
      totalTransform = vec4(aPos,1.0);
      break;
    }

    totalTransform += skinnedMatrix[boneIds[i]] * vec4(aPos,1.0) * weights[i];
  }

  gl_Position = projection * view * model * totalTransform;
  TextureCoord = aTextureCoord;
}
