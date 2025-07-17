#include "./Animator.hpp"

Animator::Animator(Animation& animation){
  currentAnimation = &animation;
  currentTime = 0.0f;
  deltaTime = 0.0f;
  
  skinnedMatrix.reserve(100);
  for(int i = 0; i < 100; i++){
    skinnedMatrix.push_back(glm::mat4(1.0f));
  }
}

void Animator::UpdateAnimation(float dt){
  deltaTime = dt;
  if(currentAnimation){
    currentTime += currentAnimation->GetTicksPerSecond() * dt;
    currentTime = fmod(currentTime,currentAnimation->GetDuration());
    CalculateBoneTransforms(currentAnimation->GetRootNode(),glm::mat4(1.0f));
  }
}

void Animator::PlayAnimation(Animation& newAnimation){
  currentAnimation = &newAnimation;
  currentTime = 0.0f;
}

void Animator::CalculateBoneTransforms(AssimpNodeData& node,glm::mat4 parentTransform){
  std::string boneName = node.name;
  glm::mat4 localTransform = node.transformation;

  Bone* bone = currentAnimation->FindBone(boneName);
  if(bone){
    bone->Update(currentTime);
    localTransform = bone->GetLocalTransform();
  }
  
  glm::mat4 globalTransform = parentTransform * localTransform;
  
  std::map<std::string,BoneInfo>& animationBoneInfoMap = currentAnimation->GetAnimationBoneInfoMap();

  if(animationBoneInfoMap.find(boneName) != animationBoneInfoMap.end()){
    int index = animationBoneInfoMap[boneName].id;
    glm::mat4 offsetMatrix = animationBoneInfoMap[boneName].offsetMatrix;
    skinnedMatrix[index] = globalTransform * offsetMatrix;
  }

  for(int i = 0; i < node.childrenCount; i++){
    CalculateBoneTransforms(node.children[i],globalTransform);
  }
}
