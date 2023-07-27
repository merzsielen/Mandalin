#ifndef WORLD_H
#define WORLD_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>

#include "chunk.h"
#include "../util/geometry.h"

namespace Mandalin
{
	/*-------------------------------------------------*/
	/* Planet                                          */
	/*-------------------------------------------------*/
	/*
		I figure we might as well try to work in support
		for multiple planets, just in case that becomes
		a thing we want to play with later.
	*/
	class Planet
	{
	private:
		/*-----------------------------------------------*/
		/* Position */
		/*-----------------------------------------------*/
		glm::vec3				position = glm::vec3(0.0f, 0.0f, 0.0f);
		float					radius;
		unsigned int			totalTriangles;

		/*-----------------------------------------------*/
		/* Chunks */
		/*-----------------------------------------------*/
		/*
			Given that we want to be able to (in theory)
			support an arbitrary number of tiles, we need to
			use a vector to hold our chunks. This is less
			than ideal, so we may change this later.
		*/
		std::vector<Chunk>		chunks;

		/*-----------------------------------------------*/
		/* World Generation Options */
		/*-----------------------------------------------*/
		unsigned int			worldSize;

	public:
		/*-----------------------------------------------*/
		/* Variable Functions */
		/*-----------------------------------------------*/
		glm::vec3				GetPosition() { return position; }
		float					GetRadius() { return radius; }
		unsigned int			GetWorldSize() { return worldSize; }
		unsigned int			GetTotalTriangles() { return totalTriangles; }

		/*-----------------------------------------------*/
		/* Chunks, Cont. */
		/*-----------------------------------------------*/
		unsigned int			ChunkCount() { return chunks.size(); }
		Chunk*					GetChunk(unsigned int i) { return &chunks[i]; }
		void					SortChunks();

		/*-----------------------------------------------*/
		/* World Generation */
		/*-----------------------------------------------*/
		/*
			Here is where we are going to outline the
			functions necessary for basic world generation.
			First, we need to generate the chunks and hexes
			of which the world is composed.

			(Next follows tectonic plates, temperature,
			wind, currents, biomes, etc.)
		*/
		/*---------------------*/
		/* Geometry Generation */
		/*---------------------*/
		void					Hexify(Polyhedron* polyhedron);

		/*---------------------*/
		/* Constructor         */
		/*---------------------*/
		Planet(unsigned int worldSize);
	};
}

#endif