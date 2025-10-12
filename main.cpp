#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

int WIDTH = 1920;
int HEIGHT = 1013;
const char* TITLE = "Hell-Slayer";
GLFWwindow* window = nullptr;
float dt = 0.0f;
float lastFrame = 0.0f;

bool IsKeyPressed(GLenum key){
  return glfwGetKey(window, key)==GLFW_PRESS;
}

bool IsKeyReleased(GLenum key){
  return glfwGetKey(window, key)==GLFW_RELEASE;
}

bool IsMousePressed(GLenum button){
  return glfwGetMouseButton(window, button)==GLFW_PRESS;
}

bool IsMouseReleased(GLenum button){
  return glfwGetMouseButton(window, button)==GLFW_RELEASE;
}

class Camera{
private:
  float mSpeed = 5.0f;
  float mSensitivity = 0.1f;
  float mYaw = -90.0f;
  float mPitch = 0.0f;
  float mFov = 45.0f;
  float lastX = WIDTH/2.0f;
  float lastY = HEIGHT/2.0f;
  bool mFirstMouse = true;

  glm::vec3 mPosition = glm::vec3(0.0f,1.5f,3.0f);
  glm::vec3 mFront = glm::vec3(0.0f,0.0f,-1.0f);
  glm::vec3 mUp = glm::vec3(0.0f,1.0f,0.0f);
  
  glm::mat4 mView = glm::mat4(1.0f);
  glm::mat4 mProjection = glm::mat4(1.0f);
  float mNearPlane = 0.1f;
  float mFarPlane = 1000.0f;
  
  void UpdateMovement(float dt){
    glm::vec3 front = glm::vec3(mFront.x, 0.0f, mFront.z);

    if(IsKeyPressed(GLFW_KEY_W)) mPosition += mSpeed * dt * front;
    if(IsKeyPressed(GLFW_KEY_S)) mPosition -= mSpeed * dt * front;
    if(IsKeyPressed(GLFW_KEY_A)) mPosition -= mSpeed * dt * glm::normalize(glm::cross(front, mUp));
    if(IsKeyPressed(GLFW_KEY_D)) mPosition += mSpeed * dt * glm::normalize(glm::cross(front, mUp));
  }

public:
  Camera()=default;
  ~Camera()=default;
  
  const glm::mat4& GetViewMatrix() const {return mView;}
  const glm::mat4& GetProjectionMatrix() const {return mProjection;}

  void UpdateCamera(float width, float height, float dt){
    mView = glm::lookAt(mPosition, mPosition + mFront, mUp);
    mProjection = glm::perspective(glm::radians(mFov), width/height, mNearPlane, mFarPlane);
    UpdateMovement(dt);
  }

  void UpdateMouse(float xpos, float ypox){
    if(mFirstMouse){
      lastX = xpos;
      lastY = ypox;
      mFirstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypox;
    xoffset *= mSensitivity;
    yoffset *= mSensitivity;

    lastX = xpos;
    lastY = ypox;

    mYaw += xoffset;
    mPitch += yoffset;

    if(mPitch < -89.0f) mPitch = -89.0f;
    if(mPitch > 89.0f) mPitch = 89.0f;

    glm::vec3 direction;
    direction.x = glm::cos(glm::radians(mYaw)) * glm::cos(glm::radians(mPitch));
    direction.y = glm::sin(glm::radians(mPitch));
    direction.z = glm::cos(glm::radians(mPitch)) * glm::sin(glm::radians(mYaw));

    mFront = glm::normalize(direction);
  }
};

class Shader{
private:
  unsigned int mId;

  std::string LoadFile(const std::string& path){
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
    catch(const std::ifstream::failure& e){
      std::cerr<<"ERROR: Reading file -> "<<e.what()<<std::endl;
      exit(1);
    }
    return code;
  }
  
  unsigned int CompileShader(const std::string& srcCode, bool isVert){
    const char* code = srcCode.c_str();
    int success;
    char infoLog[512];
    unsigned int shader = isVert? glCreateShader(GL_VERTEX_SHADER) : glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(shader, 1, &code, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success){
      glGetShaderInfoLog(shader, 512, NULL, infoLog);
      std::cerr<<"ERROR: Compiling "<<(isVert? "Vert" : "Frag")<<" shader -> "<<infoLog<<std::endl;
      exit(1);
    }
    return shader;
  }

