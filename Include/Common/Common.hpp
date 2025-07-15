#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "../ThirdParty/STB_IMAGE/stb_image.h"

#define MAX_BONE_INFLUENCE 4

struct Vertex{
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec3 textureCoord;

  int boneIds[MAX_BONE_INFLUENCE];
  float weights[MAX_BONE_INFLUENCE];
};

struct Texture{
  unsigned int id;
  std::string type;
  std::string filename;
};

struct BoneInfo{
  int id;
  glm::mat4 offsetMatrix;
};

struct PositionKey{
  glm::vec3 position;
  float timeStamp;
};

struct RotationKey{
  glm::quat rotation;
  float timeStamp;
};

struct ScalingKey{
  glm::vec3 scale;
  float timeStamp;
};

struct AssimpNodeData{
  std::string name;
  int childrenCount;
  std::vector<AssimpNodeData> children;
  glm::mat4 transformation;
};

// for model loading
std::vector<Texture> LoadMaterialTexture(const aiMaterial* material,aiTextureType type,const std::string& typeName, const std::string& directory);

void LoadTextureFromFile(Texture& texture,const std::string& directory);
