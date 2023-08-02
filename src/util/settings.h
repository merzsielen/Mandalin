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