  void CreateShaderProgram(unsigned int& vert, unsigned int& frag){
    mId = glCreateProgram();
    glAttachShader(mId, vert);
    glAttachShader(mId, frag);
    glLinkProgram(mId);

    int success;
    char infoLog[512];
    glGetProgramiv(mId, GL_LINK_STATUS, &success);
    if(!success){
      glGetProgramInfoLog(mId, 512, NULL, infoLog);
      std::cerr<<"ERROR: Linking shader program -> "<<infoLog<<std::endl;
      exit(1);
    }
    
    glDeleteShader(vert);
    glDeleteShader(frag);

    std::cout<<"Shader program created successfully!"<<std::endl;
  }

public:
  Shader(const std::string& vertPath, const std::string& fragPath){
    std::string vCode = LoadFile(vertPath);
    std::string fCode = LoadFile(fragPath);
    unsigned int vert = CompileShader(vCode, true);
    unsigned int frag = CompileShader(fCode, false);
    CreateShaderProgram(vert, frag);
  }

  ~Shader(){
    glDeleteProgram(mId);
  }

  void Use(){
    glUseProgram(mId);
  }

  template <typename T>
  void SetValue(const std::string& name, const T& val){
    const unsigned int loc = glGetUniformLocation(mId, name.c_str());

    if constexpr(std::is_same_v<T,int>) glUniform1i(loc, val);
    else if constexpr(std::is_same_v<T,bool>) glUniform1i(loc, (int)val);
    else if constexpr(std::is_same_v<T,float>) glUniform1f(loc, val);
    else if constexpr(std::is_same_v<T,glm::vec2>) glUniform2fv(loc, 1, glm::value_ptr(val));
    else if constexpr(std::is_same_v<T,glm::vec3>) glUniform3fv(loc, 1, glm::value_ptr(val));
    else if constexpr(std::is_same_v<T,glm::mat4>) glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(val));
  }
};

class VBO{
private:
  unsigned int mId;

public:
  VBO(){
    glGenBuffers(1, &mId);
  }

  ~VBO(){
    glDeleteBuffers(1, &mId);
  }

  void Bind(){
    glBindBuffer(GL_ARRAY_BUFFER, mId);
  }

  void Unbind(){
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  void AllocateAndFill(size_t size, const void* data, GLenum usage){
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
  }

  void Allocate(size_t size, GLenum usage){
    glBufferData(GL_ARRAY_BUFFER, size, NULL, usage);
  }

  void Fill(size_t size, size_t offset, const void* data){
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
  }
};

class VAO{
private:
  unsigned int mId;

public:
  VAO(){
    glGenVertexArrays(1, &mId);
  }

  ~VAO(){
    glDeleteVertexArrays(1, &mId);
  }

  void Bind(){
    glBindVertexArray(mId);
  }

  void Unbind(){
    glBindVertexArray(0);
  }

  void SetAttrib(int loc, int nr, size_t stride, size_t offset){
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, nr, GL_FLOAT, GL_FALSE, stride, (void*)offset);
  }
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
  glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos){
  Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
  if(camera){
    camera->UpdateMouse(xpos, ypos);
  }
}

void ProcessInput(){
  if(IsKeyPressed(GLFW_KEY_ESCAPE)){
    glfwSetWindowShouldClose(window, true);
  }

  if(IsKeyPressed(GLFW_KEY_T)){
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  if(IsKeyPressed(GLFW_KEY_Y)){
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
}

void UpdateWindow(){
  glfwGetWindowSize(window, &WIDTH, &HEIGHT);
}

void UpdateTime(){
  float currentFrame = (float)glfwGetTime();
  dt = currentFrame - lastFrame;
  lastFrame = currentFrame;
}

int main(int argc, char* argv[]){
  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, NULL, NULL);
  glfwMakeContextCurrent(window);

  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  
  float vertices[] = {
    -1.0f,-1.0f,  0.0f,0.0f,
    1.0f,-1.0f,   1.0f,0.0f,
    1.0f,1.0f,    1.0f,1.0f,

    1.0f,1.0f,    1.0f,1.0f,
    -1.0f,1.0f,   0.0f,1.0f,
    -1.0f,-1.0f,  0.0f,0.0f
  };
  
  VBO vbo;
  VAO vao;
  vao.Bind();
  vbo.Bind();
  vbo.AllocateAndFill(sizeof(vertices), vertices, GL_STATIC_DRAW);
  vao.SetAttrib(0, 2, 4*sizeof(float), 0);
  vao.SetAttrib(1, 2, 4*sizeof(float), 2*sizeof(float));
  vao.Unbind();
  
  Shader shader("../vert.glsl", "../frag.glsl");
  
  Camera camera;
  glfwSetWindowUserPointer(window, &camera);

  glEnable(GL_DEPTH_TEST);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  while(!glfwWindowShouldClose(window)){
    glfwPollEvents();
    UpdateWindow();
    UpdateTime();
    ProcessInput();
    camera.UpdateCamera(WIDTH, HEIGHT, dt);

    glClearColor(0.0f,0.0f,0.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = camera.GetProjectionMatrix();

    shader.Use();
    shader.SetValue("model", model);
    shader.SetValue("view", view);
    shader.SetValue("projection", projection);
    vao.Bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    vao.Unbind();

    glfwSwapBuffers(window);
  }

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
