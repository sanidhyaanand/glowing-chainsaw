#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "stb_image.h"

class Texture
{
public:
	unsigned int loadTexture(char const* path);
};