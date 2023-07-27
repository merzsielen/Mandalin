#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Mandalin
{
	/*-----------------------------------------------*/
	/* Temporary Input Handling */
	/*-----------------------------------------------*/
	void Camera::HandleInput(float deltaTime)
	{
		bool lookUp = (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS);
		bool lookDown = ((glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) && !lookUp);
		bool moveIn = (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS);
		bool moveOut = ((glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) && !moveIn);
		bool lookRight = (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS);
		bool lookLeft = ((glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) && !lookRight);

		if (moveIn) position.z += 100.0f * zoom * deltaTime;
		else if (moveOut) position.z -= 100.0f * zoom * deltaTime;

		Quaternion q = { 2.0f * deltaTime, 1, 0, 0 };
		Quaternion q2 = { 2.0f * -deltaTime, 1, 0, 0 };
		if (lookUp) rotation = rotation * q;
		else if (lookDown) rotation = rotation * q2;

		q = { 0.5f * deltaTime, 0, 1, 0 };
		q2 = { 0.5f * -deltaTime, 0, 1, 0 };
		if (lookRight) rotation = rotation * q;
		else if (lookLeft) rotation = rotation * q2;

		NormalizeQuaternion(rotation);
	}

	/*-----------------------------------------------*/
	/* Projection & View Functions */
	/*-----------------------------------------------*/
	void Camera::UpdateProjection()
	{
		if (lens == Lens::orthographic)
		{
			float halfWidth = (windowWidth / 2.0f) * zoom;
			float halfHeight = (windowHeight / 2.0f) * zoom;

			projection = glm::ortho(-halfWidth, halfWidth, -halfWidth, halfHeight, nearClip, farClip);
		}
		else
		{
			projection = glm::perspective(fov, (windowWidth / (float)windowHeight), nearClip, farClip);

		}
	}

	void Camera::UpdateView()
	{
		view = glm::lookAt(position, position + CameraForward(), CameraUp());
		if (lens == Lens::perspective) view = glm::inverse(view);
	}

	/*-----------------------------------------------*/
	/* Update (Main Loop) */
	/*-----------------------------------------------*/
	void Camera::Update(float deltaTime)
	{
		HandleInput(deltaTime);

		glfwGetWindowSize(window, &windowWidth, &windowHeight);
		glViewport(0, 0, windowWidth, windowHeight);

		UpdateProjection();
		UpdateView();

		UpdateRotation();

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glClearDepth(1.0);
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	/*-----------------------------------------------*/
	/* Constructor & Deconstructor */
	/*-----------------------------------------------*/
	Camera::Camera(glm::vec3 position, Quaternion rotation, float zoom, GLFWwindow* window)
	{
		this->position = position;
		this->rotation = rotation;
		this->zoom = zoom;

		this->window = window;
		glfwGetWindowSize(window, &windowWidth, &windowHeight);

		UpdateView();
		UpdateProjection();
	}

	Camera::~Camera()
	{
		// Nothing yet really to do here.
	}
}