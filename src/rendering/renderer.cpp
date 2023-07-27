#include "renderer.h"
#include <iostream>

namespace Mandalin
{
	/*-----------------------------------------------*/
	/* Render */
	/*-----------------------------------------------*/
	void Renderer::Render(std::vector<Triangle> in)
	{
		shaders[0].Use();
		shaders[0].SetMatrix("MVP", camera->GetViewProjection());

		for (int i = 0; i < in.size(); i += Chunk::MAXTRIS)
		{
			unsigned int triCount = std::min(Chunk::MAXTRIS, (unsigned int)in.size() - i);

			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, triCount * sizeof(Triangle), &in[i]);
			glDrawElements(GL_TRIANGLES, triCount * 3, GL_UNSIGNED_INT, nullptr);
		}
	}

	void Renderer::Render(Planet* planet)
	{
		shaders[0].Use();
		shaders[0].SetMatrix("MVP", camera->GetViewProjection());

		glm::vec3 camPos = camera->GetPosition();
		glm::vec3 planetPos = planet->GetPosition();

		float relCamDistance = glm::distance(camPos, planetPos) / planet->GetRadius();
		float highestTheta = 1.74f * relCamDistance;

		glm::vec3 ab = planetPos - camPos;

		for (int i = 0; i < planet->ChunkCount(); i++)
		{
			Chunk* c = planet->GetChunk(i);

			glm::vec3 bc = c->center - planetPos;

			float dotABC = glm::dot(ab, bc);
			float magABC = glm::length(ab) * glm::length(bc);

			float theta = acosf(dotABC / magABC);

			if (theta < highestTheta)
			{
				glBindVertexArray(VAO);
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glBufferSubData(GL_ARRAY_BUFFER, 0, c->triCount * sizeof(Triangle), &c->triangles[0]);
				glDrawElements(GL_TRIANGLES, c->triCount * 3, GL_UNSIGNED_INT, nullptr);
			}
		}

		// std::cout << relCamDistance << std::endl;
	}

	/*-----------------------------------------------*/
	/* Constructor & Deconstructor */
	/*-----------------------------------------------*/
	Renderer::Renderer(Camera* camera)
	{
		this->camera = camera;

		Shader shader = { "assets/shaders/base.vert", "assets/shaders/base.frag" };
		shaders.push_back(shader);

		GLuint IBO;

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glGenBuffers(1, &IBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

		glBufferData(GL_ARRAY_BUFFER, Chunk::MAXTRIS * sizeof(Triangle), nullptr, GL_DYNAMIC_DRAW);

		// Coordinates
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));
		glEnableVertexAttribArray(0);

		// Color
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, r));
		glEnableVertexAttribArray(1);

		unsigned int indices[Chunk::MAXTRIS * 3];
		for (int i = 0; i < Chunk::MAXTRIS; i++)
		{
			const int offset = 3 * i;

			indices[offset + 0] = offset + 0;
			indices[offset + 1] = offset + 1;
			indices[offset + 2] = offset + 2;
		}

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glUseProgram(shader.ID);
	}

	Renderer::~Renderer()
	{
		// Nothing quite yet.
	}
}