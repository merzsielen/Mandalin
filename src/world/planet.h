#ifndef WORLD_H
#define WORLD_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>

#include "river.h"
#include "../util/checkerror.h"

namespace Mandalin
{
	/*-------------------------------------------------*/
	/* Planet                                          */
	/*-------------------------------------------------*/
	/*
		I figure we might as well try to work in support
		for multiple planets, just in case that becomes
		a thing we want to play with later.
	*/
	class Planet
	{
	private:
		/*-----------------------------------------------*/
		/* Position */
		/*-----------------------------------------------*/
		glm::vec3				position = glm::vec3(0.0f, 0.0f, 0.0f);
		float					radius;
		unsigned int			totalTriangles;

		/*-----------------------------------------------*/
		/* Chunks */
		/*-----------------------------------------------*/
		/*
			Given that we want to be able to (in theory)
			support an arbitrary number of tiles, we need to
			use a vector to hold our chunks. This is less
			than ideal, so we may change this later.
		*/
		std::vector<Chunk>		chunks;

		/*-----------------------------------------------*/
		/* Ocean */
		/*-----------------------------------------------*/
		Ocean*					ocean;

		/*-----------------------------------------------*/
		/* Rivers */
		/*-----------------------------------------------*/
		RiverSystem*			rivers;

		/*-----------------------------------------------*/
		/* World Generation Options */
		/*-----------------------------------------------*/
		unsigned int			worldSize;

	public:
		/*-----------------------------------------------*/
		/* Variable Functions */
		/*-----------------------------------------------*/
		glm::vec3				GetPosition() { return position; }
		float					GetRadius() { return radius; }
		unsigned int			GetWorldSize() { return worldSize; }
		unsigned int			GetTotalTriangles() { return totalTriangles; }

		/*-----------------------------------------------*/
		/* Chunks, Cont. */
		/*-----------------------------------------------*/
		unsigned int			ChunkCount() { return chunks.size(); }
		Chunk*					GetChunk(unsigned int i) { return &chunks[i]; }

		Hex*					GetHex(unsigned int chunk, unsigned int index);

		void					SetPopulation(unsigned int chunk, unsigned int hex, int population);
		void					SetLanguage(unsigned int chunk, unsigned int hex, int language);

		/*-----------------------------------------------*/
		/* Ocean */
		/*-----------------------------------------------*/
		Ocean*					GetOcean() { return ocean; }

		/*-----------------------------------------------*/
		/* World Generation */
		/*-----------------------------------------------*/
		/*
			Here is where we are going to outline the
			functions necessary for basic world generation.
			First, we need to generate the chunks and hexes
			of which the world is composed.

			(Next follows tectonic plates, temperature,
			wind, currents, biomes, etc.)
		*/
		float					GetRise(Biome biome);
		Biome					GetBiome(HexNode* hn);
		std::vector<HexNode>	Hexify(Polyhedron* polyhedron);
		std::vector<HexNode>	SortNeighbors(std::vector<HexNode> hexNodes);
		std::vector<HexNode>	GenerateTopology(std::vector<HexNode> hexNodes);
		void					GenerateGeometry(std::vector<HexNode> hexNodes);

		/*---------------------*/
		/* Constructor         */
		/*---------------------*/
		Planet(unsigned int worldSize);
		~Planet();
	};
}

#endif