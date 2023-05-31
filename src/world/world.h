#ifndef WORLD_H
#define WORLD_H

#include <glm/glm.hpp>

namespace Mandalin
{
	/*-------------------------------------------------*/
	/* Chunk                                           */
	/*-------------------------------------------------*/
	/*
		Chunks exist solely to minimize the number of
		tiles we send to the renderer each frame. They
		will *only* contain the vertex data of hexes.
	*/
	#define CHUNKSIZE 1000; // This is currently arbitrary.
	struct Chunk
	{
		unsigned int	index;
		glm::vec3 vertices[CHUNKSIZE];
	};

	/*-------------------------------------------------*/
	/* Hex                                             */
	/*-------------------------------------------------*/
	/*
		The hex is going to be the basic unit of land
		in the game. While 99% of hexes are actually
		hexes, there will be twelve pentagons left over
		due to the fact that you can't perfectly tile
		a sphere with hexes. We could make these special
		or simply treat them as any other tile. Having
		kinda done this before, I can say that the
		treating them as the same as the other hexes
		worked fine in the past.
	*/
	struct Hex
	{
		// The indices of the six neighbors each hex
		// has (or five, if a pentagon).
		unsigned int	neighbors[6];
	};

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
		// An array of all the hexes, I guess.
		// I feel like I'll rewrite this later.
		Hex				hexes[10];

		// We'll also include an array of all
		// the chunks.
		
	};
}

#endif