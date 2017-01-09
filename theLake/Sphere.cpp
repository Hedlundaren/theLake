#include "Sphere.h"



Sphere::Sphere() : res_phi(10), res_theta(10), radius(1.0f)
{
	create_sphere();
}

Sphere::Sphere(const int res_p, const int res_t, float r) : res_phi(res_p), res_theta(res_t), radius(r) 
{
	create_sphere();
}

Sphere::~Sphere()
{

}

glm::mat4 rotationMatrix(glm::vec3 axis, float angle)
{
	axis = normalize(axis);
	float s = sin(angle);
	float c = cos(angle);
	float oc = 1.0 - c;

	return glm::mat4(oc * axis.x * axis.x + c, oc * axis.x * axis.y - axis.z * s, oc * axis.z * axis.x + axis.y * s, 0.0,
		oc * axis.x * axis.y + axis.z * s, oc * axis.y * axis.y + c, oc * axis.y * axis.z - axis.x * s, 0.0,
		oc * axis.z * axis.x - axis.y * s, oc * axis.y * axis.z + axis.x * s, oc * axis.z * axis.z + c, 0.0,
		0.0, 0.0, 0.0, 1.0);
}

void Sphere::make_face(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 face_color) {
	glm::vec3 face_normal = normalize(cross(b - a, c - a));

	glm::vec3 normal = face_normal;
	glm::vec3 in_color = face_color;

	vertices.push_back(a);
	vertices.push_back(b);
	vertices.push_back(c);

	indices.push_back(vertices.size() - 3);
	indices.push_back(vertices.size() - 2);
	indices.push_back(vertices.size() - 1);

	//gl_Position = P * MV * vec4(a, 1.0);
	//EmitVertex();

	//normal = face_normal;
	//in_color = face_color;
	//gl_Position = P * MV * vec4(b, 1.0);
	//EmitVertex();

	//normal = face_normal;
	//in_color = face_color;
	//gl_Position = P * MV * vec4(c, 1.0);
	//EmitVertex();
}

void Sphere::create_sphere() {



	glGenVertexArrays(1, &sphereVAO);
	glBindVertexArray(sphereVAO);

	glm::vec3 center = glm::vec3(0.0f, -10.0f, 0.0f);
	float phi_stepSize = 2.0 * PI / res_phi;
	float theta_stepSize = 2.0 / res_theta;

	// Top piece
	glm::vec3 top = radius * glm::vec3(0, 1, 0) - center;
	for (int j = 0; j < res_phi; j++) {

		float xz_offset = sqrt(1 - pow((1.0 - theta_stepSize), 2.0));
		glm::vec4 a_raw = glm::vec4(glm::vec3(xz_offset, 1 - theta_stepSize, 0), 1.0) * rotationMatrix(glm::vec3(0, 1, 0), phi_stepSize * j);
		glm::vec4 b_raw = glm::vec4(glm::vec3(xz_offset, 1 - theta_stepSize, 0), 1.0) * rotationMatrix(glm::vec3(0, 1, 0), phi_stepSize * (j + 1));

		glm::vec3 a = radius * glm::vec3(a_raw.x, a_raw.y, a_raw.z) - center;
		glm::vec3 b = radius * glm::vec3(b_raw.x, b_raw.y, b_raw.z) - center;

		make_face(top, b, a, RED);
	}

	// Middle piece
	for (int i = 1; i < res_theta - 1; i++) {

		for (int j = 0; j < res_phi; j++) {

			float xz_offset1 = sqrt(1.0 - pow((1.0 - theta_stepSize*i), 2.0));
			float xz_offset2 = sqrt(1.0 - pow((1.0 - theta_stepSize*(i + 1)), 2.0));

			glm::vec4 a_raw = glm::vec4(glm::vec3(xz_offset1, 1 - theta_stepSize*i, 0), 1.0) * rotationMatrix(glm::vec3(0, 1, 0), phi_stepSize * j);
			glm::vec4 b_raw = glm::vec4(glm::vec3(xz_offset1, 1 - theta_stepSize*i, 0), 1.0) * rotationMatrix(glm::vec3(0, 1, 0), phi_stepSize * (j + 1));

			glm::vec4 c_raw = glm::vec4(glm::vec3(xz_offset2, 1 - theta_stepSize*(i + 1), 0), 1.0) * rotationMatrix(glm::vec3(0, 1, 0), phi_stepSize * j);
			glm::vec4 d_raw = glm::vec4(glm::vec3(xz_offset2, 1 - theta_stepSize*(i + 1), 0), 1.0) * rotationMatrix(glm::vec3(0, 1, 0), phi_stepSize * (j + 1));

			glm::vec3 a = radius * glm::vec3(a_raw.x, a_raw.y, a_raw.z) - center;
			glm::vec3 b = radius * glm::vec3(b_raw.x, b_raw.y, b_raw.z) - center;
			glm::vec3 c = radius * glm::vec3(c_raw.x, c_raw.y, c_raw.z) - center;
			glm::vec3 d = radius * glm::vec3(d_raw.x, d_raw.y, d_raw.z) - center;

			make_face(a, b, d, RED);
			make_face(a, d, c, RED);
		}
	}

	// Bottom piece
	glm::vec3 bottom = radius * glm::normalize(glm::vec3(0, -1, 0)) - center;
	for (int j = 0; j < res_phi; j++) {

		float xz_offset = sqrt(1 - pow((1 - theta_stepSize), 2.0));

		glm::vec4 a_raw = glm::vec4(glm::vec3(xz_offset, -1 + theta_stepSize, 0), 1.0) * rotationMatrix(glm::vec3(0, 1, 0), phi_stepSize * j);
		glm::vec4 b_raw = glm::vec4(glm::vec3(xz_offset, -1 + theta_stepSize, 0), 1.0) * rotationMatrix(glm::vec3(0, 1, 0), phi_stepSize  * (j + 1));

		glm::vec3 a = radius * glm::vec3(a_raw.x, a_raw.y, a_raw.z) - center;
		glm::vec3 b = radius * glm::vec3(b_raw.x, b_raw.y, b_raw.z) - center;
		make_face(bottom, a, b, RED);
	}


	// Model vertices
	glGenBuffers(1, &sphereVBO);
	glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
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

	// Model indices
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);


}

void Sphere::draw() {
	glLineWidth(2.0);

	//Draw Object
	glEnableClientState(GL_VERTEX_ARRAY);

	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBindVertexArray(sphereVAO);

	glDrawElements(
		GL_LINES,      // mode
		indices.size(),    // count
		GL_UNSIGNED_INT,   // type
		(void*)0           // element array buffer offset
	);

	glBindVertexArray(0);

}
