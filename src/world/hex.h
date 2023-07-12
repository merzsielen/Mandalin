#ifndef HEX_H
#define HEX_H

namespace Mandalin
{
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
		// The chunk indices of each neighbor followed by
		// their indices within that chunk.
		std::vector<std::pair<unsigned int, unsigned int>>	neighbors;

		// Information about its chunk and its location
		// within it.
		unsigned int	chunk;
		unsigned int	index;
	};
}

#endif