#include "camera.h"

#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Mandalin
{
	/*-----------------------------------------------*/
	/* Temporary Input Handling */
	/*-----------------------------------------------*/
	void Camera::HandleInput(float deltaTime)
	{
		// I know.
		glm::vec3 planetPosition = glm::vec3(0.0f, 0.0f, 0.0f);
		float planetRadius = 100.0f;

		float distance = glm::distance(position, planetPosition);

		glm::vec3 forward = CameraForward();
		glm::vec3 up = CameraUp();
		glm::vec3 right = CameraRight();

		bool moveRight = (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS);
		bool moveLeft = ((glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) && !moveRight);
		bool moveUp = (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS);
		bool moveDown = ((glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) && !moveUp);
		bool moveIn = (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS);
		bool moveOut = ((glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) && !moveIn);

		float rotationSpeed = movementSpeed / (180.0f * (distance / (planetRadius + 15.0f)));

		if (moveRight)
		{
			position += right * movementSpeed * zoom * deltaTime;
			rotation = Slerp(rotation, rotation * Quaternion(deltaTime, 0.0f, 1.0f, 0.0f), deltaTime * rotationSpeed);
		}
		else if (moveLeft)
		{
			position -= right * movementSpeed * zoom * deltaTime;
			rotation = Slerp(rotation, rotation * Quaternion(deltaTime, 0.0f, -1.0f, 0.0f), deltaTime * rotationSpeed);
		}

		if (moveUp)
		{
			position += up * movementSpeed * zoom * deltaTime;
			rotation = Slerp(rotation, rotation * Quaternion(deltaTime, -1.0f, 0.0f, 0.0f), deltaTime * rotationSpeed);
		}
		else if (moveDown)
		{
			position -= up * movementSpeed * zoom * deltaTime;
			rotation = Slerp(rotation, rotation * Quaternion(deltaTime, 1.0f, 0.0f, 0.0f), deltaTime * rotationSpeed);
		}

		if (moveIn && (distance < maxCameraDistance)) position += forward * movementSpeed * zoom * deltaTime;
		else if (moveOut && (distance > minCameraDistance)) position -=  forward * movementSpeed * zoom * deltaTime;

		distance = std::min(std::max(distance, minCameraDistance), maxCameraDistance);
		if (!moveIn && !moveOut) position = distance * glm::normalize(position);
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
		view = glm::lookAt(position + CameraForward(), position, CameraUp());
		// if (lens == Lens::perspective) view = glm::inverse(view);
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