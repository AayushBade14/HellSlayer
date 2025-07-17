#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "./Include/Shader/Shader.hpp"
#include "./Include/Model/Model.hpp"
#include "./Include/Animation/Animation.hpp"
#include "./Include/Animator/Animator.hpp"

#define WIDTH 1920.0f
#define HEIGHT 1040.0f
#define TITLE "HellSlayer"

glm::vec3 cameraPos = {0.0f,0.0f,3.0f};
glm::vec3 cameraFront = {0.0f,0.0f,-1.0f};
glm::vec3 cameraUp = {0.0f,1.0f,0.0f};

float cameraSpeed = 5.0f;
float sensitivity = 0.1f;
bool first_mouse = true;

float fov = 45.0f;
float yaw = -90.0f;
float pitch = 0.0f;

float lastX = 0.0f;
float lastY = 0.0f;

float dt = 0.0f;
float lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow* window,int width,int height){
  glViewport(0,0,width,height);
}

void mouse_callback(GLFWwindow* window,double xpos,double ypos){
  float Xpos = (float)xpos;
  float Ypos = (float)ypos;
  if(first_mouse){
    lastX = Xpos;
    lastY = Ypos;
    first_mouse = false;
  }

  float xoffset = Xpos - lastX;
  float yoffset = lastY - Ypos;

  lastX = Xpos;
  lastY = Ypos;

  xoffset *= sensitivity;
  yoffset *= sensitivity;

  yaw += xoffset;
  pitch += yoffset;

  if(pitch < -89.0f)
    pitch = 89.0f;
  if(pitch > 89.0f)
    pitch = 89.0f;

  glm::vec3 direction;
  direction.x = glm::cos(glm::radians(pitch)) * glm::cos(glm::radians(yaw));
  direction.y = glm::sin(glm::radians(pitch));
  direction.z = glm::cos(glm::radians(pitch)) * glm::sin(glm::radians(yaw));

  cameraFront = glm::normalize(direction);
}

void scroll_callback(GLFWwindow* window,double xoffset,double yoffset){
  fov -= (float)yoffset;

  if(fov < 1.0f)
    fov = 1.0f;
  if(fov > 45.0f)
    fov = 45.0f;
}

void ProcessInput(GLFWwindow* window){
  if(glfwGetKey(window,GLFW_KEY_ESCAPE)==GLFW_PRESS)
    glfwSetWindowShouldClose(window,true);

  if(glfwGetKey(window,GLFW_KEY_T)==GLFW_PRESS)
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
  if(glfwGetKey(window,GLFW_KEY_Y)==GLFW_PRESS)
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

  if(glfwGetKey(window,GLFW_KEY_W)==GLFW_PRESS)
    cameraPos += cameraSpeed * dt * cameraFront;
  if(glfwGetKey(window,GLFW_KEY_S)==GLFW_PRESS)
    cameraPos -= cameraSpeed * dt * cameraFront;
  if(glfwGetKey(window,GLFW_KEY_A)==GLFW_PRESS)
    cameraPos -= cameraSpeed * dt * glm::normalize(glm::cross(cameraFront,cameraUp));
  if(glfwGetKey(window,GLFW_KEY_D)==GLFW_PRESS)
    cameraPos += cameraSpeed * dt * glm::normalize(glm::cross(cameraFront,cameraUp));
}

int main(void){
  
  if(glfwInit()<0){
    std::cerr<<"ERROR: GLFW_INIT!"<<std::endl;
    return -1;
  }
  
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,6);
  glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(WIDTH,
                                        HEIGHT,
                                        TITLE,
                                        NULL,
                                        NULL);

  if(!window){
    std::cerr<<"ERROR: Creating window!"<<std::endl;
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
    std::cerr<<"ERROR: GLAD_INIT!"<<std::endl;
    glfwTerminate();
    glfwDestroyWindow(window);
    return -1;
  }
    
  Shader shader("./Assets/Shaders/vert.glsl","./Assets/Shaders/frag.glsl");
  
  Model player("./Assets/Models/surgeonModel/Ch16_nonPBR.dae");
  Model eve("./Assets/Models/eve/Eve By J.Gonzales.dae");
  
  Animation walkingAnimation("./Assets/Animations/Walking/Walking.dae",player);
  Animator animator(walkingAnimation);

  Model ground("./Assets/Models/groundTiled/scene.gltf");

  glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
  glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);
  glfwSetCursorPosCallback(window,mouse_callback);
  glfwSetScrollCallback(window,scroll_callback);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  
  std::cout<<"NOW STARTING!!"<<std::endl;
  
  while(!glfwWindowShouldClose(window)){
    float currentFrame = (float)glfwGetTime();
    dt = currentFrame - lastFrame;
    lastFrame = currentFrame;
    
    ProcessInput(window);

    glClearColor(0.0f,0.0f,0.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    animator.UpdateAnimation(dt);    
    
    shader.Use();
    
    std::vector<glm::mat4> transforms = animator.GetSkinnedMatrix();
    for(unsigned int i = 0; i < transforms.size(); i++){
      shader.SetValue("finalBonesMatrices["+std::to_string(i)+"]",transforms[i]);
    }

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::lookAt(cameraPos,cameraPos+cameraFront,cameraUp);
    glm::mat4 projection = glm::perspective(glm::radians(fov),WIDTH/HEIGHT,0.1f,1000.0f);
    
    shader.SetValue("model",model);
    shader.SetValue("view",view);
    shader.SetValue("projection",projection);

    player.Draw(shader);
    
    model = glm::mat4(1.0f);
    model = glm::translate(model,glm::vec3(3.0f,0.0f,0.0f));
    model = glm::scale(model,glm::vec3(0.01f));

    shader.SetValue("model",model);

    eve.Draw(shader);

    model = glm::mat4(1.0f);
    model = glm::rotate(model,glm::radians(-90.0f),glm::vec3(1.0f,0.0f,0.0f));

    shader.SetValue("model",model);

    ground.Draw(shader);

    glfwPollEvents(); 
    glfwSwapBuffers(window);
  }

  glfwTerminate();
  glfwDestroyWindow(window);

  return 0;
}
