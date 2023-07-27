#include "planet.h"

#include <time.h>
#include <iostream>
#include <algorithm>

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
				unsigned int ind = pv->sharers[j];
				ind += polyhedron->vertices.size();
				neighborIndices.push_back(ind);
			}

			HexNode hn =
			{
				i,
				pv->vertex,
				neighborIndices,
				polyhedron->faces[pv->sharers[0]].faceOrigin
			};

			hexNodes.push_back(hn);
		}

		/*
			Now, we go through each face.
		*/
		for (int i = 0; i < polyhedron->faces.size(); i++)
		{
			TriFace* tf = &polyhedron->faces[i];

			/*
				Each hex derived from a face has six neighbors:
					
					3 derived from vertices
					3 derived from triangles

				Those derived from vertices come from the three
				vertices that make up the triangle, while the
				three derived from triangles come from the neighbors
				that share two vertices with the triangle.

				Since the former are easier, we can do those first.
			*/

			std::vector<unsigned int> neighborIndices;

			neighborIndices.push_back(tf->a);
			neighborIndices.push_back(tf->b);
			neighborIndices.push_back(tf->c);

			/*
				Now, we must go through and find the neighboring
				triangles that share two vertices with this one.

				We're going to do this in pairs. First all the
				sharers of vertex A, checking if they have B,
				then all the sharers of vertex B, checking if
				they have C, then all the sharers of vertex C,
				checking if they have A.
			*/

			// At A Checking B
			PolyVert* tfA = &polyhedron->vertices[tf->a];
			for (int j = 0; j < tfA->sharers.size(); j++)
			{
				TriFace* tf2 = &polyhedron->faces[tfA->sharers[j]];

				if (tf == tf2) continue;

				if (tf2->a == tf->b ||
					tf2->b == tf->b ||
					tf2->c == tf->b)
				{
					neighborIndices.push_back(polyhedron->vertices.size() + tfA->sharers[j]);
					break;
				}
			}

			// At B Checking C
			PolyVert* tfB = &polyhedron->vertices[tf->b];
			for (int j = 0; j < tfB->sharers.size(); j++)
			{
				TriFace* tf2 = &polyhedron->faces[tfB->sharers[j]];

				if (tf == tf2) continue;

				if (tf2->a == tf->c ||
					tf2->b == tf->c ||
					tf2->c == tf->c)
				{
					neighborIndices.push_back(polyhedron->vertices.size() + tfB->sharers[j]);
					break;
				}
			}

			// At C Checking A
			PolyVert* tfC = &polyhedron->vertices[tf->c];
			for (int j = 0; j < tfC->sharers.size(); j++)
			{
				TriFace* tf2 = &polyhedron->faces[tfC->sharers[j]];

				if (tf == tf2) continue;

				if (tf2->a == tf->a ||
					tf2->b == tf->a ||
					tf2->c == tf->a)
				{
					neighborIndices.push_back(polyhedron->vertices.size() + tfC->sharers[j]);
					break;
				}
			}

			glm::vec3 center = polyhedron->vertices[tf->a].vertex;
			center += polyhedron->vertices[tf->b].vertex;
			center += polyhedron->vertices[tf->c].vertex;
			center = polyhedron->radius * glm::normalize(center / 3.0f);

			HexNode hn =
			{
				i + polyhedron->vertices.size(),
				center,
				neighborIndices,
				tf->faceOrigin
			};

			hexNodes.push_back(hn);
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
			Before we convert to triangles, we need to sort them so that
			the hexes in a chunk are all (mostly) contiguous.
		*/
		/*std::vector<std::pair<unsigned int, unsigned int>> oldIndices;
		for (int i = 0; i < hexNodes.size(); i++)
		{
			oldIndices.push_back(std::pair<unsigned int, unsigned int>(hexNodes[i].index, 0));
		}

		std::sort(hexNodes.begin(), hexNodes.end(), CompareDistances);

		for (int i = 0; i < hexNodes.size(); i++)
		{
			oldIndices[hexNodes[i].index].second = i;
		}

		for (int i = 0; i < hexNodes.size(); i++)
		{
			for (int j = 0; j < hexNodes[i].neighbors.size(); j++)
			{
				hexNodes[i].neighbors[j] = oldIndices[hexNodes[i].neighbors[j]].second;
			}
		}

		for (int i = 0; i < hexNodes.size(); i++)
		{
			hexNodes[i].index = i;
		}*/

		std::vector<std::pair<unsigned int, unsigned int>> oldIndices;
		for (int i = 0; i < hexNodes.size(); i++)
		{
			oldIndices.push_back(std::pair<unsigned int, unsigned int>(hexNodes[i].index, 0));
		}

		// std::sort(hexNodes.begin(), hexNodes.end(), CompareDistances);
		hexNodes = VoronoiSort(hexNodes, Chunk::MAXHEXES);

		for (int i = 0; i < hexNodes.size(); i++)
		{
			oldIndices[hexNodes[i].index].second = i;
		}

		for (int i = 0; i < hexNodes.size(); i++)
		{
			for (int j = 0; j < hexNodes[i].neighbors.size(); j++)
			{
				hexNodes[i].neighbors[j] = oldIndices[hexNodes[i].neighbors[j]].second;
			}
		}

		for (int i = 0; i < hexNodes.size(); i++)
		{
			hexNodes[i].index = i;
		}

		/*std::vector<HexNode*> orderedHexNodes;
		for (int i = 0; i < hexNodes.size(); i++) orderedHexNodes.push_back(&hexNodes[i]);

		orderedHexNodes = VoronoiSort(orderedHexNodes, Chunk::MAXHEXES);

		for (int i = 0; i < orderedHexNodes.size(); i++) orderedHexNodes[i]->index = i;

		for (int i = 0; i < hexNodes.size(); i++)
		{
			HexNode* hn = &hexNodes[i];

			for (int j = 0; j < hn->neighbors.size(); j++)
			{
				hn->neighbors[j] = hexNodes[hn->neighbors[j]].index;
			}
		}*/

		/*
			Now, finally, we go through and convert each hex
			to triangles. This is temporary, just to ensure that
			our hexification is working correctly.
		*/
		std::vector<Triangle> triangles;

		for (int i = 0; i < hexNodes.size(); i++)
		{
			HexNode* hn = &hexNodes[i];
			
			// glm::vec3 color = glm::vec3((rand() % 100 + 1) / 100.0f, (rand() % 100 + 1) / 100.0f, (rand() % 100 + 1) / 100.0f);
			glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

			std::vector<glm::vec3> verts;

			for (int j = 0; j < hn->neighbors.size(); j++)
			{
				glm::vec3 a = hn->center;
				glm::vec3 b = hexNodes[hn->neighbors[j]].center;
				glm::vec3 c = hexNodes[hn->neighbors[(j + 1) % hn->neighbors.size()]].center;
				verts.push_back(polyhedron->radius * glm::normalize((a + b + c) / 3.0f));
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

		/*
			Now, we go through and convert these hex nodes to hexes.
		*/
		std::cout << "Generated " << triangles.size() << " triangles." << std::endl;

		int t = 0;
		for (int i = 0; i < hexNodes.size(); i += Chunk::MAXHEXES)
		{

			unsigned int allotedHexes = std::min(Chunk::MAXHEXES, (unsigned int)hexNodes.size() - i);

			chunks.push_back({});
			Chunk* c = &chunks[chunks.size() - 1];

			c->index = chunks.size() - 1;
			c->hexCount = allotedHexes;
			c->triCount = allotedHexes * 6;

			c->center = glm::vec3(0.0f, 0.0f, 0.0f);
			for (int j = i; j < i + allotedHexes; j++)
			{
				Hex hex =
				{
					c->index,
					j - i,
					{}
				};

				for (int k = 0; k < hexNodes[j].neighbors.size(); k++)
				{
					std::pair<unsigned int, unsigned int> p =
					{
						hexNodes[j].neighbors[k] / Chunk::MAXHEXES,
						hexNodes[j].neighbors[k] % Chunk::MAXHEXES
					};

					hex.neighbors.push_back(p);
				}

				if (hexNodes[j].neighbors.size() == 5) c->triCount--;

				c->center += hexNodes[j].center;
				c->hexes[j - i] = hex;
			}

			c->center /= allotedHexes;
			c->center = polyhedron->radius * glm::normalize(c->center);

			glm::vec3 color = glm::vec3((rand() % 100 + 1) / 100.0f, (rand() % 100 + 1) / 100.0f, (rand() % 100 + 1) / 100.0f);
			for (int j = t; j < t + c->triCount; j++)
			{
				triangles[j].a.r = color.r;
				triangles[j].b.r = color.r;
				triangles[j].c.r = color.r;
				triangles[j].a.g = color.g;
				triangles[j].b.g = color.g;
				triangles[j].c.g = color.g;
				triangles[j].a.b = color.b;
				triangles[j].b.b = color.b;
				triangles[j].c.b = color.b;

				c->triangles[j - t] = triangles[j];
			}

			t += c->triCount;
		}
	}

	Planet::Planet(unsigned int worldSize)
	{
		this->worldSize = worldSize;

		srand(time(NULL));

		Polyhedron* polyhedron = new Polyhedron(worldSize);
		this->radius = polyhedron->radius;

		for (int i = 0; i < worldSize; i++) polyhedron->Subdivide();

		Hexify(polyhedron);
	}
}