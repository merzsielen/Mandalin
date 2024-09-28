#include "history.h"
#include <algorithm>
#include <random>

namespace Mandalin
{
	/*-------------------------------------------------*/
	/* Populations                                     */
	/*-------------------------------------------------*/
	void History::PopulationSplit(Population* population, Hex* origin)
	{
		double ratio = ((rand() % 35 + 1) + 15.0) / 100.0;

		unsigned int nHex = floor(population->domain * ratio);

		if (nHex == 0) return;

		int popID = rand() % 10000 + 1;

		Population newPopulation =
		{
			popID,
			{ 0 },
			nHex,
			{ }
		};

		population->domain -= nHex;
		populations.push_back(newPopulation);

		// Now, we're going to flood-fill a number of hexes from the origin
		// and add these to our new population, removing them from the old one.

		unsigned int number = 1;
		origin->checked = 1;
		std::vector<Hex*> searchArea = { origin };

		while (true)
		{
			std::vector<Hex*> newSearchArea;

			for (int i = 0; i < searchArea.size(); i++)
			{
				Hex* hex = searchArea[i];

				for (int j = 0; j < hex->neighbors.size(); j++)
				{
					Hex* n = planet->GetHex(hex->neighbors[j].first, hex->neighbors[j].second);

					if (n->checked != -1) continue;

					if (population->subpopulations.find(n) != population->subpopulations.end() && number < nHex)
					{
						number++;
						n->checked = 1;
						newSearchArea.push_back(n);
					}
				}
			}

			for (int i = 0; i < newSearchArea.size(); i++) searchArea.push_back(newSearchArea[i]);

			if (number >= nHex || newSearchArea.size() == 0) break;
		}

		for (int i = 0; i < searchArea.size(); i++)
		{
			Hex* hex = searchArea[i];

			newPopulation.subpopulations[hex] = population->subpopulations[hex];
			population->subpopulations.erase(hex);

			CheckPopulation(hex);

			hex->checked = -1;
		}

		populations.push_back(newPopulation);
	}

	void History::MoveSubpopulation(Hex* origin, Hex* destination, unsigned int women, unsigned int men)
	{
		// First, let's grab save some variables for later use.
		unsigned int originOldPop = origin->Population();
		unsigned int destinationOldPop = origin->Population();

		// Now, we're just gonna plainly transfer the people.
		destination->population.first += women;
		destination->population.second += men;

		if ((int)origin->population.first - (int)women < 0) origin->population.first = 0;
		else origin->population.first -= women;

		if ((int)origin->population.second - (int)men < 0) origin->population.second = 0;
		else origin->population.second -= men;

		// Now, we need to go through each population and
		// update the percentage of each population that belongs
		// to them.
		double migrantPerc = (women + men) / destination->Population();
		double dilution = migrantPerc / destination->subpopCount;
		double concentration = migrantPerc / origin->subpopCount;

		unsigned int blockedPops = 0;

		for (int i = 0; i < populations.size(); i++)
		{
			Population* p = &populations[i];

			if (p->subpopulations.find(destination) != p->subpopulations.end())
			{
				// If this population is a pre-existing one at the destination,
				// we need to see how much its percentage of the population is
				// diluted by the arrival of these new people.

				p->subpopulations[destination] -= dilution;

				if (p->subpopulations[destination] <= 0)
				{
					p->domain--;
					destination->subpopCount--;
					p->subpopulations.erase(destination);
				}
			}
			
			if (p->subpopulations.find(origin) != p->subpopulations.end())
			{
				// If this population is departing the origin, we need to see
				// how much of a percentage they gain in the new location.

				// If the population is too small, just don't migrate it
				// and add it instead to the blocked pops.
				if (p->subpopulations[origin] < 0.1)
				{
					blockedPops++;
					continue;
				}

				if (p->subpopulations.find(destination) != p->subpopulations.end())
				{
					p->subpopulations[destination] += concentration;
				}
				else if (p->domain < Settings::DomainLimit - 1)
				{
					p->domain++;
					destination->subpopCount++;
					p->subpopulations[destination] = concentration;
				}
				else // if (p->domain == Settings::DomainLimit)
				{
					p->domain++;
					destination->subpopCount++;
					p->subpopulations[destination] = concentration;
					PopulationSplit(p, origin);
				}
				//else
				//{
				//	// Here, we're dealing with a population that has reached
				//	// its domain limit which causes some zaney weirdness. We
				//	// have to go back and adjust the other populations to 
				//	// account for the fact that we can't add this pop to these
				//	// others.

				//	blockedPops++;

				//	// We need to check if ethnogenesis occurs which sometimes
				//	// happens when a population spreads itself too thin.
				//}
			}
		}

		if (blockedPops > 0)
		{
			for (int i = 0; i < populations.size(); i++)
			{
				Population* p = &populations[i];

				if (p->subpopulations.find(destination) != p->subpopulations.end())
				{
					p->subpopulations[destination] += (concentration / (double)blockedPops);
				}
			}

		}

		CheckPopulation(origin);
		CheckPopulation(destination);
	}

