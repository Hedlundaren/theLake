#pragma once

#include <iostream>
#ifdef _WIN32
#include "GL/glew.h"
#endif

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class DisplayWindow
{
public:
	DisplayWindow(GLFWwindow* &window, const unsigned width, const unsigned height);
	~DisplayWindow();
	void initFrame(glm::vec3 clear_color);


};

