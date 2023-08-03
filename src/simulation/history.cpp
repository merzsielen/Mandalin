#include "history.h"

namespace Mandalin
{
	/*-------------------------------------------------*/
	/* Utilities                                       */
	/*-------------------------------------------------*/
	void History::IncrementTime()
	{
		day++;
		if (day > Settings::DaysPerMonth)
		{
			day = 1;
			month++;
		}
		if (month > Settings::MonthsPerYear)
		{
			month = 1;
			year++;
		}
	}

	void History::PopulateHex(Population* pop, Hex* hex, unsigned int people)
	{
		/*
			We may consider a more efficient way of doing this later.
			But for now this will have to suffice.

			First, we check to see if any other population
			already has a subpopulation on this hex.
		*/

		//std::vector<Subpopulation*>	existingSubpops;

		//for (int i = 0; i < populations.size(); i++)
		//{
		//	for (int j = 0; j < populations[i].subpopulations.size(); j++)
		//	{
		//		Subpopulation* subpop = &populations[i].subpopulations[j];

		//		if (subpop->chunk == hex->chunk && subpop->tile == hex->index)
		//		{
		//			existingSubpops.push_back(subpop);
		//		}
		//	}
		//}

		//Population* currentMajority = 

		//float percMod = hex->population / (float)(hex->population + people);
		//for (int i = 0; i < existingSubpops.size(); i++)
		//{
		//	existingSubpops[i]->perc *= percMod;
		//}

		//float perc = people / (float)(hex->population + people);
		//hex->population += people;

		//Population* currentMaj = &populations[hex->populationID];

		//// if (currentMaj->)

		//neighbor->population = hex->population;
		//planet->SetPopulation(neighbor->chunk, neighbor->index, hex->population);
		//newSubpops.push_back({ 0, neighbor->chunk, neighbor->index, 1.0f });
	}

	/*-------------------------------------------------*/
	/* Update                                          */
	/*-------------------------------------------------*/
	void History::Update()
	{
		// Where does the time go?
		IncrementTime();

		/*
			TEST
		*/
		for (int i = 0; i < populations.size(); i++)
		{
			Population* pop = &populations[i];

			std::vector<Subpopulation> newSubpops;

			for (int j = 0; j < pop->subpopulations.size(); j++)
			{
				Subpopulation* subpop = &pop->subpopulations[j];

				Hex* hex = planet->GetHex(subpop->chunk, subpop->tile);

				for (int k = 0; k < hex->neighbors.size(); k++)
				{
					Hex* neighbor = planet->GetHex(hex->neighbors[k].first, hex->neighbors[k].second);

					if (neighbor->population != hex->population && rand() % 100 + 1 > 75)
					{
						neighbor->population = hex->population;
						planet->SetPopulation(neighbor->chunk, neighbor->index, hex->population);
						newSubpops.push_back({ 0, neighbor->chunk, neighbor->index, 1.0f });
					}
				}
			}

			for (int j = 0; j < newSubpops.size(); j++) pop->subpopulations.push_back(newSubpops[j]);
		}
		/*
			END TEST
		*/
	}

	/*-------------------------------------------------*/
	/* Constructor                                     */
	/*-------------------------------------------------*/
	History::History(Planet* planet)
	{
		srand(time(NULL));

		this->planet = planet;

		this->day = 1;
		this->month = 1;
		this->year = 1;

		int c = rand() % planet->ChunkCount();
		Chunk* chunk = planet->GetChunk(c);

		int h;
		Hex* hex;

		while (true)
		{
			h = rand() % chunk->hexCount;
			hex = &chunk->hexes[h];

			if (hex->biome != Biome::ocean) break;
		}

		Subpopulation subpop =
		{
			c, h, 1.0f
		};

		Population pop =
		{
			rand() % 10000 + 1,
			{ 0 },
			{ subpop }
		};

		hex->population = pop.id;
		planet->SetPopulation(c, h, pop.id);

		populations.push_back(pop);
	}
}