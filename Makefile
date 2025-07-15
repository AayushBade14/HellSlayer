#compiler
CXX = g++

#libs 
LIBS = -lglfw -lGL -lassimp

#dirs
BUILD = ./Build
INCLUDE = ./Include/ThirdParty/

#flags
CXXFLAGS = -Wall -g -I$(INCLUDE)

#objs
OBJS = ./Main.o \
			 ./Include/Shader/Shader.o \
			 ./Include/Common/Common.o \
			 ./Include/Mesh/Mesh.o \
			 ./Include/Model/Model.o \
			 ./Include/ThirdParty/STB_IMAGE/stb_image.o \
			 ./Include/ThirdParty/glad/glad.o

#target
TARGET = $(BUILD)/hell

#all-rule
all: $(TARGET)

#linking
$(TARGET) : $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LIBS) $(CXXFLAGS)

#compiling
%.o : %.cpp
	$(CXX) -c $< -o $@ $(LIBS) $(CXXFLAGS)

#clean-rule
.PHONY = clean
clean:
	rm -rf $(OBJS) $(TARGET)
