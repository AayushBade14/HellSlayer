#include "./Animation.hpp"

Animation::Animation(const std::string& path,Model& model){
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(path.c_str(),aiProcess_Triangulate | aiProcess_GenNormals);
  if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
    std::cerr<<"ERROR::ASSIMP: Loading animation ->"<<path<<" ->"<<importer.GetErrorString()<<std::endl;
    return;
  }
  const aiAnimation* animation = scene->mAnimations[0];

  duration = animation->mDuration;
  ticksPerSecond = animation->mTicksPerSecond;

  ReadHierarchy(rootNode,scene->mRootNode);
  ReadMissingBones(animation,model);
}

void Animation::ReadHierarchy(AssimpNodeData& dest,const aiNode* node){
  dest.name = node->mName.C_Str();
  dest.childrenCount = node->mNumChildren;
  dest.transformation = getGLMMat(node->mTransformation);

  for(unsigned int i = 0; i < node->mNumChildren; i++){
    AssimpNodeData newNodeData;
    ReadHierarchy(newNodeData,node->mChildren[i]);
    dest.children.push_back(newNodeData);
  }
}

void Animation::ReadMissingBones(const aiAnimation* animation,Model& model){

  std::map<std::string,BoneInfo>& modelBoneInfoMap = model.GetBoneInfoMap();
  int& modelBoneCounter = model.GetBoneCounter();

  for(unisgned int i = 0; i < animation->mNumChannels; i++){
    aiNodeAnim* channel = animation->mChannels[i];

    std::string boneName = channel->mNodeName.C_Str();
    if(modelBoneInfoMap.find(boneName)==modelBoneInfoMap.end()){
      BoneInfo newBoneInfo;
      newBoneInfo.id = modelBoneCounter;
      newBoneInfo.offsetMatrix = glm::mat4(1.0f);
      modelBoneCounter++;
      modelBoneInfoMap[boneName] = newBoneInfo;
    }
    animationBones.push_back(Bone(boneName,modelBoneInfoMap[boneName].id,channel));
  }

  animationBoneInfoMap = modelBoneInfoMap;
}

Bone* FindBone(const std::string& boneName){
  int index = -1;
  std::cout<<"SIZE_OF_ANIMATION_BONES: "<<animationBones.size()<<std::endl;
  for(unsigned int i = 0; i < animationBones.size(); i++){
    if(animationBones[i].GetName() == boneName){
      index = i;
      break;
    }
  }
  if(index == -1){
    std::cerr<<"NO-BONE-FOUND -> "<<boneName<<std::endl;
    return nullptr;
  }
  else{
    return &animationBones[index];
  }
}
