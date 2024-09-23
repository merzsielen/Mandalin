#include "planet.h"

#include <time.h>
#include <iostream>
#include <algorithm>

namespace Mandalin
{
	/*-----------------------------------------------*/
	/* Utility */
	/*-----------------------------------------------*/
	Hex* Planet::GetHex(unsigned int chunk, unsigned int index)
	{
		if (chunk > chunks.size() || index > Settings::ChunkMaxHexes) return nullptr;
		return &chunks[chunk].hexes[index];
	}

	void Planet::SetPopulation(unsigned int chunk, unsigned int hex, int population)
	{
		Chunk* c = &chunks[chunk];
		Hex* h = &c->hexes[hex];

		h->populationID = population;

		glBindVertexArray(c->vao);
		glBindBuffer(GL_ARRAY_BUFFER, c->vbo);

		int triStart = h->trisIndex * sizeof(Triangle);
		int popOffset = offsetof(Vertex, population);

		for (int i = 0; i < h->tris; i++)
		{
			
			int triOffset = i * sizeof(Triangle);

			for (int j = 0; j < 3; j++)
			{
				int vertOffset = j * sizeof(Vertex);
				glBufferSubData(GL_ARRAY_BUFFER, triStart + triOffset + vertOffset + popOffset, sizeof(int), &population);
			}
		}

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void Planet::SetLanguage(unsigned int chunk, unsigned int hex, int language)
	{
		Chunk* c = &chunks[chunk];
		Hex* h = &c->hexes[hex];

		h->languageID = language;

		glBindVertexArray(c->vao);
		glBindBuffer(GL_ARRAY_BUFFER, c->vbo);

		int triStart = h->trisIndex * sizeof(Triangle);
		int langOffset = offsetof(Vertex, language);

		for (int i = 0; i < h->tris; i++)
		{
			int triOffset = i * sizeof(Triangle);

			for (int j = 0; j < 3; j++)
			{
				int vertOffset = j * sizeof(Vertex);
				glBufferSubData(GL_ARRAY_BUFFER, triStart + triOffset + vertOffset + langOffset, sizeof(int), &language);
			}
		}

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	/*-----------------------------------------------*/
	/* World Generation */
	/*-----------------------------------------------*/
	float Planet::GetRise(Biome biome)
	{
		if (biome == Biome::ocean) return (radius - (radius * Settings::OceanOffset) - 5.0f) * (3.0f / worldSize);
		else if (biome == Biome::highlands) return 0.95f * (3.0f / worldSize);
		else if (biome == Biome::mountain) return 1.5f * (3.0f / worldSize);

		return 0.5f * (3.0f / worldSize);
	}

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

		hexNodes = VoronoiSort(hexNodes);

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

	std::vector<HexNode> Planet::GenerateTopology(std::vector<HexNode> hexNodes)
	{
		/*
			Now, we go through and find the biome of each
			area.

			First, this involves finding our oceans which are
			mostly randomly picked from amongst our "continents."
		*/

		std::vector<int> oceans;

		for (int i = 0; i < Settings::ContinentCount; i++)
		{
			oceans.push_back((rand() % 100) + 1);
		}

		/*
			Before we find our biomes, we're going
			to roughen up the coastlines a bit.
			Sadly, this means looping over the nodes a few too
			many times.

			We can also take this moment to set all the faults.
		*/
		for (int i = 0; i < hexNodes.size(); i++)
		{
			HexNode* hn = &hexNodes[i];
			hn->ocean = (oceans[hn->continent] > 50);
		}

		for (int iter = 0; iter < Settings::CoastRoughingIterations; iter++)
		{
			for (int i = 0; i < hexNodes.size(); i++)
			{
				HexNode* hn = &hexNodes[i];

				// Now we see how many ocean and land
				// neighbors this node has.
				int oceanNeighbors = 0;
				int landNeighbors = 0;
				int nonPlateNeighbors = 0;

				for (int j = 0; j < hn->neighbors.size(); j++)
				{
					HexNode* neighbor = &hexNodes[hn->neighbors[j]];

					if (neighbor->ocean) oceanNeighbors++;
					else landNeighbors++;

					if (hn->tectonicPlate != neighbor->tectonicPlate) nonPlateNeighbors++;
				}

				int r = rand() % 9 + 1;
				if (!hn->ocean && r < oceanNeighbors) hn->ocean = true;
				else if (hn->ocean && r < landNeighbors) hn->ocean = false;

				if (nonPlateNeighbors != 0) hn->fault = true;
			}
		}

		/*
			Now that we have our oceans in place, we can
			find all our highlands and mountains.
		*/
		for (int i = 0; i < hexNodes.size(); i++)
		{
			HexNode* hn = &hexNodes[i];

			for (int j = 0; j < hn->neighbors.size(); j++)
			{
				HexNode* neighbor = &hexNodes[hn->neighbors[j]];
				if (hn->ocean != neighbor->ocean) hn->oceanNeighbor = true;
				if (!hn->fault && neighbor->fault) hn->faultNeighbor = true;
			}

			if (hn->ocean) hn->biomeVariation = 0;
			else hn->biomeVariation = rand() % 5;
		}

		return hexNodes;
	}

	void Planet::GenerateGeometry(std::vector<HexNode> hexNodes)
	{
		srand(time(NULL));

		/*
			Now, we go through and convert to triangles.
		*/
		std::vector<Triangle> triangles;

		for (int i = 0; i < hexNodes.size(); i++)
		{
			HexNode* hn = &hexNodes[i];
			glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			glm::vec4 sideColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);

			float rise = GetRise(hn->biome);
			glm::vec3 offset = rise * glm::normalize(hn->center);

			int sideBiomeVariation = (((int)hn->biome * 5) - 4);
			if (sideBiomeVariation < 0) sideBiomeVariation = 0;
			int biomeVariation = sideBiomeVariation + hn->biomeVariation;

			std::vector<std::pair<unsigned int, unsigned int>> riverPartners;
			if (hn->rivers)
			{
				for (int j = 0; j < rivers->RiverCount(); j++)
				{
					for (int k = 0; k < rivers->GetRiver(j)->nodes.size(); k++)
					{
						RiverNode* rivNode = &rivers->GetRiver(j)->nodes[k];

						if (rivNode->a == i) riverPartners.push_back(std::pair<unsigned int, unsigned int>(rivNode->b, rivNode->c));
						else if (rivNode->b == i) riverPartners.push_back(std::pair<unsigned int, unsigned int>(rivNode->a, rivNode->c));
						else if (rivNode->c == i) riverPartners.push_back(std::pair<unsigned int, unsigned int>(rivNode->a, rivNode->b));
					}
				}
			}

			// Here, we find the vertices which will make up the corners
			// of the hex.
			std::vector<glm::vec3> verts;
			for (int j = 0; j < hn->neighbors.size(); j++)
			{
				HexNode* neighbor1 = &hexNodes[hn->neighbors[j]];
				HexNode* neighbor2 = &hexNodes[hn->neighbors[(j + 1) % hn->neighbors.size()]];

				bool river = false;
				if (hn->rivers)
				{
					for (int k = 0; k < riverPartners.size(); k++)
					{
						if ((riverPartners[k].first == neighbor1->index &&
							riverPartners[k].second == neighbor2->index) ||
							(riverPartners[k].first == neighbor2->index &&
							riverPartners[k].second == neighbor1->index))
						{
							river = true;
						}
					}
				}

				glm::vec3 offset1 = rise * glm::normalize(neighbor1->center);
				glm::vec3 offset2 = rise * glm::normalize(neighbor2->center);

				glm::vec3 a = hn->center + offset;
				glm::vec3 b = neighbor1->center + offset1;
				glm::vec3 c = neighbor2->center + offset2;

				float total = 3.0f;
				float bcMod = 1.0f;

				if (river)
				{
					bcMod = 0.5f;
					total = 2.0f;
				}

				glm::vec3 vert = (a + b * bcMod + c * bcMod) / total;
				verts.push_back(vert);
			}

			// Quickly, we need to calculate some things.
			float temp = (hn->temperature + 25) / 55.0f;
			float rain = hn->rainfall / 5000.0f;

			// Now we go about actually putting the hex together.
			// First, we add the top of the hex.
			for (int j = 0; j < verts.size(); j++)
			{
				glm::vec3 a = hn->center + offset;
				glm::vec3 b = verts[j];
				glm::vec3 c = verts[(static_cast<unsigned long long>(j) + 1) % verts.size()];

				Triangle t =
				{
					{ a.x, a.y, a.z, color.r, color.g, color.b, color.a, 0, biomeVariation, hn->tectonicPlate, temp, rain, 0, 0 },
					{ b.x, b.y, b.z, color.r, color.g, color.b, color.a, 0, biomeVariation, hn->tectonicPlate, temp, rain, 0, 0 },
					{ c.x, c.y, c.z, color.r, color.g, color.b, color.a, 0, biomeVariation, hn->tectonicPlate, temp, rain, 0, 0 }
				};

				triangles.push_back(t);
				hn->tris++;
			}

			// And now we add the sides.
			// We only do this if necessary.
			if (hn->oceanNeighbor || hn->biome == Biome::mountain || hn->biome == Biome::highlands || hn->rivers)
			{
				glm::vec3 onset = -((radius / 4.0f) * glm::normalize(hn->center));

				if (hn->rivers) sideColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

				for (int j = 0; j < verts.size(); j++)
				{
					glm::vec3 a = verts[j];
					glm::vec3 b = a + onset;
					glm::vec3 c = verts[(static_cast<unsigned long long>(j) + 1) % verts.size()];
					glm::vec3 d = c + onset;

					Triangle adb =
					{
						{ a.x, a.y, a.z, sideColor.r, sideColor.g, sideColor.b, sideColor.a, (int)hn->rivers, sideBiomeVariation, hn->tectonicPlate, temp, rain, 0, 0 },
						{ d.x, d.y, d.z, sideColor.r, sideColor.g, sideColor.b, sideColor.a, (int)hn->rivers, sideBiomeVariation, hn->tectonicPlate, temp, rain, 0, 0 },
						{ b.x, b.y, b.z, sideColor.r, sideColor.g, sideColor.b, sideColor.a, (int)hn->rivers, sideBiomeVariation, hn->tectonicPlate, temp, rain, 0, 0 }
					};

					Triangle acd =
					{
						{ a.x, a.y, a.z, sideColor.r, sideColor.g, sideColor.b, sideColor.a, (int)hn->rivers, sideBiomeVariation, hn->tectonicPlate, temp, rain, 0, 0 },
						{ c.x, c.y, c.z, sideColor.r, sideColor.g, sideColor.b, sideColor.a, (int)hn->rivers, sideBiomeVariation, hn->tectonicPlate, temp, rain, 0, 0 },
						{ d.x, d.y, d.z, sideColor.r, sideColor.g, sideColor.b, sideColor.a, (int)hn->rivers, sideBiomeVariation, hn->tectonicPlate, temp, rain, 0, 0 }
					};

					triangles.push_back(adb);
					triangles.push_back(acd);
					hn->tris += 2;
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
		for (int i = 0; i < hexNodes.size(); i += Settings::ChunkMaxHexes)
		{
			unsigned int allotedHexes = std::min(Settings::ChunkMaxHexes, (unsigned int)hexNodes.size() - i);

			chunks.push_back({});
			Chunk* c = &chunks[chunks.size() - 1];

			c->index = chunks.size() - 1;
			c->hexCount = allotedHexes;
			c->triCount = 0;

			c->center = glm::vec3(0.0f, 0.0f, 0.0f);
			for (int j = i; j < i + allotedHexes; j++)
			{
				HexNode* hn = &hexNodes[j];

				Hex hex =
				{
					c->index,
					j - i,
					hn->biome,
					hn->biomeVariation,
					hn->region,
					hn->continent,
					hn->tectonicPlate,
					0,
					0,
					0,
					hn->tris,
					c->triCount,
					{}
				};

				for (int k = 0; k < hn->neighbors.size(); k++)
				{
					std::pair<unsigned int, unsigned int> p =
					{
						hn->neighbors[k] / Settings::ChunkMaxHexes,
						hn->neighbors[k] % Settings::ChunkMaxHexes
					};

					hex.neighbors.push_back(p);
				}

				// Add Necessary Triangles
				c->triCount += hn->tris;

				// Find Center
				c->center += hn->center;

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

			// Rivers
			glVertexAttribIPointer(2, 1, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, river));
			glEnableVertexAttribArray(2);

			// Biome Color Index
			glVertexAttribIPointer(3, 1, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, biome));
			glEnableVertexAttribArray(3);

			// Tectonic Plate Color Index
			glVertexAttribIPointer(4, 1, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, tectonicPlate));
			glEnableVertexAttribArray(4);

			// Temperature
			glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, temperature));
			glEnableVertexAttribArray(5);

			// Rainfall
			glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, rainfall));
			glEnableVertexAttribArray(6);

			// Population Index
			glVertexAttribIPointer(7, 1, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, population));
			glEnableVertexAttribArray(7);

			// Language Index
			glVertexAttribIPointer(8, 1, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, language));
			glEnableVertexAttribArray(8);

			unsigned int indices[Settings::ChunkMaxTris * 3];
			for (int i = 0; i < Settings::ChunkMaxTris; i++)
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
		rivers = new RiverSystem();

		std::vector<HexNode> hexNodes = Hexify(polyhedron);
		hexNodes = SortNeighbors(hexNodes);
		hexNodes = GenerateTopology(hexNodes);
		hexNodes = GenerateBiomes(hexNodes, position, radius);
		hexNodes = rivers->GenerateRivers(hexNodes);
		GenerateGeometry(hexNodes);
		delete polyhedron;
	}

	Planet::~Planet()
	{
		delete ocean;
	}
}