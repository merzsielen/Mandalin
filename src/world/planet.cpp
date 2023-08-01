#include "planet.h"

#include <time.h>
#include <iostream>
#include <algorithm>

namespace Mandalin
{
	std::vector<HexNode> Planet::Hexify(Polyhedron* polyhedron)
	{
		/*
			First, we have to turn our polyhedron into hexes.
			Then, we feed those into our chunks.
			This is probably going to be computationally
			expensive as we also need to determine the
			neighbors of each hex.

			Right now, this function is kinda *thick*, so I'd
			like to find some way to break it down into smaller
			functions so it is more readable.
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
		return hexNodes;
	}

	std::vector<HexNode> Planet::SortNeighbors(std::vector<HexNode> hexNodes)
	{
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

		return hexNodes;
	}

	void Planet::GenerateGeometry(std::vector<HexNode> hexNodes)
	{
		/*
			Now, finally, we go through and convert each hex
			to triangles.
		*/
		std::vector<Triangle> triangles;

		int continentCount = hexNodes.size() / Chunk::MAXHEXES;

		std::vector<int> oceans;

		for (int i = 0; i < continentCount; i++)
		{
			oceans.push_back((rand() % 100) + 1);
		}

		/*
			Before we convert to triangles, we're actually going
			to roughen up the coastlines a bit.
			Sadly, this means looping over the nodes a few too
			many times.
			First, we have to set all the oceans.

			We can also take this moment to set all the faults.
		*/
		for (int i = 0; i < hexNodes.size(); i++)
		{
			HexNode* hn = &hexNodes[i];
			hn->ocean = (oceans[hn->continent] > 50);
		}

		for (int iter = 0; iter < 10; iter++)
		{
			for (int i = 0; i < hexNodes.size(); i++)
			{
				HexNode* hn = &hexNodes[i];

				// Now we see how many ocean and land
				// neighbors this node has.
				int oceanNeighbors = 0;
				int landNeighbors = 0;

				for (int j = 0; j < hn->neighbors.size(); j++)
				{
					HexNode* neighbor = &hexNodes[hn->neighbors[j]];

					if (neighbor->ocean) oceanNeighbors++;
					else landNeighbors++;
				}

				int r = rand() % 9 + 1;
				if (!hn->ocean && r < oceanNeighbors) hn->ocean = true;
				else if (hn->ocean && r < landNeighbors) hn->ocean = false;
			}
		}

		for (int i = 0; i < hexNodes.size(); i++)
		{
			HexNode* hn = &hexNodes[i];

			for (int j = 0; j < hn->neighbors.size(); j++)
			{
				HexNode* neighbor = &hexNodes[hn->neighbors[j]];
				if (hn->ocean != neighbor->ocean) hn->fault = true;
			}
		}

		/*
			And *now* we go through and convert to triangles.
		*/
		for (int i = 0; i < hexNodes.size(); i++)
		{
			HexNode* hn = &hexNodes[i];

			float rise = 0.5f;
			if (hn->ocean) rise = radius - (radius * ocean->GetOffset()) - 5.0f;
			glm::vec3 offset = rise * glm::normalize(hn->center);

			// glm::vec3 color = glm::vec3((rand() % 100 + 1) / 100.0f, (rand() % 100 + 1) / 100.0f, (rand() % 100 + 1) / 100.0f);
			glm::vec4 color = glm::vec4(0.05f, 0.73f, 0.28f, 1.0f);
			if (hn->ocean) color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

			// Here, we find the vertices which will make up the corners
			// of the hex.
			std::vector<glm::vec3> verts;
			for (int j = 0; j < hn->neighbors.size(); j++)
			{
				HexNode* neighbor1 = &hexNodes[hn->neighbors[j]];
				HexNode* neighbor2 = &hexNodes[hn->neighbors[(j + 1) % hn->neighbors.size()]];
				bool sameHeight1 = (hn->ocean == neighbor1->ocean);
				bool sameHeight2 = (hn->ocean == neighbor2->ocean);

				glm::vec3 offset1 = sameHeight1 ? rise * glm::normalize(neighbor1->center) : offset;
				glm::vec3 offset2 = sameHeight2 ? rise * glm::normalize(neighbor2->center) : offset;

				glm::vec3 a = hn->center + offset;
				glm::vec3 b = neighbor1->center + offset1;
				glm::vec3 c = neighbor2->center + offset2;
				verts.push_back((a + b + c) / 3.0f);
			}

			// Now we go about actually putting the hex together.
			// First, we add the top of the hex.
			for (int j = 0; j < verts.size(); j++)
			{
				glm::vec3 a = hn->center + offset;
				glm::vec3 b = verts[j];
				glm::vec3 c = verts[(static_cast<unsigned long long>(j) + 1) % verts.size()];

				Triangle t =
				{
					{ a.x, a.y, a.z, color.r, color.g, color.b, color.a },
					{ b.x, b.y, b.z, color.r, color.g, color.b, color.a },
					{ c.x, c.y, c.z, color.r, color.g, color.b, color.a }
				};

				triangles.push_back(t);
			}

			// And now we add the sides.
			// We only do this if necessary.
			if (hn->fault)
			{
				color = glm::vec4(color.r / 2.0f, color.g / 2.0f, color.b / 2.0f, color.a);

				glm::vec3 onset = -((radius / 4.0f) * glm::normalize(hn->center));

				for (int j = 0; j < verts.size(); j++)
				{
					glm::vec3 a = verts[j];
					glm::vec3 b = a + onset;
					glm::vec3 c = verts[(static_cast<unsigned long long>(j) + 1) % verts.size()];
					glm::vec3 d = c + onset;

					Triangle adb =
					{
						{ a.x, a.y, a.z, color.r, color.g, color.b, color.a },
						{ d.x, d.y, d.z, color.r, color.g, color.b, color.a },
						{ b.x, b.y, b.z, color.r, color.g, color.b, color.a }
					};

					Triangle acd =
					{
						{ a.x, a.y, a.z, color.r, color.g, color.b, color.a },
						{ c.x, c.y, c.z, color.r, color.g, color.b, color.a },
						{ d.x, d.y, d.z, color.r, color.g, color.b, color.a }
					};

					triangles.push_back(adb);
					triangles.push_back(acd);
				}
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
			c->upperBounds = glm::vec3(-INFINITY);
			c->lowerBounds = glm::vec3(INFINITY);
			for (int j = i; j < i + allotedHexes; j++)
			{
				HexNode* hn = &hexNodes[j];

				Hex hex =
				{
					c->index,
					j - i,
					hn->fault,
					hn->ocean,
					{}
				};

				for (int k = 0; k < hn->neighbors.size(); k++)
				{
					std::pair<unsigned int, unsigned int> p =
					{
						hn->neighbors[k] / Chunk::MAXHEXES,
						hn->neighbors[k] % Chunk::MAXHEXES
					};

					hex.neighbors.push_back(p);
				}

				// Cull Unnecessary Triangles
				if (hn->neighbors.size() == 5) c->triCount -= 1;
				else if (hn->neighbors.size() == 5 && !hn->fault) c->triCount -= 3;
				else if (!hn->fault) c->triCount -= 12;

				// Calculate Lower and Upper Chunk Bounds
				if (hn->center.x > c->upperBounds.x) c->upperBounds.x = hn->center.x;
				if (hn->center.y > c->upperBounds.y) c->upperBounds.y = hn->center.y;
				if (hn->center.z > c->upperBounds.z) c->upperBounds.z = hn->center.z;
				if (hn->center.x < c->lowerBounds.x) c->lowerBounds.x = hn->center.x;
				if (hn->center.y < c->lowerBounds.y) c->lowerBounds.y = hn->center.y;
				if (hn->center.z < c->lowerBounds.z) c->lowerBounds.z = hn->center.z;

				// Find Center
				c->center += hexNodes[j].center;

				// Done
				c->hexes[j - i] = hex;
			}

			c->center /= allotedHexes;
			c->center = radius * glm::normalize(c->center);

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
		triangles.clear();
		hexNodes.clear();
	}

	Planet::Planet(unsigned int worldSize)
	{
		this->worldSize = worldSize;

		srand(time(NULL));

		Polyhedron* polyhedron = new Polyhedron(worldSize);
		this->radius = polyhedron->radius;

		for (int i = 0; i < worldSize; i++) polyhedron->Subdivide();

		ocean = new Ocean(polyhedron);


		std::vector<HexNode> hexNodes = Hexify(polyhedron);
		hexNodes = SortNeighbors(hexNodes);
		GenerateGeometry(hexNodes);
		delete polyhedron;
	}

	Planet::~Planet()
	{
		delete ocean;
	}
}