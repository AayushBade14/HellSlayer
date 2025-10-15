#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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
  glm::vec3 GetPosition() {return mPosition;}
  glm::vec3 GetFront() {return mFront;}
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
  
  VBO(const VBO&) = delete;
  VBO& operator=(const VBO&) = delete;
  
  VBO(VBO&& other) noexcept : mId(other.mId) {
    other.mId = 0;
  }
  
  VBO& operator=(VBO&& other) noexcept {
    if(this != &other){
      glDeleteBuffers(1, &mId);
      mId = other.mId;
      other.mId = 0;
    }
    return *this;
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

class EBO{
private:
  unsigned int mId;

public:
  EBO(){
    glGenBuffers(1, &mId);
  }

  ~EBO(){
    glDeleteBuffers(1, &mId);
  }
  
  EBO(const EBO&) = delete;
  EBO& operator=(const EBO&) = delete;
  
  EBO(EBO&& other) noexcept : mId(other.mId) {
    other.mId = 0;
  }
  
  EBO& operator=(EBO&& other) noexcept {
    if(this != &other){
      glDeleteBuffers(1, &mId);
      mId = other.mId;
      other.mId = 0;
    }
    return *this;
  }

  void Bind(){
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mId);
  }

  void Unbind(){
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }

  void AllocateAndFill(size_t size, const void* data, GLenum usage){
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
  }

  void Allocate(size_t size, GLenum usage){
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, NULL, usage);
  }

  void Fill(size_t size, size_t offset, const void* data){
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data);
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

  VAO(const VAO&) = delete;
  VAO& operator=(const VAO&) = delete;
  
  VAO(VAO&& other) noexcept : mId(other.mId) {
    other.mId = 0;
  }
  
  VAO& operator=(VAO&& other) noexcept {
    if(this != &other){
      glDeleteVertexArrays(1, &mId);
      mId = other.mId;
      other.mId = 0;
    }
    return *this;
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

struct Vertex{
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 texcoord; 
};

struct Transform{
  glm::vec3 position;
  glm::quat rotation;
  glm::vec3 scale;

  glm::mat4 ToMatrix(){
    glm::mat4 T = glm::translate(glm::mat4(1.0f),position);
    glm::mat4 R = glm::toMat4(rotation);
    glm::mat4 S = glm::scale(glm::mat4(1.0f),scale);
    return T*R*S;
  }
};

struct Skeleton{
  std::vector<int> parentIndices;
  std::vector<glm::mat4> offsetMatrix;
};

struct Keyframe{
  float time;
  glm::vec3 position;
  glm::quat rotation;
  glm::vec3 scale;
};

struct BoneTrack{
  std::vector<Keyframe> keyframes;
};

struct AnimationClip{
  std::string name;
  float duration;
  std::vector<BoneTrack> tracks;
};

std::unordered_map<std::string, int> boneToIndexMap;


class Mesh{
private:
  std::vector<Vertex> mVertices;
  std::vector<unsigned int> mIndices;
  
  VBO mVbo;
  VAO mVao;
  EBO mEbo;
  
  void SetupMesh(){
    mVao.Bind();
    
    mVbo.Bind();
    mVbo.AllocateAndFill(mVertices.size() * sizeof(Vertex), mVertices.data(), GL_STATIC_DRAW);
    mEbo.Bind();
    mEbo.AllocateAndFill(mIndices.size() * sizeof(unsigned int), mIndices.data(), GL_STATIC_DRAW);

    mVao.SetAttrib(0, 3, sizeof(Vertex), offsetof(Vertex, position));
    mVao.SetAttrib(1, 3, sizeof(Vertex), offsetof(Vertex, normal));
    mVao.SetAttrib(2, 2, sizeof(Vertex), offsetof(Vertex, texcoord));

    mVao.Unbind();
  }
  
public:
  Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices){
    this->mVertices = vertices;
    this->mIndices = indices;

    SetupMesh();
  }
  
  ~Mesh()=default;
  
  Mesh(const Mesh&) = delete;
  Mesh& operator=(const Mesh&) = delete;

  Mesh(Mesh&& other) noexcept 
    : mVertices(std::move(other.mVertices)),
      mIndices(std::move(other.mIndices)),
      mVbo(std::move(other.mVbo)),
      mVao(std::move(other.mVao)),
      mEbo(std::move(other.mEbo))
  {
  }

  Mesh& operator=(Mesh&& other) noexcept {
    if(this != &other){
      mVertices = std::move(other.mVertices);
      mIndices = std::move(other.mIndices);
      mVbo = std::move(other.mVbo);
      mVao = std::move(other.mVao);
      mEbo = std::move(other.mEbo);
    }
    return *this;
  }

  std::vector<Vertex>& GetVertices() {return mVertices;}
  std::vector<unsigned int>& GetIndices() {return mIndices;}

  void Draw(Shader& shader){
    mVao.Bind();
    glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
    mVao.Unbind();
  }
};

