#pragma once

#ifdef _WIN32
#include "GL/glew.h"
#endif

#include <GLFW/glfw3.h>

class Quad
{
public:
	Quad();
	~Quad();
	void draw();

private:
	GLuint quadVAO, quadVBO;

};

