#include "settings.h"

#include <time.h>

#include "../world/hex.h"

namespace Mandalin
{
	std::unordered_map<unsigned int, glm::vec4> Settings::BiomeColorMap;
	std::unordered_map<unsigned int, glm::vec4> Settings::ChunkColorMap;
	std::unordered_map<unsigned int, glm::vec4> Settings::RegionColorMap;
	std::unordered_map<unsigned int, glm::vec4> Settings::ContinentColorMap;
	std::unordered_map<unsigned int, glm::vec4> Settings::TectonicPlateColorMap;

	void Settings::Setup()
	{
		srand(time(NULL));

		int i = 0;
		
		// OCEAN
		BiomeColorMap.insert(std::pair<unsigned int, glm::vec4>(i++, OceanDepthColor));

		// MOUNTAIN
		for (int iter = 0; iter < 5; iter++)
		{
			glm::vec4 r = glm::vec4(((rand() % 10) / 100.0f) - 0.2f, ((rand() % 10) / 100.0f) - 0.2f, ((rand() % 10) / 100.0f) - 0.2f, 0.0f);
			BiomeColorMap.insert(std::pair<unsigned int, glm::vec4>(i++, MountainColor + r));
		}

		// HIGHLANDS
		for (int iter = 0; iter < 5; iter++)
		{
			glm::vec4 r = glm::vec4(((rand() % 10) / 100.0f) - 0.2f, ((rand() % 10) / 100.0f) - 0.2f, ((rand() % 10) / 100.0f) - 0.2f, 0.0f);
			BiomeColorMap.insert(std::pair<unsigned int, glm::vec4>(i++, HighlandColor + r));
		}

		// FLATLANDS
		for (int iter = 0; iter < 5; iter++)
		{
			glm::vec4 r = glm::vec4(((rand() % 10) / 100.0f) - 0.2f, ((rand() % 10) / 100.0f) - 0.2f, ((rand() % 10) / 100.0f) - 0.2f, 0.0f);
			BiomeColorMap.insert(std::pair<unsigned int, glm::vec4>(i++, LandColor + r));
		}
	}
}