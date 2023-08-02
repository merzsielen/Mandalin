#include "biomes.h"

namespace Mandalin
{
	std::vector<HexNode> GenerateBiomes(std::vector<HexNode> hexNodes)
	{
		/*
			We should first mark off our oceans and mountains;
			then, we can go through and find all our other biomes.

			This is going to be a multi-stage process.
		*/
		for (int i = 0; i < hexNodes.size(); i++)
		{
			HexNode* hn = &hexNodes[i];

			if (hn->ocean) hn->biome = Biome::ocean;
			else if (hn->faultNeighbor || (hn->fault && hn->oceanNeighbor)) hn->biome = Biome::highlands;
			else if (hn->fault) hn->biome = Biome::mountain;
		}
	}
}