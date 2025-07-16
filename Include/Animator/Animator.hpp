#pragma once

#include "../Animation/Animation.hpp"

class Animator{
private:
  Animation* currentAnimation;
  float currentTime;
  float deltaTime;
  
  std::vector<glm::mat4> skinnedMatrix;

  void CalculateBoneTransforms(AssimpNodeData& node,glm::mat4 parentTransform);
public:
  Animator::Animator(Animation& animation);
  ~Animator() = default;

  void UpdateAnimation(float dt);

  void PlayAnimation(Animation& newAnimation);

  std::vector<glm::mat4> GetSkinnedMatrix() {return skinnedMatrix;}
};
