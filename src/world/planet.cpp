#include "planet.h"
#include <time.h>
#include <iostream>

namespace Mandalin
{
	bool CheckNode(TempNode* node, unsigned int aID, unsigned int bID, unsigned int cID)
	{
		if (node != nullptr)
		{
			bool foundB = false;
			bool foundC = false;

			for (int j = 0; j < node->neighborIDs.size(); j++)
			{
				if (node->neighborIDs[j] == bID) foundB = true;
				else if (node->neighborIDs[j] == cID) foundC = true;
			}

			if (!foundB) node->neighborIDs.push_back(bID);
			if (!foundC) node->neighborIDs.push_back(cID);

			return true;
		}
		
		return false;
	}

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
			So, we are given a list of triangles (with redundant
			vertices) and we are tasked with doing a few things:
			
				- Find all unique vertices.
				- Find all the triangles which share each vertex.
				- Use this information to create all the hexes.
				- Divide them into chunks.
				- Turn each hex into a hex-column.
				- Find all their neighbors.
		*/

		/*
			First, we need to find all our hexes.
			Luckily (and hopefully), all our triangles
			have unique ids. We can use these to find all the
			triangles that share vertices and from these
			construct our hexes.
		*/

		// We need a list of all the ids we've seen so far (and it needs
		// to be the proper size).
		std::vector<TempNode> tempNodes = std::vector<TempNode>(12 * pow(2, worldSize));

		for (int i = 0; i < polyhedron->faces.size(); i++)
		{
			// First, we go through each triangle
			// and grab each vertex and its ID.

			glm::vec3 a = polyhedron->faces[i].vertices[0];
			glm::vec3 b = polyhedron->faces[i].vertices[1];
			glm::vec3 c = polyhedron->faces[i].vertices[2];

			unsigned int aID = polyhedron->faces[i].vertIDS[0];
			unsigned int bID = polyhedron->faces[i].vertIDS[1];
			unsigned int cID = polyhedron->faces[i].vertIDS[2];

			TempNode* aNode = &tempNodes[aID];
			TempNode* bNode = &tempNodes[bID];
			TempNode* cNode = &tempNodes[cID];

			bool foundA = CheckNode(aNode, aID, bID, cID);
			bool foundB = CheckNode(bNode, bID, aID, cID);
			bool foundC = CheckNode(cNode, cID, aID, bID);

			if (!foundA) tempNodes[aID] = { a, aID, { bID, cID } };
			if (!foundB) tempNodes[bID] = { b, bID, { aID, cID } };
			if (!foundC) tempNodes[cID] = { c, cID, { aID, bID } };
		}

		// std::vector<Hex> hexes;

		for (int i = 0; i < tempNodes.size(); i++)
		{
			
		}
	}

	Planet::Planet(unsigned int worldSize)
	{
		this->worldSize = worldSize;

		srand(time(NULL));

		Polyhedron* polyhedron = new Polyhedron(worldSize);

		for (int i = 0; i < worldSize; i++) polyhedron->Subdivide();

		std::vector<Mandalin::Triangle> triangles;

		for (int i = 0; i < polyhedron->faces.size(); i++)
		{
			Mandalin::Triangle t;

			glm::vec3 a = polyhedron->faces[i].vertices[0];
			glm::vec3 b = polyhedron->faces[i].vertices[1];
			glm::vec3 c = polyhedron->faces[i].vertices[2];

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

		Hexify(polyhedron);
	}
}