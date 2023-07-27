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
				neighborIndices
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
				neighborIndices
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

		std::vector<std::pair<unsigned int, unsigned int>> oldIndices;
		for (int i = 0; i < hexNodes.size(); i++)
		{
			oldIndices.push_back(std::pair<unsigned int, unsigned int>(hexNodes[i].index, 0));
		}

		// std::sort(hexNodes.begin(), hexNodes.end(), CompareDistances);
		hexNodes = VoronoiSort(hexNodes, worldSize * Chunk::MAXHEXES);

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

		/*
			Now, finally, we go through and convert each hex
			to triangles.
		*/
		std::vector<Triangle> triangles;

		int continentCount = hexNodes.size() / Chunk::MAXHEXES;

		std::vector<int> oceans;
		std::vector<float> rises;

		for (int i = 0; i < continentCount; i++)
		{
			oceans.push_back((rand() % 100) + 1);
			rises.push_back((rand() % 5) / 10.0f);
			// rises.push_back(0.0f);
		}

		for (int i = 0; i < hexNodes.size(); i++)
		{
			HexNode* hn = &hexNodes[i];

			bool ocean = (oceans[hn->continent] > 50);
			float rise = rises[hn->continent];
			if (ocean) rise = -1.0f;
			glm::vec3 offset = rise * glm::normalize(hn->center);

			// glm::vec3 color = glm::vec3((rand() % 100 + 1) / 100.0f, (rand() % 100 + 1) / 100.0f, (rand() % 100 + 1) / 100.0f);
			glm::vec3 color = glm::vec3(0.05f, 0.73f, 0.28f);
			if (ocean) color = glm::vec3(0.0f, 0.0f, 1.0f);

			std::vector<glm::vec3> verts;

			for (int j = 0; j < hn->neighbors.size(); j++)
			{
				HexNode* neighbor1 = &hexNodes[hn->neighbors[j]];
				HexNode* neighbor2 = &hexNodes[hn->neighbors[(j + 1) % hn->neighbors.size()]];
				bool sameCont1 = (hn->continent == neighbor1->continent);
				bool sameCont2 = (hn->continent == neighbor2->continent);

				glm::vec3 offset1 = sameCont1 ? rise * glm::normalize(neighbor1->center) : offset;
				glm::vec3 offset2 = sameCont2 ? rise * glm::normalize(neighbor2->center) : offset;

				glm::vec3 a = hn->center + offset;
				glm::vec3 b = neighbor1->center + offset1;
				glm::vec3 c = neighbor2->center + offset2;
				// verts.push_back(polyhedron->radius * glm::normalize((a + b + c) / 3.0f));
				verts.push_back((a + b + c) / 3.0f);
			}

			// First, we add the top of the hex.
			for (int j = 0; j < verts.size(); j++)
			{
				glm::vec3 a = hn->center + offset;
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

			// And now we add the sides.
			color /= 2.0f;

			glm::vec3 onset = -((radius / 4.0f) * glm::normalize(hn->center));

			for (int j = 0; j < verts.size(); j++)
			{
				glm::vec3 a = verts[j];
				glm::vec3 b = a + onset;
				glm::vec3 c = verts[(static_cast<unsigned long long>(j) + 1) % verts.size()];
				glm::vec3 d = c + onset;

				Triangle adb =
				{
					{ a.x, a.y, a.z, color.r, color.g, color.b, 1.0f },
					{ d.x, d.y, d.z, color.r, color.g, color.b, 1.0f },
					{ b.x, b.y, b.z, color.r, color.g, color.b, 1.0f }
				};

				Triangle acd =
				{
					{ a.x, a.y, a.z, color.r, color.g, color.b, 1.0f },
					{ c.x, c.y, c.z, color.r, color.g, color.b, 1.0f },
					{ d.x, d.y, d.z, color.r, color.g, color.b, 1.0f }
				};

				triangles.push_back(adb);
				triangles.push_back(acd);
			}
		}

		/*
			Now, we go through and convert these hex nodes to hexes.
		*/
		std::cout << "Generated " << triangles.size() << " triangles." << std::endl;

		/*
			Since we're going to be modifying the chunks' vbos, we need to
			bind the vao of the renderer first.
		*/

		int t = 0;
		for (int i = 0; i < hexNodes.size(); i += Chunk::MAXHEXES)
		{
			unsigned int allotedHexes = std::min(Chunk::MAXHEXES, (unsigned int)hexNodes.size() - i);

			chunks.push_back({});
			Chunk* c = &chunks[chunks.size() - 1];

			c->index = chunks.size() - 1;
			c->hexCount = allotedHexes;
			c->triCount = allotedHexes * Chunk::TRISPERHEX;

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

				if (hexNodes[j].neighbors.size() == 5) c->triCount -= 3;

				c->center += hexNodes[j].center;
				c->hexes[j - i] = hex;
			}

			c->center /= allotedHexes;
			c->center = polyhedron->radius * glm::normalize(c->center);

			/*glm::vec3 color = glm::vec3((rand() % 100 + 1) / 100.0f, (rand() % 100 + 1) / 100.0f, (rand() % 100 + 1) / 100.0f);
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
			}*/

			GLuint IBO;

			glGenVertexArrays(1, &c->vao);
			glBindVertexArray(c->vao);

			glGenBuffers(1, &c->vbo);
			glBindBuffer(GL_ARRAY_BUFFER, c->vbo);

			glGenBuffers(1, &IBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

			glBufferData(GL_ARRAY_BUFFER, c->triCount * sizeof(Triangle), &triangles[t], GL_DYNAMIC_DRAW);

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

			t += c->triCount;
		}

		glBindVertexArray(0);
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