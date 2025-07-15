#include "./Common.hpp"

std::vector<Texture> LoadMaterialTexture(const aiMaterial* material,aiTextureType type,const std::string& typeName, const std::string& directory){
  std::vector<Texture> textures;

  for(unsigned int i = 0; i < material->GetTextureCount(type); i++){
    aiString path;
    material->GetTexture(type,i,&path);

    std::string fullPath = path.C_Str();
    fullPath = fullPath.substr(fullPath.find_last_of("/\\") + 1);
    
    Texture texture;
    texture.type = typeName;
    texture.filename = fullPath;

    LoadTextureFromFile(texture,directory);

    textures.push_back(texture);
  }

  return textures;
}

void LoadTextureFromFile(Texture& texture,const std::string& directory){
  std::string fullPath = directory + "/" + texture.filename;

  glGenTextures(1,&(texture.id));
  glBindTexture(GL_TEXTURE_2D,texture.id);

  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  
  stbi_set_flip_vertically_on_load(true);

  int width;
  int height;
  int nrChannels;

  unsigned char* data = stbi_load(fullPath.c_str(),&width,&height,&nrChannels,0);
  GLenum flag = nrChannels == 3 ? GL_RGB : GL_RGBA;
  if(data){
    glTexImage2D(GL_TEXTURE_2D,0,flag,width,height,0,flag,GL_UNSIGNED_BYTE,data);
    glGenerateMipmap(GL_TEXTURE_2D);
  }

  stbi_image_free(data);
  std::cout<<"TEXTURE: Loaded "<<texture.filename<<" successfully!"<<std::endl;
}


// glm helpers
glm::vec3 getGLMVec(const aiVector3D& v){
  return glm::vec3(v.x,v.y,v.z);
}

glm::vec2 getGLMVec(const aiVector2D& v){
  return glm::vec2(v.x,v.y);
}

glm::quat getGLMQuat(const aiQuaternion& q){
  return glm::quat(q.w,q.x,q.y,q.z);
}

glm::mat4 getGLMMat(const aiMatrix4x4& from){
  glm::mat4 to;

  to[0][0] = from.a1; to[0][1] = from.a2; to[0][2] = from.a3; to[0][3] = from.a4;
  to[1][0] = from.b1; to[1][1] = from.b2; to[1][2] = from.b3; to[1][3] = from.b4;
  to[2][0] = from.c1; to[2][1] = from.c2; to[2][2] = from.c3; to[2][3] = from.c4;
  to[3][0] = from.d1; to[3][1] = from.d2; to[3][2] = from.d3; to[3][3] = from.d4;
  
  return to;

}

