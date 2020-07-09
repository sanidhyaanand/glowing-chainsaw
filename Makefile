OPENGLLIB = -lGL
GLEWLIB = -lGLEW
GLFWLIB = -lglfw
LIBS=$(OPENGLLIB) $(GLEWLIB) $(GLFWLIB)
LDFLAGS=-L/C:/Users/Sanidhya/Desktop/OpenGL_SOC/lib
CPPFLAGS=-I/C:/Users/Sanidhya/Desktop/OpenGL_SoC/include

BIN1=container_to_play_with
BIN2=normal_light_twoboxes
BIN3=Problem1
BIN4=Problem2
BIN5=Problem3
BIN6=SOC Project
BIN7=spotlight_implemented

SRCS1=container_to_play_with.cpp
SRCS2=normal_light_twoboxes.cpp
SRCS3=Problem1.cpp
SRCS4=Problem2.cpp
SRCS5=Problem3.cpp
SRCS6=SOC Project.cpp
SRCS7=spotlight_implemented.cpp utility/skybox.cpp
INCLUDES=utility/camera.h utility/shader.h utility/skybox.h utility/stb_image.h

all: $(BIN1) $(BIN2) $(BIN3) $(BIN4) $(BIN5) $(BIN6) $(BIN7)
$(BIN1): $(SRCS1) $(INCLUDES)
	g++ $(CPPFLAGS) $(SRCS1) -o $(BIN1) $(LDFLAGS) $(LIBS)
$(BIN2): $(SRCS2) $(INCLUDES)
	g++ $(CPPFLAGS) $(SRCS2) -o $(BIN2) $(LDFLAGS) $(LIBS)
$(BIN3): $(SRCS3) $(INCLUDES)
	g++ $(CPPFLAGS) $(SRCS3) -o $(BIN3) $(LDFLAGS) $(LIBS)
$(BIN4): $(SRCS4) $(INCLUDES)
	g++ $(CPPFLAGS) $(SRCS4) -o $(BIN4) $(LDFLAGS) $(LIBS)
$(BIN5): $(SRCS5) $(INCLUDES)
	g++ $(CPPFLAGS) $(SRCS5) -o $(BIN5) $(LDFLAGS) $(LIBS)
$(BIN6): $(SRCS6) $(INCLUDES)
	g++ $(CPPFLAGS) $(SRCS6) -o $(BIN6) $(LDFLAGS) $(LIBS)
$(BIN7): $(SRCS7) $(INCLUDES)
	g++ $(CPPFLAGS) $(SRCS7) -o $(BIN7) $(LDFLAGS) $(LIBS)
clean:
	rm -f *~ *.o $(BIN1) $(BIN2) $(BIN3) $(BIN4) $(BIN5) $(BIN6) $(BIN7)