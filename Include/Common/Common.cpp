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