	// Returns true if successful and false if a failure.
	void History::ProximalMigration(Hex* origin, unsigned int nWomen, unsigned int nMen)
	{
		/*
			A proximal migration, as opposed to a medial or distal migration,
			searches until it finds a valid target. If this migration cannot purely
			occur over land and / or across a short distance, then a medial migration
			occurs.

			First, we are going to flood-fill from the origin tile till we have
			an area of acceptable size or we can no longer add new tiles.
		*/

		unsigned int number = nWomen + nMen;

		origin->checked = (origin->lcc - origin->Population());
		std::vector<Hex*> searchArea;

		int bestImmediateScore = 0;

		for (int i = 0; i < origin->neighbors.size(); i++)
		{
			Hex* n = planet->GetHex(origin->neighbors[i].first, origin->neighbors[i].second);

			if (n->checked == -1 && n->biome != Biome::ocean)
			{
				n->checked = (n->lcc - n->Population());
				searchArea.push_back(n);
				if (n->checked > bestImmediateScore) bestImmediateScore = n->checked;
			}
		}

		/*
			Now, we flood-fill the area around the origin's neighbors
			till we run out of tiles to add or reach the search limit.
		*/

		if (bestImmediateScore < number * 2)
		{
			for (int i = 0; i < Settings::ProximalMigrationSearchDistance; i++)
			{
				std::vector<Hex*> newSearchAreas;

				for (int j = 0; j < searchArea.size(); j++)
				{
					Hex* h = searchArea[j];

					for (int k = 0; k < h->neighbors.size(); k++)
					{
						Hex* n = planet->GetHex(h->neighbors[k].first, h->neighbors[k].second);

						if (n->checked == -1 && n->biome != Biome::ocean)
						{
							n->checked = (n->lcc - n->Population()) - (500 * (i + 1));
							newSearchAreas.push_back(n);
						}
					}
				}

				for (int j = 0; j < newSearchAreas.size(); j++) searchArea.push_back(newSearchAreas[j]);
			}
		}

		/*
			Now that we have our area, we pick the best destination.
		*/

		Hex* destination = nullptr;
		int bestScore = origin->checked;

		for (int i = 0; i < searchArea.size(); i++)
		{
			if (searchArea[i]->checked > bestScore)
			{
				bestScore = searchArea[i]->checked;
				destination = searchArea[i];
			}
		}

		origin->checked = -1;
		for (int i = 0; i < searchArea.size(); i++)
		{
			searchArea[i]->checked = -1;
		}

		if (destination == nullptr)
		{
			// std::cout << "FAILED MIGRATION" << std::endl;
			return;
		}

		double r = 0.5 + (((rand() % 20 + 1) / 100.0) - 0.1);

		int women = ceil(number * r);
		int men = number - women;

		// std::cout << "Moving " << women << " women and " << men << " men from (" << origin->chunk << " / " << origin->index << ") to (" << destination->chunk << ") / (" << destination->index << ")." << std::endl;
		if (women + men > 0) MoveSubpopulation(origin, destination, women, men);
	}

	void History::GrowPopulation(Hex* hex)
	{
		unsigned int oldPop = hex->Population();

		int dWomen = 0;
		int dMen = 0;

		if (hex->population.first != 0 && hex->population.second != 0)
		{ 
			double top = pow(((hex->population.first / (double)hex->population.second) - 1.0), 2);
			double roiMod = exp(-top / 0.125);

			int dPop = (Settings::BaseRateOfNaturalIncrease * roiMod) * oldPop * (1.0 - ((float)oldPop / hex->lcc));

			double ratio = 0.5 + ((rand() % 10 + 1) / 100.0);

			dWomen = floor(dPop * ratio);
			dMen = dPop - dWomen;
		}
		else
		{
			int dPop = Settings::BaseRateOfNaturalIncrease * oldPop * (1.0 - ((float)oldPop / hex->lcc));

			if (hex->population.first != 0 && dWomen < 0)
			{
				dWomen = dPop;
			}
			else if (hex->population.second != 0 && dMen < 0)
			{
				dMen = dPop;
			}
		}

		if ((int)hex->population.first + dWomen <= 0) hex->population.first = 0;
		else hex->population.first += dWomen;

		if ((int)hex->population.second + dMen <= 0) hex->population.second = 0;
		else hex->population.second += dMen;

		if (hex->Population() <= 0)
		{
			for (int i = 0; i < populations.size(); i++)
			{
				Population* p = &populations[i];

				if (p->subpopulations.find(hex) != p->subpopulations.end())
				{
					p->domain--;
					p->subpopulations.erase(hex);
				}
			}

			hex->subpopCount = 0;
			CheckPopulation(hex);
		}
	}

