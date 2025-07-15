#pragma once

#include "../Common/Common.hpp"

class Bone{
private:
  int id;
  std::string name;
  
  int nposkey;
  int nrotkey;
  int scalekey;

  std::vector<PositionKey> positionKeys;
  std::vector<RotationKey> rotationKeys;
  std::vector<ScalingKey> scalingKeys;
  
  glm::mat4 localTransform;
  
  float GetScalingFactor(float lastFrame,float nextFrame,float animTime);

  int GetPositionIndex(float animTime);
  int GetRotationIndex(float animTime);
  int GetScalingIndex(float animTime);

  glm::mat4 InterpolatePosition(float animTime);
  glm::mat4 InterpolateRotation(float animTime);
  glm::mat4 InterpolateScaling(float animTime);

public:
  Bone(const std::string& name,int id,const aiNodeAnim* channel);
  ~Bone() = default;

  void Update(float animTime);

  int GetId() {return id;}
  std::string GetName() {return name;}
  glm::mat4 GetLocalTransform() {return localTransform;}
};
