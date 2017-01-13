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
#include "Sound.h";
//
//#define HEIGHT 675
//#define WIDTH 1200

#define HEIGHT 720
#define WIDTH 1280

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
	ShaderProgram earth_program("shaders/earth.vert", "", "", "", "shaders/earth.frag");
	ShaderProgram environment_program("shaders/environment.vert", "", "", "", "shaders/environment.frag");

	glUseProgram(0);

	MouseRotator rotator;
	rotator.init(window);

	Surface water(4, 4, 250, 600);
	Surface mountain(100, 200, 200, 400);
	Sphere sun(22, 22, 7.0f);
	Sphere sphereMap(20, 20, 1800.0f);
	Quad quad;

	Texture texture("textures/albin.png");
	Texture sky_texture("textures/mordor.jpg");
	Texture caustics_texture("textures/caustics.png");
	Texture earth_texture("textures/moonsphere.jpg");

	double time;

	Framebuffer framebuffer[6] = { { WIDTH, HEIGHT },{ WIDTH, HEIGHT },{ WIDTH, HEIGHT },{ WIDTH, HEIGHT },{ WIDTH, HEIGHT },{ WIDTH, HEIGHT } };
	Framebuffer depthBuffer(WIDTH, HEIGHT);
	Framebuffer refractionBuffer(WIDTH, HEIGHT);
	Framebuffer reflectionBuffer(WIDTH, HEIGHT);
	Framebuffer preScreenBuffer(WIDTH, HEIGHT);

	Sound test("sounds/birds1.mp3");
	test.setVolume(100);
	test.play();

	glViewport(0, 0, WIDTH, HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glClear(GL_COLOR_BUFFER_BIT);


	do {
		// Setup frame
		time = glfwGetTime();
		rotator.poll(window);
		glm::vec3 clear_color = glm::vec3(1.0f, 0.6f, 0.4f);
		myWindow.initFrame(clear_color);
		
		// Bind textures to this location
		GLint texLoc; 

		// =========================
		// Refraction render pass 
		// =========================
		refractionBuffer.bindBuffer(); // Render to framebuffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // We're not using stencil buffer now

		mountain_refraction_program();
		mountain_refraction_program.updateCommonUniforms(rotator, WIDTH, HEIGHT, time, clear_color);
		texLoc = glGetUniformLocation(mountain_refraction_program, "causticsTexture");
		glUniform1i(texLoc, 0);
		texLoc = glGetUniformLocation(mountain_refraction_program, "skyTexture");
		glUniform1i(texLoc, 1);
		glActiveTexture(GL_TEXTURE0);
		caustics_texture.bindTexture();
		glActiveTexture(GL_TEXTURE1);
		sky_texture.bindTexture();
		mountain.draw(window);

		environment_program();
		environment_program.updateMirrorUniforms(rotator, WIDTH, HEIGHT, time, clear_color);
		sphereMap.draw();

		// =========================
		// Reflection render pass 
		// =========================
		reflectionBuffer.bindBuffer(); // Render to framebuffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // We're not using stencil buffer now

		mountain_mirror_program();
		mountain_mirror_program.updateCommonUniforms(rotator, WIDTH, HEIGHT, time, clear_color);
		mountain.draw(window);

		glActiveTexture(GL_TEXTURE0);
		sky_texture.bindTexture();
		environment_program();
		environment_program.updateMirrorUniforms(rotator, WIDTH, HEIGHT, time, clear_color);
		sphereMap.draw();

		texLoc = glGetUniformLocation(earth_program, "refractionTexture");
		glUniform1i(texLoc, 0);
		texLoc = glGetUniformLocation(earth_program, "earthTexture");
		glUniform1i(texLoc, 1);

		glActiveTexture(GL_TEXTURE0);
		texture.bindTexture();
		glActiveTexture(GL_TEXTURE1);
		earth_texture.bindTexture();
		earth_program();
		earth_program.updateMirrorUniforms(rotator, WIDTH, HEIGHT, time, clear_color);
		sun.draw();

		// =========================
		// Depth render pass 
		// =========================
		depthBuffer.bindBuffer();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // We're not using stencil buffer now

		mountain_depth_program();
		mountain_depth_program.updateCommonUniforms(rotator, WIDTH, HEIGHT, time, clear_color);
		mountain.draw(window);
		
		// =========================
		// Environment render pass 
		// =========================
		if (glfwGetKey(window, GLFW_KEY_W)) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); else glPolygonMode(GL_FRONT_AND_BACK, GL_TRIANGLES);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // We're not using stencil buffer now

		environment_program();

		texLoc = glGetUniformLocation(environment_program, "skyTexture");
		glUniform1i(texLoc, 0);
		glActiveTexture(GL_TEXTURE0);
		sky_texture.bindTexture();

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

		// Render mode - Changes method in fragment shader of water_program
		GLint renderMode_Loc = glGetUniformLocation(water_program, "render_mode");
		float render_mode = 0.0f;
		if (glfwGetKey(window, GLFW_KEY_1)) render_mode = 1.0f;
		else if (glfwGetKey(window, GLFW_KEY_2)) render_mode = 2.0f;
		else if (glfwGetKey(window, GLFW_KEY_3)) render_mode = 3.0f;
		else if (glfwGetKey(window, GLFW_KEY_4)) render_mode = 4.0f;
		else if (glfwGetKey(window, GLFW_KEY_5)) render_mode = 5.0f;
		else if (glfwGetKey(window, GLFW_KEY_6)) render_mode = 6.0f;
		glProgramUniform1f(water_program, renderMode_Loc, render_mode);

		water.draw(window);

		// =========================
		// Sun render pass 
		// =========================
		earth_program();
		earth_program.updateCommonUniforms(rotator, WIDTH, HEIGHT, time, clear_color);

		texLoc = glGetUniformLocation(earth_program, "refractionTexture");
		glUniform1i(texLoc, 0);
		texLoc = glGetUniformLocation(earth_program, "earthTexture");
		glUniform1i(texLoc, 1);

		glActiveTexture(GL_TEXTURE0);
		texture.bindTexture();
		glActiveTexture(GL_TEXTURE1);
		earth_texture.bindTexture();
		sun.draw();

		//// =========================
		//// Post render pass 
		//// =========================
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//post_program();

		//texLoc = glGetUniformLocation(post_program, "screenTexture");
		//glUniform1i(texLoc, 0);

		//glActiveTexture(GL_TEXTURE0);
		//glEnable(GL_TEXTURE_2D);
		//preScreenBuffer.bindTexture();
		//quad.draw();

		// Finish frame
		glfwSwapInterval(0);
		glfwSwapBuffers(window);
		glfwPollEvents();

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	glDisableVertexAttribArray(0);

	return 0;
}


