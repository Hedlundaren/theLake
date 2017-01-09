#pragma once


#ifdef _WIN32
#include "GL/glew.h"
#endif

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#define PI 3.1415926
const glm::vec3 RED = glm::vec3(1.0, 0.0, 0.0);


class Sphere
{
public:
	Sphere();
	Sphere(const int res_p, const int res_t, float r);
	~Sphere();

	// class methods
	void create_sphere();
	void draw();


private:
	const int res_phi, res_theta;
	float radius;
	GLuint sphereVAO, sphereVBO;

};

