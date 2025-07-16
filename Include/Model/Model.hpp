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
  void ProcessNode(const aiNode* node,const aiScene* scene);
  Mesh ProcessMesh(const aiMesh* mesh,const aiScene* scene);
  void ExtractBoneData(std::vector<Vertex>& vertices,const aiMesh* mesh,const aiScene* scene);
  
  void SetDefaultVertexBoneData(Vertex& vertex);
  void SetVertexBoneData(Vertex& vertex, int id, float weight);
public:
  Model(const std::string& path);
  ~Model()=default;

  void Draw(Shader& shader);
  
  std::map<std::string,BoneInfo>& GetBoneInfoMap() {return boneInfoMap;}
  int& GetBoneCounter() {return boneCount;}
};
