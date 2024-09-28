#ifndef POPULATION_H
#define POPULATION_H

#include "../world/hex.h"
#include "language.h"
#include <utility>

namespace Mandalin
{
	struct Population
	{
		unsigned int									id;

		std::vector<unsigned int>						languages;

		unsigned int									domain;
		// This double is the percentage of a hex that a population makes up.
		std::unordered_map<Hex*, double>				subpopulations;
	};
}

#endif