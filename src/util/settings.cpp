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

		// DESERTS
		for (int iter = 0; iter < 5; iter++)
		{
			glm::vec4 r = glm::vec4(((rand() % 10) / 100.0f) - 0.2f, ((rand() % 10) / 100.0f) - 0.2f, ((rand() % 10) / 100.0f) - 0.2f, 0.0f);
			BiomeColors[i++] = DesertColor + r;
		}

		// STEPPES
		for (int iter = 0; iter < 5; iter++)
		{
			glm::vec4 r = glm::vec4(((rand() % 10) / 100.0f) - 0.2f, ((rand() % 10) / 100.0f) - 0.2f, ((rand() % 10) / 100.0f) - 0.2f, 0.0f);
			BiomeColors[i++] = SteppeColor + r;
		}

		// SAVANNAS
		for (int iter = 0; iter < 5; iter++)
		{
			glm::vec4 r = glm::vec4(((rand() % 10) / 100.0f) - 0.2f, ((rand() % 10) / 100.0f) - 0.2f, ((rand() % 10) / 100.0f) - 0.2f, 0.0f);
			BiomeColors[i++] = SavannaColor + r;
		}

		// DRY FORESTS
		for (int iter = 0; iter < 5; iter++)
		{
			glm::vec4 r = glm::vec4(((rand() % 10) / 100.0f) - 0.2f, ((rand() % 10) / 100.0f) - 0.2f, ((rand() % 10) / 100.0f) - 0.2f, 0.0f);
			BiomeColors[i++] = DryForestColor + r;
		}

		// BROADLEAF FORESTS
		for (int iter = 0; iter < 5; iter++)
		{
			glm::vec4 r = glm::vec4(((rand() % 10) / 100.0f) - 0.2f, ((rand() % 10) / 100.0f) - 0.2f, ((rand() % 10) / 100.0f) - 0.2f, 0.0f);
			BiomeColors[i++] = BroadleafForestColor + r;
		}

		// RAINFORESTS
		for (int iter = 0; iter < 5; iter++)
		{
			glm::vec4 r = glm::vec4(((rand() % 10) / 100.0f) - 0.2f, ((rand() % 10) / 100.0f) - 0.2f, ((rand() % 10) / 100.0f) - 0.2f, 0.0f);
			BiomeColors[i++] = RainforestColor + r;
		}

		// TUNDRA
		for (int iter = 0; iter < 5; iter++)
		{
			glm::vec4 r = glm::vec4(((rand() % 10) / 100.0f) - 0.2f, ((rand() % 10) / 100.0f) - 0.2f, ((rand() % 10) / 100.0f) - 0.2f, 0.0f);
			BiomeColors[i++] = TundraColor + r;
		}

		// TAIGA
		for (int iter = 0; iter < 5; iter++)
		{
			glm::vec4 r = glm::vec4(((rand() % 10) / 100.0f) - 0.2f, ((rand() % 10) / 100.0f) - 0.2f, ((rand() % 10) / 100.0f) - 0.2f, 0.0f);
			BiomeColors[i++] = TaigaColor + r;
		}

		//  MEDITERRANEAN
		for (int iter = 0; iter < 5; iter++)
		{
			glm::vec4 r = glm::vec4(((rand() % 10) / 100.0f) - 0.2f, ((rand() % 10) / 100.0f) - 0.2f, ((rand() % 10) / 100.0f) - 0.2f, 0.0f);
			BiomeColors[i++] = MediterraneanColor + r;
		}

		//  OCEANIC
		for (int iter = 0; iter < 5; iter++)
		{
			glm::vec4 r = glm::vec4(((rand() % 10) / 100.0f) - 0.2f, ((rand() % 10) / 100.0f) - 0.2f, ((rand() % 10) / 100.0f) - 0.2f, 0.0f);
			BiomeColors[i++] = OceanicColor + r;
		}
	}
}