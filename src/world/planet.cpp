#include "planet.h"
#include <time.h>
#include <iostream>

namespace Mandalin
{
	void Planet::Hexify(Polyhedron* polyhedron)
	{
		/*
			First, we have to turn our tri-faces into hexes.
			Then, we feed those into our chunks.
			This is probably going to be computationally
			expensive as we also need to determine the
			neighbors of each hex.
		*/

		/*
			So, we are given a list of vertices and triangles
			(composed of the indices of their vertices) and
			are tasked with doing a few things:
			
				- Find all the triangles which share each vertex.
				- Use this information to create all the hexes.
				- Divide them into chunks.
				- Turn each hex into a hex-column.
				- Find all their neighbors.
		*/

		/*
			First, we need to find all our hexes.
			Each unique vertex and each triangle is
			going to be made into a hex. This does complicate
			forming chunks later (as there isn't a super easy
			way to interleave these two processes), but it
			shouldn't be too bad.
		*/

		std::vector<HexNode> hexNodes;

		/*
			First, we go through each vertex.
		*/
		for (int i = 0; i < polyhedron->vertices.size(); i++)
		{
			PolyVert* pv = &polyhedron->vertices[i];

			/*
				We need to find all the vertices of the triangles
				which share this vertex.
			*/
			std::vector<unsigned int> neighborIndices;

			for (int j = 0; j < pv->sharers.size(); j++)
			{
				TriFace* tf = &polyhedron->faces[pv->sharers[j]];

				PolyVert* pa = &polyhedron->vertices[tf->a];
				PolyVert* pb = &polyhedron->vertices[tf->b];
				PolyVert* pc = &polyhedron->vertices[tf->c];

				if (pa == pv) pa = nullptr;
				else if (pb == pv) pb = nullptr;
				else if (pc == pv) pc = nullptr;

				for (int k = 0; k < neighborIndices.size(); k++)
				{
					if (neighborIndices[k] == tf->a) pa = nullptr;
					else if (neighborIndices[k] == tf->b) pb = nullptr;
					else if (neighborIndices[k] == tf->c) pc = nullptr;
				}

				if (pa) neighborIndices.push_back(tf->a);
				if (pb) neighborIndices.push_back(tf->b);
				if (pc) neighborIndices.push_back(tf->c);
			}

			HexNode hn =
			{
				i,
				pv->vertex,
				neighborIndices
			};

			hexNodes.push_back(hn);
		}

		/*
			Now, we go through each face.
		*/
		for (int i = 0; i < polyhedron->faces.size(); i++)
		{

		}

		std::cout << "Generated " << hexNodes.size() << " hexes." << std::endl;

		/*
			Now, we need to go through and sort each hex's neighbors.

		*/
		for (int i = 0; i < hexNodes.size(); i++)
		{
			HexNode* hn = &hexNodes[i];

			unsigned int currentNeighborIdx = hn->neighbors[0];
			std::vector<unsigned int> sortedNeighbors = { currentNeighborIdx };

			// Now, we iterate through all the neighbors.
			while (true)
			{
				// And we see what the nearest neighbor is which hasn't already
				// been added to sortedNeighbors.
				glm::vec3 a = hexNodes[currentNeighborIdx].center;

				unsigned int nearestNeighbor;
				float nearestDistance = INFINITY;

				for (int j = 0; j < hn->neighbors.size(); j++)
				{
					bool alreadyIncluded = false;

					for (int k = 0; k < sortedNeighbors.size(); k++)
					{
						if (sortedNeighbors[k] == hn->neighbors[j]) alreadyIncluded = true;
					}

					if (alreadyIncluded) continue;

					glm::vec3 b = hexNodes[hn->neighbors[j]].center;

					float dist = glm::distance(a, b);

					if (dist < nearestDistance)
					{
						nearestDistance = dist;
						nearestNeighbor = hn->neighbors[j];
					}
				}

				if (nearestDistance == INFINITY) break;
				sortedNeighbors.push_back(nearestNeighbor);
				currentNeighborIdx = nearestNeighbor;
			}

			hn->neighbors = sortedNeighbors;
		}

		/*
			Now, finally, we go through and convert each hex
			to triangles. This is temporary, just to ensure that
			our hexification is working correctly.
		*/
		std::vector<Triangle> triangles;

		for (int i = 0; i < hexNodes.size(); i++)
		{
			HexNode* hn = &hexNodes[i];

			glm::vec3 color = glm::vec3((rand() % 100 + 1) / 100.0f, (rand() % 100 + 1) / 100.0f, (rand() % 100 + 1) / 100.0f);

			std::vector<glm::vec3> verts;

			for (int j = 0; j < hn->neighbors.size(); j++)
			{
				glm::vec3 a = hn->center;
				glm::vec3 b = hexNodes[hn->neighbors[j]].center;
				verts.push_back(polyhedron->radius * glm::normalize(Lerp(a, b, (1 / 3.0f))));
			}

			for (int j = 0; j < verts.size(); j++)
			{
				glm::vec3 a = hn->center;
				glm::vec3 b = verts[j];
				glm::vec3 c = verts[(static_cast<unsigned long long>(j) + 1) % verts.size()];

				Triangle t =
				{
					{ a.x, a.y, a.z, color.r, color.g, color.b, 1.0f },
					{ b.x, b.y, b.z, color.r, color.g, color.b, 1.0f },
					{ c.x, c.y, c.z, color.r, color.g, color.b, 1.0f }
				};

				triangles.push_back(t);
			}
		}

		std::cout << "Generated " << triangles.size() << " triangles." << std::endl;

		for (int i = 0; i < triangles.size(); i += Chunk::MAXTRIS)
		{
			unsigned int allotedTris = std::min(Chunk::MAXTRIS, (unsigned int)triangles.size() - i);

			chunks.push_back({});

			Chunk* c = &chunks[chunks.size() - 1];

			c->index = chunks.size() - 1;
			c->triCount = allotedTris;
			for (int j = i; j < i + allotedTris; j++) c->triangles[j - i] = triangles[j];
		}

		std::cout << "Done." << std::endl;
	}

	Planet::Planet(unsigned int worldSize)
	{
		this->worldSize = worldSize;

		srand(time(NULL));

		Polyhedron* polyhedron = new Polyhedron(worldSize);

		for (int i = 0; i < worldSize; i++) polyhedron->Subdivide();

		/*std::vector<Triangle> triangles;

		for (int j = 0; j < polyhedron->faces.size(); j++)
		{
			TriFace tf = polyhedron->faces[j];

			glm::vec3 a = polyhedron->vertices[tf.a].vertex;
			glm::vec3 b = polyhedron->vertices[tf.b].vertex;
			glm::vec3 c = polyhedron->vertices[tf.c].vertex;

			glm::vec3 color = glm::vec3((rand() % 100 + 1) / 100.0f, (rand() % 100 + 1) / 100.0f, (rand() % 100 + 1) / 100.0f);

			Triangle t =
			{
				{ a.x, a.y, a.z, color.r, color.g, color.b, 1.0f },
				{ b.x, b.y, b.z, color.r, color.g, color.b, 1.0f },
				{ c.x, c.y, c.z, color.r, color.g, color.b, 1.0f }
			};

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
		}*/

		Hexify(polyhedron);
	}
}