	void History::CheckPopulation(Hex* hex)
	{
		// If this hex is not populated, we made a mistake by calling this.
		if (hex->Population() == 0) planet->SetPopulation(hex->chunk, hex->index, 0);

		// Otherwise, we've gotta see who is the biggest.
		double largestSubpopulation = 0;
		unsigned int largestSubpopID = 0;

		for (int i = 0; i < populations.size(); i++)
		{
			Population* p = &populations[i];

			if (p->subpopulations.find(hex) != p->subpopulations.end())
			{
				double subPerc = p->subpopulations[hex];

				if (subPerc > largestSubpopulation)
				{
					largestSubpopulation = subPerc;
					largestSubpopID = p->id;
				}
			}
		}

		planet->SetPopulation(hex->chunk, hex->index, largestSubpopID);
	}

	/*-------------------------------------------------*/
	/* Update                                          */
	/*-------------------------------------------------*/
	void History::OverflowPopulation(Hex* hex)
	{
		// We're going to trigger a migration.
		unsigned int n = ceil(hex->Population() - (hex->lcc * 0.85));
		double ratio = 0.5 + ((rand() % 10 + 1) / 100.0);

		unsigned int nWomen = floor(n * ratio);
		unsigned int nMen = n - nWomen;

		// For now, this only involves proximal migrations,
		// but at some point I'd like to add medial and distal
		// migrations here as well.
		ProximalMigration(hex, nWomen, nMen);
	}

	void History::UpdatePopulation()
	{
		srand(time(NULL));

		/*
			First, we're going to go through and update the population
			of each inhabited hex.
		*/
		unsigned int cn = planet->ChunkCount();

		for (int i = 0; i < cn; i++)
		{
			Chunk* c = planet->GetChunk(i);
			unsigned int hn = c->hexCount;

			for (int j = 0; j < hn; j++)
			{
				Hex* h = &c->hexes[j];

				if (h->population.first != 0 || h->population.second != 0)
				{
					GrowPopulation(h);

					// Here, we're gonna see if any migrations are
					// triggered due to populations reaching land carrying
					// capacity limits.

					// double ran = rand() % 100 + 1;
					double r = (rand() % 15 + 1) / 100.0;

					if ((h->Population() / (double)h->lcc >= 1.0 - r)) // && ran > 75)
					{
						OverflowPopulation(h);
					}
				}
			}
		}
	}

	void History::Update()
	{
		// Where does the time go?
		day++;
		if (day > Settings::DaysPerMonth)
		{
			day = 1;
			month++;

			// Every month, we update populations.
		}
		if (month > Settings::MonthsPerYear)
		{
			month = 1;
			year++;
		}

		// For now, we're updating every day.
		UpdatePopulation();
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

		unsigned int attempts = 0;
		unsigned int attemptLimit = 1000;

		for (int z = 0; z < Settings::NumberOfStartingPopulations; z++)
		{
			int c = rand() % planet->ChunkCount();
			Chunk* chunk = planet->GetChunk(c);

			int h;
			Hex* hex;

			while (true)
			{
				h = rand() % chunk->hexCount;
				hex = &chunk->hexes[h];

				if (hex->biome != Biome::ocean) break;

				if (attempts > attemptLimit) break;

				attempts++;
			}

			attempts = 0;
			if (hex->biome == Biome::ocean) continue;

			int popID = rand() % 10000 + 1;

			Population pop =
			{
				popID,
				{ 0 },
				1,
				{ }
			};

			pop.subpopulations[hex] = 1.0;
			hex->population.first += 50;
			hex->population.second += 50;
			hex->subpopCount++;

			populations.push_back(pop);
			CheckPopulation(hex);
		}
	}
}