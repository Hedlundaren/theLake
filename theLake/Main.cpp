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

#include "DisplayWindow.h"
#include "ShaderProgram.h"
#include "Rotator.h"

#include "Surface.h";
#include "Quad.h";
#include "Sphere.h";
#include "Framebuffer.h";
#include "Texture.h";
#include "OBJModel.h";
#include "Sound.h";

#define HEIGHT 900
#define WIDTH 900

int main() {

	std::cout << "===================\n";
	std::cout << "===== theLake =====\n";
	std::cout << "===================\n";
	std::cout << " \n";

	GLFWwindow* window = nullptr;
	DisplayWindow myWindow = DisplayWindow(window, WIDTH, HEIGHT);

	ShaderProgram post_program("shaders/posts.vert", "", "", "", "shaders/posts.frag");
	ShaderProgram depth_program("shaders/depth.vert", "", "", "", "shaders/depth.frag");
	ShaderProgram mountain_depth_program("shaders/mountain.vert", "", "", "", "shaders/depth.frag");
	ShaderProgram mountain_program("shaders/mountain.vert", "", "", "", "shaders/mountain.frag");
	ShaderProgram mountain_refraction_program("shaders/mountainrefraction.vert", "", "", "", "shaders/mountainrefraction.frag");
	ShaderProgram mountain_mirror_program("shaders/mountainreflection.vert", "", "", "", "shaders/mountainreflection.frag");
	ShaderProgram water_program("shaders/water.vert", "", "", "", "shaders/water.frag");
	ShaderProgram tequila_program("shaders/tequila.vert", "", "", "", "shaders/tequila.frag");
	ShaderProgram sun_program("shaders/tequila.vert", "", "", "", "shaders/tequila.frag");
	ShaderProgram environment_program("shaders/environment.vert", "", "", "", "shaders/environment.frag");
	ShaderProgram environmentref_program("shaders/environment.vert", "", "", "", "shaders/environmentref.frag");

	glUseProgram(0);

	MouseRotator rotator;
	rotator.init(window);

	Surface water(4, 4, 250, 600);
	Surface mountain(100, 200, 200, 400);
	Sphere sun(52, 52, 7.0f);
	Sphere sphereMap(20, 20, 1800.0f);
	Quad quad;

	Texture texture("textures/albin.png");
	Texture sky_texture("textures/evesphere.jpg");
	Texture caustics_texture("textures/caustics.png");
	Texture earth_texture("textures/earth.jpg");

	double time;

	Framebuffer framebuffer[6] = { { WIDTH, HEIGHT },{ WIDTH, HEIGHT },{ WIDTH, HEIGHT },{ WIDTH, HEIGHT },{ WIDTH, HEIGHT },{ WIDTH, HEIGHT } };
	Framebuffer depthBuffer(WIDTH, HEIGHT);
	Framebuffer cloudBuffer(WIDTH, HEIGHT);
	Framebuffer refractionBuffer(WIDTH, HEIGHT);
	Framebuffer reflectionBuffer(WIDTH, HEIGHT);
	Framebuffer preScreenBuffer(WIDTH, HEIGHT);

	Sound test("sounds/whatsthis.mp3");
	test.setVolume(0);
	test.play();

	do {
		// Setup frame
		time = glfwGetTime();
		rotator.poll(window);
		glm::vec3 clear_color = glm::vec3(1.0f, 0.6f, 0.4f);
		clear_color = glm::vec3(0.5f, 0.7f, 0.9f); // day
		clear_color = glm::vec3(1.0f, 0.6f, 0.4f); // eve
		myWindow.initFrame(clear_color);

		GLint texLoc; // Bind textures to this location

		// =========================
		// Refraction render pass 
		// =========================
		refractionBuffer.bindBuffer(); // Render to framebuffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // We're not using stencil buffer now
		glEnable(GL_DEPTH_TEST);

		

		mountain_refraction_program();
		mountain_refraction_program.updateCommonUniforms(rotator, WIDTH, HEIGHT, time, clear_color);
		glEnable(GL_TEXTURE_2D);
		texLoc = glGetUniformLocation(mountain_refraction_program, "causticsTexture");
		glUniform1i(texLoc, 0);
		glActiveTexture(GL_TEXTURE0);
		caustics_texture.bindTexture();
		mountain.draw(window);

		environment_program();
		environment_program.updateMirrorUniforms(rotator, WIDTH, HEIGHT, time, clear_color);
		sphereMap.draw();

		// =========================
		// Reflection render pass 
		// =========================
		reflectionBuffer.bindBuffer(); // Render to framebuffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // We're not using stencil buffer now
		glEnable(GL_DEPTH_TEST);

		mountain_mirror_program();
		mountain_mirror_program.updateCommonUniforms(rotator, WIDTH, HEIGHT, time, clear_color);
		mountain.draw(window);

		glActiveTexture(GL_TEXTURE0);
		sky_texture.bindTexture();
		environment_program();
		environment_program.updateMirrorUniforms(rotator, WIDTH, HEIGHT, time, clear_color);
		sphereMap.draw();

		sun_program();
		sun_program.updateMirrorUniforms(rotator, WIDTH, HEIGHT, time, clear_color);
		sun.draw();


		// =========================
		// Depth render pass 
		// =========================
		depthBuffer.bindBuffer();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // We're not using stencil buffer now
		glEnable(GL_DEPTH_TEST);

		mountain_depth_program();
		mountain_depth_program.updateCommonUniforms(rotator, WIDTH, HEIGHT, time, clear_color);
		mountain.draw(window);
		
		// =========================
		// Environment render pass 
		// =========================
		if (glfwGetKey(window, GLFW_KEY_W)) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); else glPolygonMode(GL_FRONT_AND_BACK, GL_TRIANGLES);
		preScreenBuffer.bindBuffer();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // We're not using stencil buffer now
		glEnable(GL_DEPTH_TEST);

		glActiveTexture(GL_TEXTURE0);
		sky_texture.bindTexture();
		environment_program();
		environment_program.updateCommonUniforms(rotator, WIDTH, HEIGHT, time, clear_color);
		sphereMap.draw();

		// =========================
		// Mountain render pass 
		// =========================
		mountain_program();
		mountain_program.updateCommonUniforms(rotator, WIDTH, HEIGHT, time, clear_color);
		mountain.draw(window);

		

		// =========================
		// Water render pass 
		// =========================

		if (glfwGetKey(window, GLFW_KEY_T)) {
			tequila_program();
			tequila_program.updateCommonUniforms(rotator, WIDTH, HEIGHT, time, clear_color);
		}
		else {
			water_program();
			water_program.updateCommonUniforms(rotator, WIDTH, HEIGHT, time, clear_color);
		}

		glEnable(GL_TEXTURE_2D);
		
		texLoc = glGetUniformLocation(water_program, "reflectionTexture");
		glUniform1i(texLoc, 0);
		texLoc = glGetUniformLocation(water_program, "refractionTexture");
		glUniform1i(texLoc, 1);
		texLoc = glGetUniformLocation(water_program, "depthTexture");
		glUniform1i(texLoc, 2);

		glActiveTexture(GL_TEXTURE0);
		reflectionBuffer.bindTexture();
		glActiveTexture(GL_TEXTURE1);
		refractionBuffer.bindTexture();
		glActiveTexture(GL_TEXTURE2);
		depthBuffer.bindTexture();

		water.draw(window);

		// =========================
		// Sun render pass 
		// =========================
		sun_program();
		sun_program.updateCommonUniforms(rotator, WIDTH, HEIGHT, time, clear_color);

		texLoc = glGetUniformLocation(sun_program, "refractionTexture");
		glUniform1i(texLoc, 0);
		texLoc = glGetUniformLocation(sun_program, "earthTexture");
		glUniform1i(texLoc, 1);

		glActiveTexture(GL_TEXTURE0);
		texture.bindTexture();
		glActiveTexture(GL_TEXTURE1);
		earth_texture.bindTexture();
		sun.draw();

		// =========================
		// Post render pass 
		// =========================
	/*	glBindFramebuffer(GL_FRAMEBUFFER, 0);
		post_program();

		texLoc = glGetUniformLocation(post_program, "screenTexture");
		glUniform1i(texLoc, 0);

		glActiveTexture(GL_TEXTURE0);
		glEnable(GL_TEXTURE_2D);
		preScreenBuffer.bindTexture();
		quad.draw();*/


		// Finish frame
		glfwSwapInterval(2);
		glfwSwapBuffers(window);
		glfwPollEvents();

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	glDisableVertexAttribArray(0);

	return 0;
}


