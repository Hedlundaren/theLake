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
#include "Quad.h";
#include "Framebuffer.h";

#define HEIGHT 900
#define WIDTH 900

using namespace std;

int main() {

	cout << "Welcome.\n";
	GLFWwindow* window = nullptr;
	DisplayWindow myWindow = DisplayWindow(window, WIDTH, HEIGHT);

	ShaderProgram post_program("shaders/posts.vert", "", "", "", "shaders/posts.frag");
	ShaderProgram mountain_program("shaders/mountain.vert", "", "", "", "shaders/mountain.frag");
	ShaderProgram mountain_mirror_program("shaders/mountainref.vert", "", "", "", "shaders/mountainref.frag");
	ShaderProgram water_program("shaders/water.vert", "", "", "", "shaders/water.frag");
	ShaderProgram tequila_program("shaders/tequila.vert", "", "", "", "shaders/tequila.frag");
	glUseProgram(0);

	MouseRotator rotator;
	rotator.init(window);

	Surface water(4, 4, 250, 500);
	Surface mountain(100, 200, 200, 400);

	Quad quad;

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

		preScreenBuffer.bindBuffer();
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);


		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // We're not using stencil buffer now
		glEnable(GL_DEPTH_TEST);
		mountain_program();
		mountain_program.updateCommonUniforms(rotator, WIDTH, HEIGHT, time, clear_color);
		mountain.draw(window);

		// =========================
		// Water render pass 
		// =========================
		preScreenBuffer.bindBuffer();
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		post_program();

		texLoc = glGetUniformLocation(post_program, "screenTexture");
		glUniform1i(texLoc, 0);

		glActiveTexture(GL_TEXTURE0);
		glEnable(GL_TEXTURE_2D);
		preScreenBuffer.bindTexture();
		quad.draw();


		// Finish frame
		glfwSwapInterval(2);
		glfwSwapBuffers(window);
		glfwPollEvents();

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	glDisableVertexAttribArray(0);

	return 0;
}


