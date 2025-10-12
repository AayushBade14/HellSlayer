cmake_minimum_required(VERSION 3.10)
project(
  hellslayer
  VERSION 2.0
  DESCRIPTION "Shooter game"
  LANGUAGES C CXX
)

include(FetchContent)
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.3.9
)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
  assimp
  GIT_REPOSITORY https://github.com/assimp/assimp.git
  GIT_TAG v5.4.3
)
FetchContent_MakeAvailable(assimp)

add_subdirectory(glm)

find_package(OpenGL REQUIRED)

add_library(
  glad
  STATIC
  ./glad/src/glad.c
)
target_include_directories(
  glad
  PUBLIC
  ./glad/include
)

add_executable(
  hellslayer
  main.cpp
)
target_link_libraries(
  hellslayer
  PUBLIC
  OpenGL::GL
  glm::glm
  glad
  glfw
  assimp
)
