#pragma once

#include <glm/glm.hpp>
#include <iostream>
#include <vector>

#include <stdio.h>
#include <string>
#include <cstring>

#ifdef _WIN32
#include "GL/glew.h"
#endif


class OBJModel
{
public:
	OBJModel();
	OBJModel(const char *path);
	~OBJModel();

	void draw();

protected:
	bool loadOBJ(const char *path);

private:
	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;

	// Read our .obj file
	std::vector < glm::vec3 > vertices;
	std::vector<unsigned int> indices;
	std::vector < glm::vec2 > uvs;
	std::vector < glm::vec3 > normals;

	GLuint vertexbuffer;
	GLuint uvbuffer;

	GLuint VAO, VBO, IBO, normalbuffer;



};

