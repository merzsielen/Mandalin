#include "planet.h"
#include <time.h>
#include <iostream>

namespace Mandalin
{
	void Planet::Hexify(std::vector<TriFace> in)
	{
		/*
			First, we have to turn our tri-faces into hexes.
			Then, we feed those into our chunks.
			This is probably going to be computationally
			expensive as we also need to determine the
			neighbors of each hex.
		*/

		/*
			So, we are given a list of triangles (with redundant
			vertices) and we are tasked with doing a few things:
			
				- Determening all the hexes.
				- Dividing them into chunks.
				- Turning each hex into a hex-column.
				- Finding all their neighbors.

				(PICK UP HERE NEXT TIME)
		*/
		
		for (int i = 0; i < in.size(); i += Chunk::MAXHEXES)
		{

			unsigned int allotedHexes = std::min(Chunk::MAXHEXES, (unsigned int)in.size() - i);

			for (int j = i; j < i + allotedHexes; j++)
			{

			}
		}
	}

	Planet::Planet(unsigned int worldSize)
	{
		this->worldSize = worldSize;

		srand(time(NULL));

		std::vector<Mandalin::TriFace> faces = Mandalin::Icosahedron();

		for (int i = 0; i < worldSize; i++) faces = Mandalin::Subdivide(faces);

		std::vector<Mandalin::Triangle> triangles;

		for (int i = 0; i < faces.size(); i++)
		{
			Mandalin::Triangle t;

			glm::vec3 a = faces[i].vertices[0];
			glm::vec3 b = faces[i].vertices[1];
			glm::vec3 c = faces[i].vertices[2];

			glm::vec3 color = glm::vec3((rand() % 100 + 1) / 100.0f, (rand() % 100 + 1) / 100.0f, (rand() % 100 + 1) / 100.0f);

			t.a = { a.x, a.y, a.z, color.r, color.g, color.b, 1.0f };
			t.b = { b.x, b.y, b.z, color.r, color.g, color.b, 1.0f };
			t.c = { c.x, c.y, c.z, color.r, color.g, color.b, 1.0f };

			triangles.push_back(t);
		}

		for (int i = 0; i < triangles.size(); i += Chunk::MAXTRIS)
		{
			unsigned int allotedTris = std::min(Chunk::MAXTRIS, (unsigned int)triangles.size() - i);

			chunks.push_back({});
			
			Chunk* c = &chunks[chunks.size() - 1];

			c->index = chunks.size() - 1;
			c->triCount = allotedTris;
			for (int j = i; j < i + allotedTris; j++) c->triangles[j - i] = triangles[j];
		}

		std::cout << "Generated " << triangles.size() << " triangles." << std::endl;

		// Hexify(faces);
	}
}