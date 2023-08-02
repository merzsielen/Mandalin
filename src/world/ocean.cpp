#include "ocean.h"

#include <iostream>

namespace Mandalin
{
	/*-----------------------------------------------*/
	/* World Generation */
	/*-----------------------------------------------*/
	Ocean::Ocean(Polyhedron* polyhedron)
	{
		std::vector<Triangle> triangles;

		float radius = polyhedron->radius * Settings::OceanOffset;

		for (int i = 0; i < polyhedron->faces.size(); i++)
		{
			TriFace* tf = &polyhedron->faces[i];

			glm::vec3 a = radius * glm::normalize(polyhedron->vertices[tf->a].vertex);
			glm::vec3 b = radius * glm::normalize(polyhedron->vertices[tf->b].vertex);
			glm::vec3 c = radius * glm::normalize(polyhedron->vertices[tf->c].vertex);

			glm::vec4 color = Settings::OceanColor;

			Triangle tri =
			{
				{ a.x, a.y, a.z, color.r, color.g, color.b, color.a, 0.0f, 0.0f },
				{ b.x, b.y, b.z, color.r, color.g, color.b, color.a, 0.0f, 0.0f },
				{ c.x, c.y, c.z, color.r, color.g, color.b, color.a, 0.0f, 0.0f }
			};

			triangles.push_back(tri);
		}

		std::cout << "Generating an ocean with " << triangles.size() << " triangles." << std::endl;

		for (int i = 0; i < triangles.size(); i += Settings::OceanChunkMaxTris)
		{
			unsigned int allotedTris = std::min(Settings::OceanChunkMaxTris, (unsigned int)triangles.size() - i);

			chunks.push_back({});
			OceanChunk* c = &chunks[chunks.size() - 1];

			c->index = chunks.size() - 1;
			c->triCount = allotedTris;

			c->center = glm::vec3(0.0f, 0.0f, 0.0f);
			for (int j = i; j < i + allotedTris; j++)
			{
				Triangle* t = &triangles[j];
				glm::vec3 ta = { t->a.x, t->a.y, t->a.z };
				glm::vec3 tb = { t->b.x, t->b.y, t->b.z };
				glm::vec3 tc = { t->c.x, t->c.y, t->c.z };
				glm::vec3 td = (ta + tb + tc) / 3.0f;

				c->center = td;
			}
			c->center /= allotedTris;
			c->center = radius * glm::normalize(c->center);

			GLuint IBO;

			glGenVertexArrays(1, &c->vao);
			glBindVertexArray(c->vao);

			glGenBuffers(1, &c->vbo);
			glBindBuffer(GL_ARRAY_BUFFER, c->vbo);

			glGenBuffers(1, &IBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

			glBufferData(GL_ARRAY_BUFFER, c->triCount * sizeof(Triangle), &triangles[i], GL_DYNAMIC_DRAW);

			// Coordinates
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));
			glEnableVertexAttribArray(0);

			// Color
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, r));
			glEnableVertexAttribArray(1);

			unsigned int indices[Settings::OceanChunkMaxTris * 3];
			for (int i = 0; i < Settings::OceanChunkMaxTris; i++)
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
		}
	}
}