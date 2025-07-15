#pragma once

#include "../Common/Common.hpp"

class Mesh{
private:
  unsigned int Vbo;
  unsigned int Vao;
  unsigned int Ebo;

  std::vector<Vertex> vertices;
  std::vector<Texture> textures;
  std::vector<unsigned int> indices;

  void SetupMesh();

public:
  Mesh(std::vector<Vertex> vertices,std::vector<unsigned int> indices,std::vector<Texture> textures);
  ~Mesh()=default;

  void Draw(Shader& shader);
};

