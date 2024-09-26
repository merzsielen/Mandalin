#ifndef HISTORY_H
#define HISTORY_H

#include <unordered_map>

#include "population.h"
#include "../world/planet.h"

namespace Mandalin
{
	class History
	{
	private:
		/*----------------------------------------------------------------------*/
		/* Chronology                                                           */
		/*----------------------------------------------------------------------*/
		unsigned int											day;
		unsigned int											month;
		unsigned int											year;

		/*----------------------------------------------------------------------*/
		/* Languages                                                            */
		/*----------------------------------------------------------------------*/
		std::vector<Language>									languages;

		/*----------------------------------------------------------------------*/
		/* Populations                                                          */
		/*----------------------------------------------------------------------*/
		std::vector<Population>									populations;

		void													MoveSubpopulation(Population* population, Hex* origin, Hex* destination, unsigned int women, unsigned int men);
		
		void													ProximalMigration(Population* population, Hex* hex, unsigned int number);
		// void													MedialMigration(Population* population, Hex* hex, unsigned int number);
		// void													DistalMigration(Population* population, Hex* hex, unsigned int number);

		void													OverflowPopulation(Hex* hex);
		void													GrowPopulation(Hex* hex);
		void													CheckPopulation(Hex* hex);

		/*----------------------------------------------------------------------*/
		/* Planet                                                               */
		/*----------------------------------------------------------------------*/
		Planet*													planet;

	public:
		/*----------------------------------------------------------------------*/
		/* Update                                                               */
		/*----------------------------------------------------------------------*/
		void													UpdatePopulation();
		void													Update();

		/*----------------------------------------------------------------------*/
		/* Constructor                                                          */
		/*----------------------------------------------------------------------*/
		History(Planet* planet);
	};
}

#endif