#include "river.h"

namespace Mandalin
{
	std::vector<HexNode> RiverSystem::GenerateRivers(std::vector<HexNode> hexNodes)
	{
		/*
			First, we're going to generate a gradient across all the tiles
			which will allow us to simplify the river-flowing process.
			In short, we want to start with our mountains and then iterate
			outward, assigning each tile a "distance-from-mountains"
			value. Then, when we create a new river, we'll have it move
			down progressively away from mountains.
		*/
		std::vector<unsigned int> checked;
		for (int i = 0; i < hexNodes.size(); i++)
		{
			HexNode* hn = &hexNodes[i];

			if (hn->biome == Biome::mountain)
			{
				checked.push_back(i);
				hn->mountainDistance = 0;
			}
		}

		while (true)
		{
			std::vector<unsigned int> newAdditions;
			
			for (int i = 0; i < checked.size(); i++)
			{
				HexNode* hn = &hexNodes[checked[i]];

				for (int j = 0; j < hn->neighbors.size(); j++)
				{
					HexNode* neigh = &hexNodes[hn->neighbors[j]];

					if (neigh->mountainDistance == -1)
					{
						neigh->mountainDistance = hn->mountainDistance + 1;
						newAdditions.push_back(hn->neighbors[j]);
					}
				}
			}

			if (newAdditions.size() == 0) break;
			checked.clear();
			for (int i = 0; i < newAdditions.size(); i++) checked.push_back(newAdditions[i]);
		}

		for (int riverID = 0; riverID < Settings::RiverCount; riverID++)
		{
			River river =
			{
				riverID,
				{}
			};

			unsigned int maxIterations = Settings::MaxRiverIterations;
			while (true)
			{
				/*
					RESUME HERE!!!
				*/

				if (maxIterations <= 0) break;
				maxIterations--;
			}

			rivers.push_back(river);
		}

		return hexNodes;
	}
}