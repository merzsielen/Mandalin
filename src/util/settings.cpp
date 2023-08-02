#include "settings.h"

#include <time.h>

#include "../world/hex.h"

namespace Mandalin
{
	glm::vec4 Settings::BiomeColors[64];
	glm::vec4 Settings::TectonicPlateColors[64];

	void Settings::Setup()
	{
		srand(time(NULL));

		int i = 0;
		
		// OCEAN
		BiomeColors[i++] = OceanDepthColor;

		// MOUNTAIN
		for (int iter = 0; iter < 5; iter++)
		{
			glm::vec4 r = glm::vec4(((rand() % 10) / 100.0f) - 0.2f, ((rand() % 10) / 100.0f) - 0.2f, ((rand() % 10) / 100.0f) - 0.2f, 0.0f);
			BiomeColors[i++] = MountainColor + r;
		}

		// HIGHLANDS
		for (int iter = 0; iter < 5; iter++)
		{
			glm::vec4 r = glm::vec4(((rand() % 10) / 100.0f) - 0.2f, ((rand() % 10) / 100.0f) - 0.2f, ((rand() % 10) / 100.0f) - 0.2f, 0.0f);
			BiomeColors[i++] = HighlandColor + r;
		}

		// FLATLANDS
		for (int iter = 0; iter < 5; iter++)
		{
			glm::vec4 r = glm::vec4(((rand() % 10) / 100.0f) - 0.2f, ((rand() % 10) / 100.0f) - 0.2f, ((rand() % 10) / 100.0f) - 0.2f, 0.0f);
			BiomeColors[i++] = LandColor + r;
		}
	}
}