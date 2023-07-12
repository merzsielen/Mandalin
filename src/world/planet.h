#ifndef WORLD_H
#define WORLD_H

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
		/* Chunks */
		/*-----------------------------------------------*/
		/*
			Given that we want to be able to (in theory)
			support an arbitrary number of tiles, we need to
			use a vector to hold our chunks. This is less
			than ideal, so we may change this later.
		*/
		std::vector<Chunk>			chunks;

		/*-----------------------------------------------*/
		/* World Generation Options */
		/*-----------------------------------------------*/
		unsigned int				worldSize;

	public:
		/*-----------------------------------------------*/
		/* Chunks, Cont. */
		/*-----------------------------------------------*/
		unsigned int				ChunkCount() { return chunks.size(); }
		Chunk*						GetChunk(unsigned int i) { return &chunks[i]; }

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
		void						Hexify(std::vector<TriFace> in);

		/*---------------------*/
		/* Constructor         */
		/*---------------------*/
		Planet(unsigned int worldSize);
	};
}

#endif