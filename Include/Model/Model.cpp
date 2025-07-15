#include "./Model.hpp"

Model::Model(const std::string& path){
  LoadModel(path);
}

void Model::LoadModel(const std::string& path){
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(path.c_str(),aiProcess_Triangulate | aiProcess_GenNormals);
  if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
    std::cerr<<"ERROR::ASSIMP: Loading model into assimp data-structures ->"<<importer.GetErrorString()<<" -> "<<path<<std::endl;
    return;
  }
  
  std::filesystem::path fullPath(path);
  directory = fullPath.parent_path().string();

  std::cout<<"ASSIMP: Successfully loaded model into assimp data-structures -> "<<path<<std::endl;

  ProcessNode(scene->mRootNode,scene);
}

void Model::SetDefaultVertexBoneData(Vertex& vertex){
  for(int i = 0; i < MAX_BONE_INFLUENCE; i++){
    vertex.boneIds[i] = -1;
    vertex.weights[i] = 0.0f;
  }
}

void Model::SetVertexBoneData(Vertex& vertex,int id,float weight){
 for(int i = 0; i < MAX_BONE_INFLUENCE; i++){
    if(vertex.boneIds[i] < 0){
      vertex.boneIds[i] = id;
      vertex.weights[i] = weight;
      break;
    }
  } 
}

void Model::ProcessNode(const aiNode* node,const aiScene* scene){
  for(unsigned int meshIndex = 0; meshIndex < node->mNumMeshes; meshIndex++){
    aiMesh* mesh = scene->mMeshes[node->mMeshes[meshIndex]];
    meshes.push_back(ProcessMesh(mesh,scene));
  }

  for(unsigned int childIndex = 0; childIndex < node->mNumChildren; childIndex++){
    ProcessNode(node->mChildren[childIndex],scene);
  }
}

Mesh Model::ProcessMesh(const aiMesh* mesh, const aiScene* scene){
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;
  
  for(unsigned int vertexIndex = 0; vertexIndex < mesh->mNumVertices; vertexIndex++){
    Vertex vertex;
    
    SetDefaultVertexBoneData(vertex);

    vertex.position = getGLMVec(mesh->mVertices[vertexIndex]);

    if(mesh->HasNormals())
      vertex.normal = getGLMVec(mesh->mNormals[vertexIndex]);

    if(mesh->mTextureCoords[0])
      vertex.textureCoord = getGLMVec(mesh->mTextureCoords[0][vertexIndex]);
    
    vertices.push_back(vertex);
  }

  for(unsigned int faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++){
    aiFace face = mesh->mFaces[faceIndex];
    for(unsigned int index = 0; index < face.mNumIndices; index++){
      indices.push_back(face.mIndices[index]);
    }
  }

  if(mesh->mMaterialIndex >= 0){
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    std::vector<Texture> diffuseMaps = LoadMaterialTexture(material,aiTextureType_DIFFUSE,"texture_diffuse",directory);
    std::vector<Texture> specularMaps = LoadMaterialTexture(material,aiTextureType_SPECULAR,"texture_specular",directory);

    textures.insert(textures.end(),diffuseMaps.begin(),diffuseMaps.end());
    textures.insert(textures.end(),diffuseMaps.begin(),diffuseMaps.end());
  }

  ExtractBoneData(vertices,mesh,scene);
  return Mesh(vertices,indices,textures);
}

void Model::ExtractBoneData(std::vector<Vertex>& vertices,const aiMesh* mesh,const aiScene* scene){
  for(unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++){
    int boneId = -1;
    std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
    
    if(boneInfoMap.find(boneName)==boneInfoMap.end()){
      // bone not found it entry so create a new entry
      BoneInfo newBoneInfo;
      boneId = boneCount;
      newBoneInfo.id = boneCount;
      newBoneInfo.offsetMatrix = getGLMMat(mesh->mBones[boneIndex]->mOffsetMatrix);
      boneInfoMap[boneName] = newBoneInfo;
      boneCount++;
    }
    else{
      boneId = boneInfoMap[boneName].id; 
    }

    std::vector<aiVertexWeight> weightsArray = mesh->mBones[boneIndex]->mWeights;
    for(unsigned int weightIndex = 0; weightIndex < mesh->mBones[boneIndex]->mNumWeights; weightIndex++){
      int vertexId = weightsArray[weightIndex].mVertexId;
      float weight = weightsArray[weightIndex].mWeight;
      SetVertexBoneData(vertices[vertexId],boneId,weight);
    }
  }
}

void Model::Draw(Shader& shader){
  for(unsigned int i = 0; i < meshes.size(); i++){
    meshes[i].Draw(shader);
  }
}
