#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Mandalin
{
	/*-----------------------------------------------*/
	/* Constructor */
	/*-----------------------------------------------*/
	Camera::Camera(glm::vec3 position, Quaternion rotation, float zoom)
	{
		this->position = position;
		this->rotation = rotation;
		this->zoom = zoom;
	}

	/*-----------------------------------------------*/
	/* Termination */
	/*-----------------------------------------------*/
	void Camera::Terminate()
	{
		delete this;
	}
}