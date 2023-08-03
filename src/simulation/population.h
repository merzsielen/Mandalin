#ifndef POPULATION_H
#define POPULATION_H

#include "language.h"

namespace Mandalin
{
	struct Subpopulation
	{
		unsigned int	parent;

		unsigned int	chunk;
		unsigned int	tile;

		float			perc;
	};

	struct Population
	{
		unsigned int					id;

		std::vector<unsigned int>		languages;
		std::vector<Subpopulation>		subpopulations;
	};
}

#endif