class Model{
private:
  std::vector<Mesh> mMeshes;
  
  Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene){
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    for(unsigned int i = 0; i < mesh->mNumVertices; i++){
      Vertex vertex;
      vertex.position.x = mesh->mVertices[i].x;
      vertex.position.y = mesh->mVertices[i].y;
      vertex.position.z = mesh->mVertices[i].z;
      if(mesh->HasNormals()){
        vertex.normal.x = mesh->mNormals[i].x;
        vertex.normal.y = mesh->mNormals[i].y;
        vertex.normal.z = mesh->mNormals[i].z;
      }

      if(mesh->mTextureCoords[0]){
        vertex.texcoord.x = mesh->mTextureCoords[0][i].x;
        vertex.texcoord.y = mesh->mTextureCoords[0][i].y;
      }
      else{
        vertex.texcoord = glm::vec2(0.0f);
      }
      vertices.push_back(vertex);
    }

    for(unsigned int i = 0; i < mesh->mNumFaces; i++){
      aiFace face = mesh->mFaces[i];
      for(unsigned int j = 0; j < face.mNumIndices; j++){
        indices.push_back(face.mIndices[j]);
      }
    }

    return Mesh(vertices, indices);
  }
  
  void ProcessNode(aiNode* node, const aiScene* scene){
    for(unsigned int i = 0; i < node->mNumMeshes; i++){
      aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
      mMeshes.push_back(ProcessMesh(mesh, scene));
    }

    for(unsigned int i = 0; i < node->mNumChildren; i++){
      ProcessNode(node->mChildren[i], scene);
    }
  }
  
  void LoadModel(const std::string& path){
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_GenNormals);
    if(!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE){
      std::cerr<<"ERROR: Loading model -> "<<path<<" REASON: "<<importer.GetErrorString()<<std::endl;
      exit(1);
    }
    ProcessNode(scene->mRootNode, scene);
  }
  
public:
  Model(const std::string& path){
    LoadModel(path);
    std::cout<<"Model Loaded successfully!"<<std::endl;
  }

  ~Model()=default;

  void Draw(Shader& shader){
    for(unsigned int i = 0; i < mMeshes.size(); i++){
      mMeshes[i].Draw(shader);
    }
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

struct Enemy{
  glm::vec3 position;
  glm::vec3 front;
};


int main(int argc, char* argv[]){
  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, NULL, NULL);
  glfwMakeContextCurrent(window);

  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  
  Shader shader("../vert.glsl", "../frag.glsl");
  
  Model monkey("../monkey_face.obj");
  Model terrain("../terrain.obj");

  Camera camera;
  glfwSetWindowUserPointer(window, &camera);
  
  std::vector<Enemy> enemies;
  Enemy enemy = {glm::vec3(5.0f), glm::vec3(0.0f,0.0f,1.0f)};
  enemies.push_back(enemy);

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
    model = glm::scale(model, glm::vec3(10.0f));
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = camera.GetProjectionMatrix();

    shader.Use();
    shader.SetValue("model", model);
    shader.SetValue("view", view);
    shader.SetValue("projection", projection);
    
    //monkey.Draw(shader);
    terrain.Draw(shader);
    
    /*for(unsigned int i = 0; i < enemies.size(); i++){
      glm::vec3 dir = camera.GetPosition() - enemies[i].position;
      float angle = glm::acos(glm::dot(dir,camera.GetFront()));
      enemies[i].position += dt * glm::normalize(dir);
      model = glm::mat4(1.0f);
      model = glm::translate(model, enemies[i].position);
      model = glm::rotate(model, angle, glm::vec3(0.0f,1.0f,0.0f));
      shader.SetValue("model",model);
      monkey.Draw(shader);
    }*/
    for (unsigned int i = 0; i < enemies.size(); i++) {
    // Direction from enemy to camera (normalized)
    glm::vec3 dir = glm::normalize(camera.GetPosition() - enemies[i].position);

    // Move enemy toward the camera
    enemies[i].position += dt * dir; // speed factor can be multiplied here if needed

    // Compute yaw angle to face the camera
    float yaw = glm::degrees(atan2(dir.z, dir.x));

    // Set up model matrix for enemy
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, enemies[i].position);
    
    // Rotate around Y-axis so enemy faces camera
    // Adjust +90 degrees if your model's default front is along X+
    model = glm::rotate(model, -glm::radians(yaw) + glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    // Send model matrix to shader and draw
    shader.SetValue("model", model);
    monkey.Draw(shader);
  }

    glfwSwapBuffers(window);
  }

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
