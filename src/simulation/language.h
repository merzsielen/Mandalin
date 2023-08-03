#ifndef LANGUAGE_H
#define LANGUAGE_H

#include <string>
#include <vector>

namespace Mandalin
{
	/*-----------------------------------------------*/
	/* Utilities                                     */
	/*-----------------------------------------------*/
	float Levenshtein(std::string a, std::string b);

	/*-----------------------------------------------*/
	/* Hex-Based Sublanguage                         */
	/*-----------------------------------------------*/
	struct Sublanguage
	{
		std::string		state;

		unsigned int	chunk;
		unsigned int	tile;

		float			perc;
	};

	/*-----------------------------------------------*/
	/* Language                                      */
	/*-----------------------------------------------*/
	struct Language
	{
		std::string											state;
		std::string											origin;

		std::vector<Sublanguage>							subs;
	};
}

#endif