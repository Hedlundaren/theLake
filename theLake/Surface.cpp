#include "Surface.h"
#include <iostream>


Surface::Surface() : res_x(10), res_z(10), width(100), length(100)
{
	float resolution = 200.0f; // resolution = 10 --> 10x10
	create_plane();
}

Surface::Surface(const int rx, const int rz, int w, int l) : res_x(rx), res_z(rz), width(w), length(l)
{
	create_plane();
}

Surface::~Surface()
{
}

// Method functions
void Surface::draw(GLFWwindow *window) {
	glLineWidth(2.0);

	//Draw Object
	glEnableClientState(GL_VERTEX_ARRAY);

	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBindVertexArray(VAO);
	
	glDrawElements(
		GL_TRIANGLES,      // mode
		indices.size(),    // count
		GL_UNSIGNED_INT,   // type
		(void*)0           // element array buffer offset
	);
	
	glBindVertexArray(0);

}

void Surface::create_plane() {
	float height = 0.0f;

	float leap_x = width / res_x;
	float offset_x = width / 2.0f - leap_x / 2.0f; // center

	float leap_z = length / res_z;
	float offset_z = length / 2.0f - leap_z / 2.0f; // center


	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	for (float z = 0; z < res_z; z+=1.0f) {
		for (float x = 0; x < res_x; x += 1.0f) {
			vertices.push_back(glm::vec3(x*leap_x - offset_x, height, z*leap_z - offset_z));
			uvs.push_back(glm::vec2(x/res_x, z/res_z));
		}
	}

	for (int i = 0; i < res_x - 1; i++) {
		for (int j = 0; j < res_z - 1; j++) {

			// Triangle 1
			indices.push_back(i + j*res_x);
			indices.push_back((i + 1) + j*res_x);
			indices.push_back(i + (j + 1)*res_x);

			// Triangle 2
			indices.push_back((i + 1) + j*res_x);
			indices.push_back((i + 1) + (j + 1)*res_x);
			indices.push_back((i)+(j + 1)*res_x);

		}
	}

	// Model vertices
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float) * 3, &vertices[0], GL_STATIC_DRAW); // Give our vertices to OpenGL.
	glEnableVertexAttribArray(0); // 1rst attribute buffer : vertices
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		sizeof(float) * 3,  // stride
		(void*)0			// array buffer offset
	);

	// Model uv coords
	glGenBuffers(1, &UVBO);
	glBindBuffer(GL_ARRAY_BUFFER, UVBO);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(float) * 2, &uvs[0], GL_STATIC_DRAW); // Give our uvs to OpenGL.
	glEnableVertexAttribArray(1); // 2rst attribute buffer : uvs
	glVertexAttribPointer(
		1,                  // attribute 1. No particular reason for 0, but must match the layout in the shader.
		2,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		sizeof(float) * 2,  // stride
		(void*)0			// array buffer offset
	);

	// Model indices
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
}