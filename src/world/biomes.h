#ifndef BIOMES_H
#define BIOMES_H

#include "ocean.h"

namespace Mandalin
{
	std::vector<HexNode> GenerateClimate(std::vector<HexNode> hexNodes, glm::vec3 planetPosition, float planetRadius);
	std::vector<HexNode> GenerateBiomes(std::vector<HexNode> hexNodes, glm::vec3 planetPosition, float planetRadius);
}

#endif