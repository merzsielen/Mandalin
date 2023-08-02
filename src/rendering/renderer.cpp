#include "renderer.h"
#include <iostream>

namespace Mandalin
{
	/*-----------------------------------------------*/
	/* Render */
	/*-----------------------------------------------*/
	void Renderer::Render(Planet* planet)
	{
		shaders[1].Use();
		shaders[1].SetMatrix("MVP", camera->GetViewProjection());
		shaders[1].SetInt("Focus", (int)camera->GetFocus());
		shaders[1].SetVector4Arr("BiomeColors", Settings::BiomeColors[0], 64);
		shaders[1].SetVector4Arr("PlateColors", Settings::TectonicPlateColors[0], 64);

		Ocean* ocean = planet->GetOcean();

		glm::vec3 camPos = -camera->GetPosition();
		glm::vec3 planetPos = planet->GetPosition();

		float camDistance = glm::distance(camPos, planetPos);
		float relCamDistance = glm::distance(camPos, planetPos) / planet->GetRadius();
		float highestTheta = 1.74f * relCamDistance;

		glm::vec3 ab = planetPos - camPos;

		float time1 = lastTime;
		float time2 = Lerp(lastTime, (rand() % 1000), 0.0001f);

		if (planet->GetWorldSize() < 8)
		{
			/*
				On sufficiently small worlds, the cost of just rendering
				all the tiles is not that great, so we can just throw
				all the chunks into the renderer. At some point, we may
				re-implement a better culling method.
			*/

			for (int i = 0; i < planet->ChunkCount(); i++)
			{
				Chunk* c = planet->GetChunk(i);
				glBindVertexArray(c->vao);
				glBindBuffer(GL_ARRAY_BUFFER, c->vbo);
				glDrawElements(GL_TRIANGLES, c->triCount * 3, GL_UNSIGNED_INT, nullptr);
			}
		}
		else
		{
			for (int i = 0; i < planet->ChunkCount(); i++)
			{
				/*
					On larger worlds, we really can't afford to
					render all the tiles.
				*/
				Chunk* c = planet->GetChunk(i);

				glm::vec3 bc = c->center - planetPos;

				float dotABC = glm::dot(ab, bc);
				float magABC = glm::length(ab) * glm::length(bc);

				float theta = acosf(dotABC / magABC);

				if (theta < highestTheta)
				{
					glBindVertexArray(c->vao);
					glBindBuffer(GL_ARRAY_BUFFER, c->vbo);
					glDrawElements(GL_TRIANGLES, c->triCount * 3, GL_UNSIGNED_INT, nullptr);
				}
			}
		}

		/*
			Regardless of the world-size, we just render all the
			oceans. The cost is fairly marginal regardless.
		*/

		shaders[0].Use();
		shaders[0].SetMatrix("MVP", camera->GetViewProjection());
		shaders[0].SetFloat("time1", time1);
		shaders[0].SetFloat("time2", time2);

		for (int i = 0; i < ocean->ChunkCount(); i++)
		{
			OceanChunk* c = ocean->GetChunk(i);
			glBindVertexArray(c->vao);
			glBindBuffer(GL_ARRAY_BUFFER, c->vbo);
			glDrawElements(GL_TRIANGLES, c->triCount * 3, GL_UNSIGNED_INT, nullptr);
		}

		lastTime = time2;

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glUseProgram(0);
	}

	/*-----------------------------------------------*/
	/* Constructor & Deconstructor */
	/*-----------------------------------------------*/
	Renderer::Renderer(Camera* camera)
	{
		this->camera = camera;

		Shader waterShader = { "assets/shaders/water.vert", "assets/shaders/water.frag" };
		shaders.push_back(waterShader);

		Shader baseShader = { "assets/shaders/base.vert", "assets/shaders/base.frag" };
		shaders.push_back(baseShader);
	}

	Renderer::~Renderer()
	{
		// Nothing quite yet.
	}
}