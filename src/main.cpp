/*
	main.cpp

	The unmoved mover....
*/

#include <chrono>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

#include "world/chunk.h"
#include "util/geometry.h"
#include "rendering/renderer.h"

#define VERSION 0.01

void main()
{
	/*
		Let's get some meta-details straight.
		Mandalin version, OpenGL version, etc.
	*/
	std::ostringstream o;
	o << std::setprecision(2) << std::noshowpoint << VERSION;
	std::cout << "Running Mandalin, version: " + o.str() + "." << std::endl;

	/*
		Now we go through the process of initializing OpenGL,
		GLAD, GLFW, GLADOS, GLERP, GLEW, whatever....
	*/
	int windowWidth = 1800;
	int windowHeight = 800;
	std::string title = "Mandalin";

	GLFWwindow* window;

	if (!glfwInit())
	{
		std::cout << "Failed to initialize GLFW." << std::endl;
		return;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHintString(GLFW_X11_CLASS_NAME, "OpenGL");
	glfwWindowHintString(GLFW_X11_INSTANCE_NAME, "OpenGL");

	window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		std::cout << "Failed to create Opengl Window." << std::endl;
		return;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	/*
		Now we're going to fire up rendering:
		camera, renderer, etc.
	*/
	Mandalin::Camera* camera = new Mandalin::Camera({ 0, 0, 0 }, { 1, 0, 0, 0 }, 1.0f, window);
	Mandalin::Renderer* renderer = new Mandalin::Renderer(camera);
	Mandalin::Planet* planet = new Mandalin::Planet(6);

	/*
		And now we can run the loop.
	*/
	float lastTime = 0.0f;
	auto start = std::chrono::steady_clock::now();
	int frameCount = 0;

	while (!glfwWindowShouldClose(window))
	{
		float deltaTime = glfwGetTime() - lastTime;
		lastTime = glfwGetTime();

		frameCount++;
		auto now = std::chrono::steady_clock::now();
		auto diff = now - start;

		if (diff >= std::chrono::seconds(1))
		{
			start = now;
			std::cout << "Frame Count: " + std::to_string(frameCount) << std::endl;
			frameCount = 0;
		}

		camera->Update(deltaTime);

		renderer->Render(planet);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	std::cout << "Shutting down Mandalin. Have a wonderful day!" << std::endl;

	delete planet;
	delete renderer;
	delete camera;
}