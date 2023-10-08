#include "biomes.h"
#include <time.h>
#include <iostream>

namespace Mandalin
{
	unsigned int FindNearestLateralNeighbor(HexNode node, std::vector<HexNode> neighbors, glm::vec3 target)
	{
		float minDist = INFINITY;
		unsigned int minIndex = 0;

		for (int i = 0; i < neighbors.size(); i++)
		{
			float dist = glm::distance(neighbors[i].center, target);
			
			if (dist < minDist && abs(neighbors[i].latitude - node.latitude) < 0.01f)
			{
				minIndex = neighbors[i].index;
				minDist = dist;
			}
		}

		return minIndex;
	}

	std::vector<HexNode> SmoothClimate(std::vector<HexNode> hexNodes)
	{
		/*
			Now, we're going to smooth rainfall and temp
			out and clamp their values.
		*/
		std::vector<float> temps;
		std::vector<float> rains;

		for (int i = 0; i < hexNodes.size(); i++)
		{
			HexNode* hn = &hexNodes[i];

			float aveRain = hn->rainfall;
			float aveTemp = hn->temperature;

			for (int j = 0; j < hn->neighbors.size(); j++)
			{
				aveRain += hexNodes[hn->neighbors[j]].rainfall;
				aveTemp += hexNodes[hn->neighbors[j]].temperature;
			}

			aveRain /= (hn->neighbors.size() + 1);
			aveTemp /= (hn->neighbors.size() + 1);

			rains.push_back(aveRain);
			temps.push_back(aveTemp);
		}

		for (int i = 0; i < hexNodes.size(); i++)
		{
			HexNode* hn = &hexNodes[i];

			hn->rainfall = std::max(std::min(rains[i], 5000.0f), 0.0f);
			hn->temperature = std::max(std::min(temps[i], 30.0f), -25.0f);
		}

		rains.clear();
		temps.clear();

		return hexNodes;
	}

	std::vector<HexNode> GenerateClimate(std::vector<HexNode> hexNodes, glm::vec3 planetPosition, float planetRadius)
	{
		/*
			First, we lay some rough groundwork.
		*/
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

		/*
			Now, we need to figure out rain shadows
			(and where rain is stronger than usual).
		*/

		for (int i = 0; i < hexNodes.size(); i++)
		{
			HexNode* hn = &hexNodes[i];

			if (hn->biome != Biome::mountain) continue;

			/*
				Now, we need to figure out which way the wind
				is a-blowin'. We are going to first add rain
				shadows to our mountains.
			*/
			HexNode* current = hn;

			for (int j = 0; j < Settings::WindStrength; j++)
			{
				// I know. Anyways...
				if (current->biome != Biome::mountain) current->rainfall *= 0.5f;

				// Now, since I can find no easier way,
				// we need to find our next hex in a more
				// complicated fashion than I'd prefer.
				glm::vec3 target = current->center;

				if (current->latitude > Settings::HadleyCutoff && current->latitude < Settings::FerrelCutoff) target = RotateRelative(planetPosition, current->center, Quaternion(0.5f, 0.0f, 1.0f, 0.0f));	// Rotate it west.
				else target = RotateRelative(planetPosition, current->center, Quaternion(0.5f, 0.0f, -1.0f, 0.0f)); // Rotate it east.

				std::vector<HexNode> neighbors;
				for (int k = 0; k < current->neighbors.size(); k++) neighbors.push_back(hexNodes[current->neighbors[k]]);

				current = &hexNodes[FindNearestLateralNeighbor(*current, neighbors, target)];
			}

			/*
				Now, we're going to do a little something.
				We're going to run a line from a mountain over
				some distance and, if it hits water anywhere,
				it will raise the rainfall of all the tiles it
				struck previously.
			*/
			bool hitWater = false;
			std::vector<HexNode*> line;
			for (int j = 0; j < Settings::WindStrength; j++)
			{
				line.push_back(current);
				glm::vec3 target = current->center;

				if (current->latitude > Settings::HadleyCutoff && current->latitude < Settings::FerrelCutoff) target = RotateRelative(planetPosition, current->center, Quaternion(0.5f, 0.0f, -1.0f, 0.0f));	// Rotate it east.
				else target = RotateRelative(planetPosition, current->center, Quaternion(0.5f, 0.0f, 1.0f, 0.0f)); // Rotate it west.

				std::vector<HexNode> neighbors;
				for (int k = 0; k < current->neighbors.size(); k++) neighbors.push_back(hexNodes[current->neighbors[k]]);

				current = &hexNodes[FindNearestLateralNeighbor(*current, neighbors, target)];

				if (current->biome == Biome::ocean)
				{
					hitWater = true;
					break;
				}
			}

			if (hitWater)
			{
				for (int j = 0; j < line.size(); j++)
				{
					line[j]->rainfall *= 1.25f * (1.0f - (j / line.size()));
				}
			}
		}

		/*
			Now, we're going to smooth rainfall and temp
			out and clamp their values.
		*/

		return SmoothClimate(hexNodes);
	}

