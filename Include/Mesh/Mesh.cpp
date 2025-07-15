#include "./Mesh.hpp"

Mesh::Mesh(std::vector<Vertex> vertices,std::vector<unsigned int> indices,std::vector<Texture> textures){
  this->vertices = vertices;
  this->indices = indices;
  this->textures = textures;

  SetupMesh();
}

void Mesh::SetupMesh(){
  glGenVertexArrays(1,&Vao);
  glGenBuffers(1,&Vbo);
  glGenBuffers(1,&Ebo);

  glBindVertexArray(Vao);
  
  glBindBuffer(GL_ARRAY_BUFFER,Vbo);
  glBufferData(GL_ARRAY_BUFFER,sizeof(Vertex)*vertices.size(),vertices.data(),GL_STATIC_DRAW);
  
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,Ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(unsigned int)*indices.size(),indices.data(),GL_STATIC_DRAW);
  
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,position));
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,normal));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,textureCoord));
  glEnableVertexAttribArray(2);

  glVertexAttribIPointer(3,4,GL_INT,sizeof(Vertex),(void*)offsetof(Vertex,boneIds));
  glEnableVertexAttribArray(3);

  glVertexAttribPointer(4,4,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,weights));
  glEnableVertexAttribArray(4);
  
  glBindVertexArray(0);
}

void Mesh::Draw(Shader& shader){
  shader.Use();

  int diffuseNr = 1;
  int specularNr = 1;
  
  for(unsigned int i = 0; i < textures.size(); i++){
    std::string type = textures[i].type;
    std::string number;

    if(type == "texture_diffuse")
      number = std::to_string(diffuseNr++);
    else if(type == "texture_specular")
      number = std::to_string(specularNr++);

    type += number;

    shader.SetValue(type,i);

    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D,textures[i].id);
  }

  glBindVertexArray(Vao);
  glDrawElements(GL_TRIANGLES,indices.size(),GL_UNSIGNED_INT,0);
  glBindVertexArray(0);

  for(unsigned int i = 0; i < textures.size(); i++){
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D,0);
  }

  glActiveTexture(GL_TEXTURE0);
}
