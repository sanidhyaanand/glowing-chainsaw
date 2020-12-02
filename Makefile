OPENGLLIB = -lGL
GLEWLIB = -lGLEW
GLFWLIB = -lglfw3
LIBS = $(OPENGLLIB) $(GLEWLIB) $(GLFWLIB)
LDFLAGS=-LD:/lib
CPPFLAGS=-ID:/include

BIN1 = Earth_textured
BIN2 = SOC_Project
BIN3 = Source
BIN4 = Sphere_Solid_Color

SRCS1 = Earth_textured.cpp
SRCS2 = SOC_Project.cpp
SRCS3 = Source.cpp
SRCS4 = Sphere_Solid_Color.cpp

SRCS5 = utility/skybox.cpp utility/Texture.cpp

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