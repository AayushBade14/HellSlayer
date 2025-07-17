#pragma once

#include "../Common/Common.hpp"
#include "../Mesh/Mesh.hpp"

class Model{
private:
  int boneCount = 0;
  std::string directory;
  std::map<std::string,BoneInfo> boneInfoMap;
  std::vector<Mesh> meshes;

  void LoadModel(const std::string& path);
  void ProcessNode(aiNode* node,const aiScene* scene);
  Mesh ProcessMesh(aiMesh* mesh,const aiScene* scene);
  void ExtractBoneData(std::vector<Vertex>& vertices,aiMesh* mesh,const aiScene* scene);
  
  void SetDefaultVertexBoneData(Vertex& vertex);
  void SetVertexBoneData(Vertex& vertex, int id, float weight);
public:
  Model(const std::string& path);
  ~Model()=default;

  void Draw(Shader& shader);
  
  std::map<std::string,BoneInfo>& GetBoneInfoMap() {return boneInfoMap;}
  int& GetBoneCounter() {return boneCount;}
};
