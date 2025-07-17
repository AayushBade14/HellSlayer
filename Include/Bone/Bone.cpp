#include "./Bone.hpp"

Bone::Bone(const std::string& name, int id, const aiNodeAnim* channel){
  this->name = name;
  this->id = id;
  localTransform = glm::mat4(1.0f); 
  nposkey = channel->mNumPositionKeys;
  for(int i = 0; i < nposkey; i++){
    aiVectorKey key = channel->mPositionKeys[i];
    
    PositionKey tmp;
    tmp.timeStamp = key.mTime;
    tmp.position = getGLMVec(key.mValue);

    positionKeys.push_back(tmp); 
  }

  nrotkey = channel->mNumRotationKeys;
  for(int i = 0; i < nrotkey; i++){
    aiQuatKey key = channel->mRotationKeys[i];

    RotationKey tmp;
    tmp.timeStamp = key.mTime;
    tmp.rotation = getGLMQuat(key.mValue);

    rotationKeys.push_back(tmp);
  }

  nscalekey = channel->mNumScalingKeys;
  for(int i = 0; i < nscalekey; i++){
    aiVectorKey key = channel->mScalingKeys[i];

    ScalingKey tmp;
    tmp.timeStamp = key.mTime;
    tmp.scale = getGLMVec(key.mValue);

    scalingKeys.push_back(tmp);
  }
}

void Bone::Update(float animTime){
  glm::mat4 T = InterpolatePosition(animTime);
  glm::mat4 R = InterpolateRotation(animTime);
  glm::mat4 S = InterpolateScaling(animTime);
  localTransform = T * R * S; 
}

float Bone::GetScalingFactor(float lastFrame,float nextFrame,float animTime){
  float sf = 0.0f;
  float progress = animTime - lastFrame;
  float total = nextFrame - lastFrame;
  sf = progress/total;
  return sf;
}

int Bone::GetPositionIndex(float animTime){
  for(int i = 0; i < nposkey - 1; i++){
    if(animTime < positionKeys[i+1].timeStamp)
      return i;
  }
  assert(0);
}

int Bone::GetRotationIndex(float animTime){
  for(int i = 0; i < nrotkey - 1; i++){
    if(animTime < rotationKeys[i+1].timeStamp)
      return i;
  }
  assert(0);
}

int Bone::GetScalingIndex(float animTime){
  for(int i = 0; i < nscalekey - 1; i++){
    if(animTime < scalingKeys[i+1].timeStamp)
      return i;
  }
  assert(0);
}

glm::mat4 Bone::InterpolatePosition(float animTime){

  if(nposkey == 1)
    return glm::translate(glm::mat4(1.0f),positionKeys[0].position);

  int p0 = GetPositionIndex(animTime);
  int p1 = p0 + 1;
  
  float sf = GetScalingFactor(positionKeys[p0].timeStamp,positionKeys[p1].timeStamp,animTime);

  glm::vec3 finalPos = glm::mix(positionKeys[p0].position,positionKeys[p1].position,sf);

  return glm::translate(glm::mat4(1.0f),finalPos);
}

glm::mat4 Bone::InterpolateRotation(float animTime){

  if(nrotkey == 1){
    auto rotation = glm::normalize(rotationKeys[0].rotation);
    return glm::toMat4(rotation);
  }
  int p0 = GetRotationIndex(animTime);
  int p1 = p0 + 1;

  float sf = GetScalingFactor(rotationKeys[p0].timeStamp,rotationKeys[p1].timeStamp,animTime);

  glm::quat finalRot = glm::slerp(rotationKeys[p0].rotation,rotationKeys[p1].rotation,sf);
  finalRot = glm::normalize(finalRot);
  
  return glm::toMat4(finalRot);
}

glm::mat4 Bone::InterpolateScaling(float animTime){
  if(nscalekey == 1){
    return glm::scale(glm::mat4(1.0f),scalingKeys[0].scale);
  }
  int p0 = GetScalingIndex(animTime);
  int p1 = p0 + 1;

  float sf = GetScalingFactor(scalingKeys[p0].timeStamp,scalingKeys[p1].timeStamp,animTime);

  glm::vec3 finalScale = glm::mix(scalingKeys[p0].scale,scalingKeys[p1].scale,sf);

  return glm::scale(glm::mat4(1.0f),finalScale);
}
