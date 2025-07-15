#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

class Shader{
private:
  unsigned int iD;

  std::string LoadFile(const std::string& path);
  unsigned int CompileShader(const std::string& srcCode);
  void CreateShaderProgram(unsigned int& vertexShader,unsigned int& fragmentShader);

public:
  Shader(const std::string& vertexPath,const std::string& fragmentPath);
  ~Shader();

  void Use();

  template <typename T>
  void SetValue(const std::string& name,const T& value){
    unsigned int loc = glGetUniformLocation(iD,name.c_str());
    
    if constexpr(std::is_same_v<T,int>)
      glUniform1i(loc,value);
    else if constexpr(std::is_same_v<T,bool>)
        glUniform1i(loc,(int)value);
    else if constexpr(std::is_same_v<T,float>)
        glUniform1f(loc,value);
    else if constexpr(std::is_same_v<T,glm::vec2>)
        glUniform2fv(loc,1,glm::value_ptr(value));
    else if constexpr(std::is_same_v<T,glm::vec3>)
        glUniform3fv(loc,1,glm::value_ptr(value));
    else if constexpr(std::is_same_v<T,glm::mat4>)
        glUniformMatrix4fv(loc,1,GL_FALSE,glm::value_ptr(value));
  }
};
