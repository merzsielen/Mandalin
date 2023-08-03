#ifndef HEX_H
#define HEX_H

#include <vector>

namespace Mandalin
{
	/*-------------------------------------------------*/
	/* Focuses                                         */
	/*-------------------------------------------------*/
	enum class Focus { primary, biome, tectonicPlate, temperature, rainfall, population, language };

	/*-------------------------------------------------*/
	/* Biomes                                          */
	/*-------------------------------------------------*/
	/*
		There are going to be a number of different
		biomes, as you might imagine.
	*/
	enum class Biome { ocean, mountain, highlands, desert, steppe, savanna, dryforest, broadleafforest, rainforest, tundra, taiga, mediterranean, oceanic };

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
		// Information about its chunk and its location
		// within it.
		unsigned int	chunk;
		unsigned int	index;

		// Biomes
		Biome			biome;
		unsigned int	biomeVariation; // 0 - 4

		// Region, Continent, & Plate
		unsigned int	region;
		unsigned int	continent;
		unsigned int	tectonicPlate;

		// Majorities
		unsigned int	populationID;
		unsigned int	languageID;

		// Variables
		unsigned int	population;

		// Associated Tris
		unsigned int	tris;
		unsigned int	trisIndex;
		
		// The chunk indices of each neighbor followed by
		// their indices within that chunk.
		std::vector<std::pair<unsigned int, unsigned int>>	neighbors;
	};
}

#endif