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

		// This pair is designated as the number of women and men (separated out) in a hex.
		unsigned int									domain;
		std::unordered_map<Hex*, std::pair<int, int>>	subpopulations;
	};
}

#endif