	std::vector<HexNode> GenerateBiomes(std::vector<HexNode> hexNodes, glm::vec3 planetPosition, float planetRadius)
	{
		srand(time(NULL));

		/*
			First, we're going to add some islands around underwater
			faultlines.
		*/
		for (int i = 0; i < hexNodes.size(); i++)
		{
			HexNode* hn = &hexNodes[i];

			if (!hn->ocean) continue;

			if ((hn->faultNeighbor || hn->fault) && rand() % 100 > Settings::IslandInitialChance)
			{
				hn->island = true;
				hn->ocean = false;
				hn->oceanNeighbor = true;
				hn->biome = Biome::broadleafforest;
				hn->biomeVariation = rand() % 5;
			}
		}

		/*
			Now we're going to iterate over the islands and expand them.
		*/
		for (int iter = 0; iter < Settings::IslandSpreadingIterations; iter++)
		{
			for (int i = 0; i < hexNodes.size(); i++)
			{
				HexNode* hn = &hexNodes[i];

				if (!hn->island) continue;

				bool oceanNeighbor = false;

				for (int j = 0; j < hn->neighbors.size(); j++)
				{
					HexNode* neighbor = &hexNodes[hn->neighbors[j]];

					if (!neighbor->ocean) continue;

					if (rand() % 100 > Settings::IslandSpreadChance)
					{
						neighbor->island = true;
						neighbor->ocean = false;
						neighbor->oceanNeighbor = true;
						neighbor->biome = Biome::broadleafforest;
						neighbor->biomeVariation = rand() % 5;
					}
					else oceanNeighbor = true;
				}

				hn->oceanNeighbor = oceanNeighbor;
			}
		}

		/*
			We should first mark off our oceans and mountains;
			then, we can go through and find all our other biomes.

			This is going to be a multi-stage process.
		*/
		for (int i = 0; i < hexNodes.size(); i++)
		{
			HexNode* hn = &hexNodes[i];

			if (hn->ocean) hn->biome = Biome::ocean;
			else if (hn->island && !hn->fault) hn->biome = Biome::broadleafforest;
			else if (hn->faultNeighbor || (hn->fault && hn->oceanNeighbor)) hn->biome = Biome::highlands;
			else if (hn->fault) hn->biome = Biome::mountain;
		}

		/*
			Quickly, we need to iterate over our islands to
			fix our highlands and mountains.
		*/
		for (int i = 0; i < hexNodes.size(); i++)
		{
			HexNode* hn = &hexNodes[i];

			if (!hn->island) continue;

			for (int j = 0; j < hn->neighbors.size(); j++)
			{
				HexNode* neighbor = &hexNodes[hn->neighbors[j]];

				if (neighbor->biome == Biome::mountain)
				{
					hn->biome = Biome::highlands;
					break;
				}
			}
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
						neighbor->biome == Biome::mountain ||
						neighbor->island) continue;

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

		/*
			Now to determine winds, rains, and average temperatures.
		*/
		hexNodes = GenerateClimate(hexNodes, planetPosition, planetRadius);

		/*
			And now that we've done *all* that, we can figure out our
			biomes. It should be relatively straightforward.
		*/
		for (int i = 0; i < hexNodes.size(); i++)
		{
			HexNode* hn = &hexNodes[i];

			if (hn->biome != Biome::broadleafforest) continue;

			if (hn->rainfall < 250 && hn->temperature < -10)
			{
				hn->biome = Biome::tundra;
			}
			else if (hn->rainfall < 1000 && hn->temperature < -10)
			{
				hn->biome = Biome::taiga;
			}
			else if (hn->rainfall < 1000)
			{
				hn->biome = Biome::desert;
			}
			else if (hn->rainfall < 2000)
			{
				hn->biome = Biome::steppe;
			}
			else if (hn->rainfall < 4500 && hn->temperature > 12.5f)
			{
				hn->biome = Biome::savanna;
			}
			else if (hn->rainfall < 4500 && hn->temperature > 10)
			{
				hn->biome = Biome::dryforest;
			}
			else if (hn->rainfall < 4500 && hn->temperature < 10)
			{
				hn->biome = Biome::broadleafforest;
			}
			else if (hn->rainfall > 4500)
			{
				hn->biome = Biome::rainforest;
			}

			/*
				Now, we need to handle our "special" biomes
				such as mediterranean and oceanic climates
				which depend on ocean currents more.
			*/
			if (!hn->oceanNeighbor ||
				hn->biome == Biome::desert ||
				hn->biome == Biome::tundra ||
				hn->biome == Biome::taiga ||
				hn->biome == Biome::rainforest) continue;

			/*
				We're also going to somewhat arbitrarily
				include a latitude threshold.
			*/
			if (hn->latitude < 0.3f) continue;

			/*
				Time to whip out the good old
				nearest lateral neighbor function.
			*/
			HexNode* current = hn;

			unsigned int westWater = 0;
			unsigned int eastWater = 0;

			for (int j = 0; j < Settings::WaterSearchDistance; j++)
			{
				glm::vec3 target = RotateRelative(planetPosition, current->center, Quaternion(0.5f, 0.0f, 1.0f, 0.0f));

				std::vector<HexNode> neighbors;
				for (int k = 0; k < current->neighbors.size(); k++) neighbors.push_back(hexNodes[current->neighbors[k]]);

				current = &hexNodes[FindNearestLateralNeighbor(*current, neighbors, target)];
				if (current->biome == Biome::ocean) westWater++;
			}

			current = hn;

			for (int j = 0; j < Settings::WaterSearchDistance; j++)
			{
				glm::vec3 target = RotateRelative(planetPosition, current->center, Quaternion(0.5f, 0.0f, -1.0f, 0.0f));

				std::vector<HexNode> neighbors;
				for (int k = 0; k < current->neighbors.size(); k++) neighbors.push_back(hexNodes[current->neighbors[k]]);

				current = &hexNodes[FindNearestLateralNeighbor(*current, neighbors, target)];
				if (current->biome == Biome::ocean) eastWater++;
			}

			if (eastWater >= westWater && hn->latitude < Settings::FerrelCutoff ||
				westWater >= eastWater && hn->latitude > Settings::FerrelCutoff)
			{
				hn->rainfall *= 1.25f;
				hn->biome = Biome::oceanic;
			}
			else
			{
				hn->rainfall *= 0.75f;
				hn->biome = Biome::mediterranean;
			}

			hn->rainfall = std::max(std::min(hn->rainfall, 5000.0f), 0.0f);
		}

		/*
			We're going to spread our coastal climates
			inwards a little.
		*/
		for (int iter = 0; iter < Settings::CoastalBiomeSpreadingIterations; iter++)
		{
			std::vector<unsigned int> newMeds;
			std::vector<unsigned int> newOceanics;

			for (int i = 0; i < hexNodes.size(); i++)
			{
				HexNode* hn = &hexNodes[i];

				if (hn->biome != Biome::mediterranean &&
					hn->biome != Biome::oceanic) continue;

				for (int j = 0; j < hn->neighbors.size(); j++)
				{
					HexNode* neighbor = &hexNodes[hn->neighbors[j]];

					if (neighbor->biome != Biome::steppe &&
						neighbor->biome != Biome::savanna &&
						neighbor->biome != Biome::dryforest &&
						neighbor->biome != Biome::broadleafforest) continue;

					if (hn->biome == Biome::mediterranean)
					{
						neighbor->rainfall *= Lerp(0.75f, 0.5f, iter / Settings::CoastalBiomeSpreadingIterations);
						newMeds.push_back(hn->neighbors[j]);
					}
					else
					{
						neighbor->rainfall *= Lerp(1.25f, 1.0f, iter / Settings::CoastalBiomeSpreadingIterations);
						newOceanics.push_back(hn->neighbors[j]);
					}

					neighbor->rainfall = std::max(std::min(neighbor->rainfall, 5000.0f), 0.0f);
				}
			}

			for (int i = 0; i < newMeds.size(); i++) hexNodes[newMeds[i]].biome = Biome::mediterranean;
			for (int i = 0; i < newOceanics.size(); i++) hexNodes[newOceanics[i]].biome = Biome::oceanic;
		}

		/*
			Now we're gonna roughen up the biomes
			a little so we don't have as many hard lines.
		*/
		for (int iter = 0; iter < Settings::BiomeRoughingIterations; iter++)
		{
			for (int i = 0; i < hexNodes.size(); i++)
			{
				HexNode* hn = &hexNodes[i];

				if (hn->biome == Biome::ocean ||
					hn->biome == Biome::mountain ||
					hn->biome == Biome::highlands) continue;

				int r = rand() % 100 + 1;
				if (r < 80) continue;

				Biome randomBiome = hexNodes[hn->neighbors[rand() % hn->neighbors.size()]].biome;

				if (randomBiome == Biome::ocean ||
					randomBiome == Biome::mountain ||
					randomBiome == Biome::highlands) continue;

				hn->biome = randomBiome;
			}
		}

		return SmoothClimate(hexNodes);
	}
}