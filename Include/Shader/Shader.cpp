#include "./Shader.hpp"

Shader::Shader(const std::string& vertexPath,const std::string& fragmentPath){
  std::string vertexCode = LoadFile(vertexPath);
  std::string fragmentCode = LoadFile(fragmentPath);

  unsigned int vertexShader = CompileShader(vertexCode);
  unsigned int fragmentShader = CompileShader(fragmentCode);

  CreateShaderProgram(vertexShader,fragmentShader);
}

Shader::~Shader(){
  glDeleteProgram(iD);
}

std::string Shader::LoadFile(const std::string& path){
  std::string code;
  std::ifstream file;

  file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try{
    file.open(path);
    std::stringstream stream;
    stream << file.rdbuf();
    file.close();
    code = stream.str();
  }

  return code;
}

unsigned int CompileShader(const std::string& srcCode,bool isVertex){
  const char* code = srcCode.c_str();
  int success;
  char infoLog[512];

  unsigned int shader = isVertex? glCreateShader(GL_VERTEX_SHADER) : glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(shader,1,&code,NULL);
  glCompileShader(shader);

  glGetShaderiv(shader,GL_COMPILE_STATUS,&success);
  if(!success){
    glGetShaderInfoLog(shader,512,NULL,infoLog);
    std::cerr<<"ERROR: Reading compiling "<< (isVertex? "VERTEX" : "FRAGMENT") << " shader-> "<<infoLog<<std::endl;
  }
  std::cout<<"SHADER: Successfully compiled "<<(isVertex? "VERTEX" : "FRAGMENT")<<" shader!"<<std::endl;
  return shader;
}

void CreateShaderProgram(unsigned int& vertexShader,unsigned int& fragmentShader){
  int success;
  char infoLog[512];

  iD = glCreateProgram();
  glAttachShader(iD,vertexShader);
  glAttachShader(iD,fragmentShader);
  glLinkProgram(iD);

  glGetProgramiv(iD,GL_LINK_STATUS,&success);
  if(!success){
    glGetProgramInfoLog(iD,512,NULL,infoLog);
    std::cerr<<"ERROR: creating shader program -> "<<infoLog<<std::endl;
    return;
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

void Shader::Use(){
  glUseProgram(iD);
}
