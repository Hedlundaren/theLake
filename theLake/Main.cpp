#include <iostream>
#include <algorithm>
#include <vector>

#ifdef _WIN32
#include "GL/glew.h"
#endif

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <vector>
#include "DisplayWindow.h"
#include "ShaderProgram.h"
#include "Rotator.h"

#include "Surface.h";
#include "Framebuffer.h";

#define HEIGHT 900
#define WIDTH 900

using namespace std;

int main() {

	cout << "Welcome.\n";
	GLFWwindow* window = nullptr;
	DisplayWindow myWindow = DisplayWindow(window, WIDTH, HEIGHT);

	ShaderProgram post_program("posts.vert", "", "", "", "posts.frag");
	ShaderProgram mountain_program("mountain.vert", "", "", "", "mountain.frag");
	ShaderProgram mountain_mirror_program("mountainref.vert", "", "", "", "mountainref.frag");
	ShaderProgram water_program("water.vert", "", "", "", "water.frag");
	ShaderProgram tequila_program("tequila.vert", "", "", "", "tequila.frag");
	glUseProgram(0);

	MouseRotator rotator;
	rotator.init(window);

	Surface water(4, 4, 250, 500);
	Surface mountain(30, 60, 200, 400);


	GLfloat quadVertices[] = {   // Vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
								 // Positions   // TexCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
	};

	GLuint quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
	glBindVertexArray(0);

	double time;

	Framebuffer framebuffer[6] = { { WIDTH, HEIGHT },{ WIDTH, HEIGHT },{ WIDTH, HEIGHT },{ WIDTH, HEIGHT },{ WIDTH, HEIGHT },{ WIDTH, HEIGHT } };
	Framebuffer refractionBuffer(WIDTH, HEIGHT);
	Framebuffer reflectionBuffer(WIDTH, HEIGHT);
	Framebuffer preScreenBuffer(WIDTH, HEIGHT);

	do {
		// Setup frame
		time = glfwGetTime();
		rotator.poll(window);
		glm::vec3 clear_color = glm::vec3(1.0f, 0.6f, 0.4f);
		clear_color = glm::vec3(0.5f, 0.7f, 0.9f); // day
		clear_color = glm::vec3(1.0f, 0.6f, 0.4f); // eve
		myWindow.initFrame(clear_color);

		int k = 0;
		if (glfwGetKey(window, GLFW_KEY_0)) k = 0;
		if (glfwGetKey(window, GLFW_KEY_F)) k = 1;
		if (glfwGetKey(window, GLFW_KEY_2)) k = 2;
		if (glfwGetKey(window, GLFW_KEY_3)) k = 3;
		if (glfwGetKey(window, GLFW_KEY_4)) k = 4;
		if (glfwGetKey(window, GLFW_KEY_5)) k = 5;

		// =========================
		// Refraction render pass 
		// =========================
		refractionBuffer.bindBuffer(); // Render to framebuffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // We're not using stencil buffer now
		glEnable(GL_DEPTH_TEST);

		mountain_program();
		mountain_program.updateCommonUniforms(rotator, WIDTH, HEIGHT, time, clear_color);
		mountain.draw(window);

		// =========================
		// Reflection render pass 
		// =========================
		reflectionBuffer.bindBuffer(); // Render to framebuffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // We're not using stencil buffer now
		glEnable(GL_DEPTH_TEST);

		mountain_mirror_program();
		mountain_mirror_program.updateCommonUniforms(rotator, WIDTH, HEIGHT, time, clear_color);
		mountain.draw(window);

		// =========================
		// Mountain render pass 
		// =========================
		if (glfwGetKey(window, GLFW_KEY_W)) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); else glPolygonMode(GL_FRONT_AND_BACK, GL_TRIANGLES);

		//preScreenBuffer.bindBuffer();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // We're not using stencil buffer now
		glEnable(GL_DEPTH_TEST);
		mountain_program();
		mountain_program.updateCommonUniforms(rotator, WIDTH, HEIGHT, time, clear_color);
		mountain.draw(window);

		// =========================
		// Water render pass 
		// =========================
		//preScreenBuffer.bindBuffer();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		if (glfwGetKey(window, GLFW_KEY_T)) {
			tequila_program();
			tequila_program.updateCommonUniforms(rotator, WIDTH, HEIGHT, time, clear_color);
		}
		else {
			water_program();
			water_program.updateCommonUniforms(rotator, WIDTH, HEIGHT, time, clear_color);
		}

		glEnable(GL_TEXTURE_2D);
		GLint texLoc;
		
		texLoc = glGetUniformLocation(water_program, "reflectionTexture");
		glUniform1i(texLoc, 0);

		texLoc = glGetUniformLocation(water_program, "refractionTexture");
		glUniform1i(texLoc, 1);

		glActiveTexture(GL_TEXTURE0);
		reflectionBuffer.bindTexture();

		glActiveTexture(GL_TEXTURE1);
		refractionBuffer.bindTexture();

		water.draw(window);

		// =========================
		// Post render pass 
		// =========================
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//post_program();
		//glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		//glBindVertexArray(quadVAO);
		//
		//texLoc = glGetUniformLocation(post_program, "screenTexture");
		//glUniform1i(texLoc, 0);

		//glActiveTexture(GL_TEXTURE0);
		//glEnable(GL_TEXTURE_2D);
		//preScreenBuffer.bindTexture();
		//glPolygonMode(GL_FRONT_AND_BACK, GL_TRIANGLES);

		//glDrawArrays(GL_TRIANGLES, 0, 6);
		//glBindVertexArray(0);

		// Finish frame
		glfwSwapInterval(2);
		glfwSwapBuffers(window);
		glfwPollEvents();

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	glDisableVertexAttribArray(0);

	return 0;
}


