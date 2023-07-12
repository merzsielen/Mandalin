#ifndef RENDERER_H
#define RENDERER_H

#include <vector>

#include "shader.h"
#include "camera.h"
#include "../world/planet.h"

/*
	renderer.h

	Handles the preparation of anything we want
	rendered to the screen.
*/

namespace Mandalin
{
	/*-------------------------------------------------*/
	/* Renderer                                        */
	/*-------------------------------------------------*/
	/*
		The Renderer, well, renders.
	*/
	class Renderer
	{
	private:
		/*-----------------------------------------------*/
		/* General */
		/*-----------------------------------------------*/
		GLuint					VAO;
		GLuint					VBO;

		/*-----------------------------------------------*/
		/* Camera */
		/*-----------------------------------------------*/
		Camera* camera;

		/*-----------------------------------------------*/
		/* Shaders */
		/*-----------------------------------------------*/
		std::vector<Shader>		shaders;

	public:
		/*-----------------------------------------------*/
		/* Render */
		/*-----------------------------------------------*/
		void					Render(Planet* planet);
		void					Render(std::vector<Triangle> in);

		/*-----------------------------------------------*/
		/* Constructor & Deconstructor */
		/*-----------------------------------------------*/
		Renderer(Camera* camera);
		~Renderer();
	};
}

#endif