#ifndef CAMERA_H
#define CAMERA_H

/*
	camera.h

	Holds all data related to OpenGL and the meta-data
	related to rendering as well as the location and
	orientation of the camera.
*/

#include <glm/glm.hpp>
#include "../util/geometry.h"

namespace Mandalin
{

	class Camera
	{
	private:
		/*-----------------------------------------------*/
		/* Position, Rotation, & Zoom */
		/*-----------------------------------------------*/
		glm::vec3		position;
		Quaternion		rotation;
		float			zoom;

		/*-----------------------------------------------*/
		/* View */
		/*-----------------------------------------------*/
		int					viewDepth = 15000;
		int					viewWidth = 1920;
		int					viewHeight = 1080;

		/*-----------------------------------------------*/
		/* Background Color */
		/*-----------------------------------------------*/
		glm::vec4			backgroundColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

	public:
		/*-----------------------------------------------*/
		/* Position & Rotation Functions */
		/*-----------------------------------------------*/
		glm::vec3			Position() { return position; }
		Quaternion			Rotation() { return rotation; }
		float				Zoom() { return zoom; }

		void				Position(glm::vec3 pos) { position = pos; }
		void				Rotation(Quaternion quat) { rotation = quat; }
		void				Zoom(float z) { zoom = z; }

		void				UpdateRotation() { NormalizeQuaternion(rotation); }
		glm::vec3			CameraForward() { return Rotate({ 0.0f, 0.0f, -1.0f }, rotation); }
		glm::vec3			CameraUp() { return Rotate({ 0.0f, 1.0f, 0.0f }, rotation); }

		/*-----------------------------------------------*/
		/* View Functions */
		/*-----------------------------------------------*/
		int					ViewDepth() { return viewDepth; }
		int					ViewWidth() { return viewWidth; }
		int					ViewHeight() { return viewHeight; }

		void				ViewDepth(int depth) { viewDepth = depth; }
		void				ViewWidth(int width) { viewWidth = width; }
		void				ViewHeight(int height) { viewHeight = height; }

		/*-----------------------------------------------*/
		/* Background Color Functions */
		/*-----------------------------------------------*/
		glm::vec4			BackgroundColor() { return backgroundColor; }

		void				BackgroundColor(glm::vec4 color) { backgroundColor = color; }

		/*-----------------------------------------------*/
		/* Constructor */
		/*-----------------------------------------------*/
		Camera(glm::vec3 position, Quaternion rotation, float zoom);

		/*-----------------------------------------------*/
		/* Termination */
		/*-----------------------------------------------*/
		void				Terminate();
	};
}

#endif