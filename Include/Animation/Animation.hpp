#pragma once

#include "../Common/Common.hpp"
#include "../Bone/Bone.hpp"
#include "../Model/Model.hpp"

class Animation{
private:
  float duration;
  float ticksPerSecond;
  
  AssimpNodeData rootNode;
  std::vector<Bone> animationBones;
  std::map<std::string,BoneInfo> animationBoneInfoMap;

public:
  Animation(const std::string& path,Model& model);
  ~Animation() = default;


};
