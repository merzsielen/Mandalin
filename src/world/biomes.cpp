#include "biomes.h"
#include <time.h>

namespace Mandalin
{
	std::vector<HexNode> GenerateClimate(std::vector<HexNode> hexNodes, glm::vec3 planetPosition, float planetRadius)
	{
		for (int i = 0; i < hexNodes.size(); i++)
		{
			HexNode* hn = &hexNodes[i];

			float unweightedLatitude = abs(hn->center.y - planetPosition.y) / planetRadius;

			hn->latitude = 0.9f * unweightedLatitude;
			hn->temperature = Lerp(30.0f, -25.0f, unweightedLatitude);
			hn->rainfall = Lerp(5000.0f, 0.0f, unweightedLatitude);

			if (hn->biome == Biome::mountain)
			{
				hn->temperature *= 0.6f;
				hn->rainfall *= 0.5f;
			}
			else if (hn->biome == Biome::highlands)
			{
				hn->temperature *= 0.8f;
				hn->rainfall *= 0.9f;
			}
		}

		return hexNodes;
	}

	std::vector<HexNode> GenerateBiomes(std::vector<HexNode> hexNodes, glm::vec3 planetPosition, float planetRadius)
	{
		srand(time(NULL));

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

		/*
			Quickly, we're going to make our mountains a little larger.
		*/
		for (int iter = 0; iter < Settings::MountainSpreadingIterations; iter++)
		{
			for (int i = 0; i < hexNodes.size(); i++)
			{
				HexNode* hn = &hexNodes[i];

				if (hn->biome != Biome::highlands &&
					hn->biome != Biome::mountain) continue;

				for (int j = 0; j < hn->neighbors.size(); j++)
				{
					HexNode* neighbor = &hexNodes[hn->neighbors[j]];

					if (neighbor->biome == Biome::ocean ||
						neighbor->biome == Biome::mountain) continue;

					if (neighbor->oceanNeighbor &&
						hn->biome == Biome::mountain) continue;

					if (neighbor->biome == hn->biome) continue;

					int r = rand() % 100 + 1;
					if (hn->biome == Biome::mountain) r *= 0.6f;

					if (neighbor->biome != hn->biome && r > 50)
					{
						neighbor->biome = hn->biome;
						break;
					}
				}
			}
		}

		hexNodes = GenerateClimate(hexNodes, planetPosition, planetRadius);

		return hexNodes;
	}
}