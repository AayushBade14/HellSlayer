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
  
  void ReadHierarchy(AssimpNodeData& dest,const aiNode* node);
  void ReadMissingBones(const aiAnimation* animation,Model& model);

public:
  Animation(const std::string& path,Model& model);
  ~Animation() = default;
  
  Bone* FindBone(const std::string& boneName);

  float GetDuration() {return duration;}
  float GetTicksPerSecond() {return ticksPerSecond;}
  AssimpNodeData& GetRootNode() {return rootNode;}
  std::map<std::string,BoneInfo>& GetAnimationBoneInfoMap() {return animationBoneInfoMap;}
};
