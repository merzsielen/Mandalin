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

		/*
			First, we go through all our vertices.
		*/
		std::vector<VertNode> vertexNodes;
		for (int i = 0; i < polyhedron->vertices.size(); i++)
		{
			PolyVert* pv = &polyhedron->vertices[i];

			/*
				Each vertex node is composed of an index of the
				vertex and the indices of the triangles which
				share that vertex.
			*/

			VertNode vn =
			{
				(pv->sharers.size() == 6),	// Does the vertex have six triangles that share it?
				i,
				{}
			};

			std::copy(pv->sharers.begin(), pv->sharers.end(), vn.neighborIndices);
			vertexNodes.push_back(vn);
		}

		std::cout << "Generated " << vertexNodes.size() << " vertices." << std::endl;
		
		/*
			Now, we go through all our triangles.
		*/
		std::vector<TriNode> triangleNodes;
		for (int i = 0; i < polyhedron->faces.size(); i++)
		{
			TriFace* f = &polyhedron->faces[i];
			TriNode tn = {};

			/*
				Each triangle node is composed of the index
				of its triangle, the indices of its vertices...
			*/
			tn.triIndex = i;
			tn.vertNeighborIndices[0] = f->a;
			tn.vertNeighborIndices[1] = f->b;
			tn.vertNeighborIndices[2] = f->c;

			/*
				... and the indices of the other triangles which
				share two of its vertices. These are a little harder
				to find.
			*/
			int neighborsFound = 0;
			for (int j = 0; j < polyhedron->vertices[f->a].sharers.size(); j++)
			{
				TriFace* n = &polyhedron->faces[polyhedron->vertices[f->a].sharers[j]];
				if (n == f) continue;

				int shared = 0;
				if (n->a == f->a || n->a == f->b || n->a == f->c) shared++;
				if (n->b == f->a || n->b == f->b || n->b == f->c) shared++;
				if (n->c == f->a || n->c == f->b || n->c == f->c) shared++;

				if (shared > 1) tn.triNeighborIndices[neighborsFound++] = j;
			}

			triangleNodes.push_back(tn);
		}

		std::cout << "Generated " << triangleNodes.size() << " faces." << std::endl;

		/*
			Now that we have all our vertex and triangle nodes.
			We know two things:
				1. The indices of the neighbor hexes that derive
				from triangle nodes will always be the indices of
				the triangles plus the total number of hexes we
				derive from vertices. In other words:
					
					newHexIndex = vertexNodes.size() + oldTriangleIndex;

				2. Similarly, the indices of the neighbor hexes
				derived from vertex nodes will always just be the
				index of the old vertex.

					newHexIndex = oldVertexIndex;
		*/

		std::vector<HexNode> hexNodes;
		
		for (int i = 0; i < vertexNodes.size(); i++)
		{
			VertNode* vn = &vertexNodes[i];
			glm::vec3 center = polyhedron->radius * glm::normalize(polyhedron->vertices[vn->vertIndex].vertex);
			HexNode hn = { i, center, {} };
			hn.neighbors.push_back(vertexNodes.size() + vn->neighborIndices[0]);
			hn.neighbors.push_back(vertexNodes.size() + vn->neighborIndices[1]);
			hn.neighbors.push_back(vertexNodes.size() + vn->neighborIndices[2]);
			hn.neighbors.push_back(vertexNodes.size() + vn->neighborIndices[3]);
			hn.neighbors.push_back(vertexNodes.size() + vn->neighborIndices[4]);
			if (!vn->pentagon) hn.neighbors.push_back(vertexNodes.size() + vn->neighborIndices[5]);
			hexNodes.push_back(hn);
		}

		for (int i = 0; i < triangleNodes.size(); i++)
		{
			TriNode* tn = &triangleNodes[i];

			glm::vec3 center = polyhedron->vertices[polyhedron->faces[tn->triIndex].a].vertex;
			center += polyhedron->vertices[polyhedron->faces[tn->triIndex].b].vertex;
			center += polyhedron->vertices[polyhedron->faces[tn->triIndex].c].vertex;
			center /= 3.0f;
			center = polyhedron->radius * glm::normalize(center);

			HexNode hn = { vertexNodes.size() + i, center, {} };
			hn.neighbors.push_back(tn->vertNeighborIndices[0]);
			hn.neighbors.push_back(tn->vertNeighborIndices[1]);
			hn.neighbors.push_back(tn->vertNeighborIndices[2]);
			hn.neighbors.push_back(vertexNodes.size() + tn->triNeighborIndices[0]);
			hn.neighbors.push_back(vertexNodes.size() + tn->triNeighborIndices[1]);
			hn.neighbors.push_back(vertexNodes.size() + tn->triNeighborIndices[2]);
			hexNodes.push_back(hn);
		}

		std::cout << "Generated " << hexNodes.size() << " hexes." << std::endl;
		
		//for (int i = 0; i < hexNodes.size(); i++)
		//{
		//	/*
		//		The vertices of each hex are at the average of
		//		the center and two of the hex's neighbors' centers.
		//		This means we have to sort the neighbors.
		//	*/

		//	HexNode* hn = &hexNodes[i];

		//	/*
		//		First, we find the neighbor closest to the north pole
		//		(kinda and only maybe, but we only need it to be kinda
		//		and maybe).
		//	*/
		//	glm::vec3 center = hn->center;
		//	glm::vec3 pole = glm::vec3(0.0f, polyhedron->radius, 0.0f);
		//	glm::vec3 up = center + pole;
		//	pole = glm::vec3(polyhedron->radius, 0.0f, 0.0f);
		//	glm::vec3 right = center + pole;

		//	unsigned int closestUpNeighbor = 0;
		//	unsigned int closestRightNeighbor = 0;
		//	float closestUpDistance = INFINITY;
		//	float closestRightDistance = INFINITY;

		//	for (int j = 0; j < hn->neighbors.size(); j++)
		//	{
		//		HexNode* neighbor = &hexNodes[hn->neighbors[j]];

		//		float upDistance = glm::distance(neighbor->center, up);
		//		float rightDistance = glm::distance(neighbor->center, right);

		//		if (upDistance < closestUpDistance)
		//		{
		//			closestUpNeighbor = j;
		//			closestUpDistance = upDistance;
		//		}
		//		if (rightDistance < closestRightDistance)
		//		{
		//			closestRightNeighbor = j;
		//			closestRightDistance = rightDistance;
		//		}
		//	}

		//	/*
		//		Now, we can go through and rotate clockwise around to find
		//		the other neighbors.
		//	*/

		//	std::vector<unsigned int> sortedNeighbors = { closestUpNeighbor, closestRightNeighbor };
		//	HexNode* currentNeighbor = &hexNodes[closestRightNeighbor];

		//	int ind = 0;
		//	while (true)
		//	{
		//		/*
		//			Take the newest added neighbor and find the neighbor it
		//			shares with the center hex which isn't the preceding
		//			node added to the vector.
		//		*/

		//		unsigned int previousNeighborIndex = sortedNeighbors[ind++];
		//		HexNode* currentNeighbor = &hexNodes[sortedNeighbors[ind]];
		//		int nextNeighborIndex = -1;

		//		for (int j = 0; j < currentNeighbor->neighbors.size(); j++)
		//		{
		//			// Get the index of this neighbor's neighbor.
		//			unsigned int potentialNeighborIndex = currentNeighbor->neighbors[j];

		//			// If this is the center or the previous neighbor, just continue.
		//			if (potentialNeighborIndex == i ||
		//				potentialNeighborIndex == previousNeighborIndex) continue;

		//			// Now check if the center shares any neighbors with it.
		//			for (int k = 0; k < hn->neighbors.size(); k++)
		//			{
		//				if (potentialNeighborIndex == hn->neighbors[k])
		//				{
		//					nextNeighborIndex = hn->neighbors[k];
		//				}
		//			}
		//		}

		//		if (nextNeighborIndex == -1) break;
		//		sortedNeighbors.push_back(nextNeighborIndex);
		//	}

		//	hn->neighbors = sortedNeighbors;
		//}

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
				glm::vec3 b = hexNodes[hn->neighbors[j]].center;
				glm::vec3 c = hexNodes[hn->neighbors[(j + 1) % hn->neighbors.size()]].center;
				verts.push_back(polyhedron->radius * glm::normalize((b + c + hn->center) / 3.0f));
			}

			for (int j = 0; j < hn->neighbors.size(); j++)
			{
				glm::vec3 a = hn->center;
				/*glm::vec3 b = verts[j];
				glm::vec3 c = verts[(static_cast<unsigned long long>(j) + 1) % verts.size()];*/
				glm::vec3 b = hexNodes[hn->neighbors[j]].center;
				glm::vec3 c = hexNodes[hn->neighbors[(j + 1) % hn->neighbors.size()]].center;

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

		std::vector<Triangle> triangles;

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
		}

		// Hexify(polyhedron);
	}
}