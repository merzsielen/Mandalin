#ifndef SETTINGS_H
#define SETTINGS_H

#include <unordered_map>

#include <glm/glm.hpp>

namespace Mandalin
{
	struct Settings
	{
		/*------------------------------------------------------------------------------------------------------*/
		/*  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  */
		/* Simulation                                                                                           */
		/*  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  */
		/*------------------------------------------------------------------------------------------------------*/
		/*-------------------------------------------------*/
		/* Inputs                                          */
		/*-------------------------------------------------*/
		static constexpr float			TicRate = 0.5f;

		/*-------------------------------------------------*/
		/* Chronology                                      */
		/*-------------------------------------------------*/
		static constexpr unsigned int	DaysPerMonth = 30;
		static constexpr unsigned int	MonthsPerYear = 12;

		/*------------------------------------------------------------------------------------------------------*/
		/*  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  */
		/* World Generation                                                                                     */
		/*  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  */
		/*------------------------------------------------------------------------------------------------------*/
		/*-------------------------------------------------*/
		/* Planet                                          */
		/*-------------------------------------------------*/
		static constexpr unsigned int	WorldSize = 6;
		static constexpr unsigned int	RegionCount = 100 * WorldSize;
		static constexpr unsigned int	ContinentCount = 20 * WorldSize;
		static constexpr unsigned int	TectonicPlateCount = 2 * WorldSize;

		static constexpr unsigned int	CoastRoughingIterations = 30;
		static constexpr unsigned int	MountainSpreadingIterations = WorldSize - 3;
		static constexpr unsigned int	CoastalBiomeSpreadingIterations = WorldSize - 3;
		static constexpr unsigned int	BiomeRoughingIterations = 5;

		static constexpr float			HadleyCutoff = 0.3f;
		static constexpr float			FerrelCutoff = 0.6f;

		static constexpr unsigned int	WindStrength = 10;
		static constexpr unsigned int	WaterSearchDistance = 10;

		static constexpr unsigned int	RiverCount = WorldSize * 10;

		/*-------------------------------------------------*/
		/* Ocean                                           */
		/*-------------------------------------------------*/
		static constexpr float			OceanOffset = 0.99f;

		/*-------------------------------------------------*/
		/* Chunks                                          */
		/*-------------------------------------------------*/
		static constexpr unsigned int	ChunkMaxHexes = 300;
		static constexpr unsigned int	ChunkTrisPerHex = 18;
		static constexpr unsigned int	ChunkMaxTris = (ChunkMaxHexes * ChunkTrisPerHex);

		static constexpr unsigned int	OceanChunkMaxTris = 10000;

		/*------------------------------------------------------------------------------------------------------*/
		/*  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  */
		/* Rendering                                                                                            */
		/*  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  --  */
		/*------------------------------------------------------------------------------------------------------*/
		/*-------------------------------------------------*/
		/* Camera                                          */
		/*-------------------------------------------------*/
		static constexpr float			FOV = 180.0f;
		static constexpr float			NearClip = 0.01f;
		static constexpr float			FarClip = 100.0f;

		static constexpr float			CameraMovementSpeed = 100.0f;
		static constexpr float			MinCameraDistance = 100.0f;
		static constexpr float			MaxCameraDistance = 155.0f;

		/*-------------------------------------------------*/
		/* Colors                                          */
		/*-------------------------------------------------*/
		static constexpr glm::vec4		BackgroundColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

		static constexpr glm::vec4		OceanColor = glm::vec4(0.13f, 0.55f, 0.69f, 0.5f);
		static constexpr glm::vec4		OceanDepthColor = glm::vec4(0.0f, 0.0f, 0.25f, 1.0f);
		static constexpr glm::vec4		LandColor = glm::vec4(0.05f, 0.73f, 0.28f, 1.0f);
		static constexpr glm::vec4		HighlandColor = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
		static constexpr glm::vec4		MountainColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		static constexpr glm::vec4		DesertColor = glm::vec4(0.9f, 0.88f, 0.64f, 1.0f);
		static constexpr glm::vec4		SteppeColor = glm::vec4(0.82f, 0.69f, 0.35f, 1.0f);
		static constexpr glm::vec4		SavannaColor = glm::vec4(0.6f, 0.87f, 0.17f, 1.0f);
		static constexpr glm::vec4		DryForestColor = glm::vec4(0.41f, 0.65f, 0.04f, 1.0f);
		static constexpr glm::vec4		BroadleafForestColor = glm::vec4(0.05f, 0.73f, 0.28f, 1.0f);
		static constexpr glm::vec4		RainforestColor = glm::vec4(0.18f, 0.7f, 0.44f, 1.0f);
		static constexpr glm::vec4		TundraColor = glm::vec4(0.77f, 0.91f, 0.91f, 1.0f);
		static constexpr glm::vec4		TaigaColor = glm::vec4(0.09f, 0.57f, 0.57f, 1.0f);
		static constexpr glm::vec4		MediterraneanColor = glm::vec4(1.0f, 0.85f, 0.0f, 1.0f);
		static constexpr glm::vec4		OceanicColor = glm::vec4(0.0f, 1.0f, 0.7f, 1.0f);

		/*static constexpr glm::vec4	DesertColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		static constexpr glm::vec4		SteppeColor = glm::vec4(1.0f, 0.71f, 0.0f, 1.0f);
		static constexpr glm::vec4		SavannaColor = glm::vec4(0.87f, 1.0f, 0.0f, 1.0f);
		static constexpr glm::vec4		DryForestColor = glm::vec4(0.15f, 0.3f, 0.15f, 1.0f);
		static constexpr glm::vec4		BroadleafForestColor = glm::vec4(0.5f, 1.0f, 0.5f, 1.0f);
		static constexpr glm::vec4		RainforestColor = glm::vec4(0.98f, 0.0f, 1.0f, 1.0f);
		static constexpr glm::vec4		TundraColor = glm::vec4(0.77f, 0.91f, 0.91f, 1.0f);
		static constexpr glm::vec4		TaigaColor = glm::vec4(0.09f, 0.57f, 0.57f, 1.0f);
		static constexpr glm::vec4		MediterraneanColor = glm::vec4(0.75f, 0.96f, 0.39f, 1.0f);
		static constexpr glm::vec4		OceanicColor = glm::vec4(0.0f, 1.0f, 0.7f, 1.0f);*/

		/*-------------------------------------------------*/
		/* Maps                                            */
		/*-------------------------------------------------*/
		static glm::vec4										BiomeColors[64];
		static glm::vec4										TectonicPlateColors[64];

		/*-------------------------------------------------*/
		/* Setup                                           */
		/*-------------------------------------------------*/
		static void Setup();
	};
}

#endif