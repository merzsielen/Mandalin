#ifndef RIVER_H
#define RIVER_H

#include "biomes.h"

namespace Mandalin
{
	struct RiverNode
	{
		/*
			Hex Indices
		*/
		unsigned int	a;
		unsigned int	b;
		unsigned int	c;
	};

	struct River
	{
		unsigned int				id;
		std::vector<RiverNode>		nodes;
	};

	class RiverSystem
	{
	private:
		std::vector<River>		rivers;

		GLuint					vao;
		GLuint					vbo;

	public:
		GLuint					GetVAO() { return vao; }
		GLuint					GetVBO() { return vbo; }

		unsigned int			RiverCount() { return rivers.size(); }
		River*					GetRiver(unsigned int i) { return &rivers[i]; }

		std::vector<HexNode>	GenerateRivers(std::vector<HexNode> hexNodes);
	};

}

#endif