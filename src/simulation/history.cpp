#include "history.h"
#include <algorithm>
#include <random>

namespace Mandalin
{
	/*-------------------------------------------------*/
	/* Populations                                     */
	/*-------------------------------------------------*/
	void History::MoveSubpopulation(Population* population, Hex* origin, Hex* destination, unsigned int women, unsigned int men)
	{
		if (population->subpopulations.find(destination) != population->subpopulations.end())
		{
			population->subpopulations[destination].first += women;
			population->subpopulations[destination].second += men;
		}
		else
		{
			population->domain++;
			destination->subpopCount++;
			population->subpopulations[destination] = std::make_pair(women, men);
		}

		population->subpopulations[origin].first -= women;
		population->subpopulations[origin].second -= men;

		if (population->subpopulations[origin].first <= 0) population->subpopulations[origin].first = 0;
		if (population->subpopulations[origin].second <= 0) population->subpopulations[origin].second = 0;

		if (population->subpopulations[origin].first == 0 && population->subpopulations[origin].second == 0)
		{
			population->domain--;
			population->subpopulations.erase(origin);
			origin->subpopCount--;
		}

		if ((int)origin->population - (int)(women + men) < 0) origin->population = 0;
		else origin->population -= women + men;

		destination->population += women + men;

		CheckPopulation(origin);
		CheckPopulation(destination);
	}

	// Returns true if successful and false if a failure.
	void History::ProximalMigration(Population* population, Hex* origin, unsigned int number)
	{
		/*
			A proximal migration, as opposed to a medial or distal migration,
			searches until it finds a valid target. If this migration cannot purely
			occur over land and / or across a short distance, then a medial migration
			occurs.

			First, we are going to flood-fill from the origin tile till we have
			an area of acceptable size or we can no longer add new tiles.
		*/

		origin->checked = (origin->lcc - origin->population);
		std::vector<Hex*> searchArea;

		int bestImmediateScore = 0;

		for (int i = 0; i < origin->neighbors.size(); i++)
		{
			Hex* n = planet->GetHex(origin->neighbors[i].first, origin->neighbors[i].second);

			if (n->checked == -1 && n->biome != Biome::ocean)
			{
				n->checked = (n->lcc - n->population);
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
							n->checked = (n->lcc - n->population) - (500 * (i + 1));
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
		MoveSubpopulation(population, origin, destination, women, men);
	}

	void History::GrowPopulation(Hex* hex)
	{
		unsigned int newPopulation = 0;

		for (int i = 0; i < populations.size(); i++)
		{
			Population* p = &populations[i];

			if (p->subpopulations.find(hex) != p->subpopulations.end())
			{
				std::pair<int, int>* woMen = &p->subpopulations[hex];

				int pop = woMen->first + woMen->second;

				// I'm adding this here just to be safe.
				if (pop == 0)
				{
					p->subpopulations.erase(hex);
					hex->subpopCount--;
					continue;
				}

				double perc = pop / (double)hex->population;

				double roiMod = 1.0;

				if (woMen->first == 0 || woMen->second == 0) roiMod = 0.0f;
				else
				{
					// The Rate of Increase modifier falls on a Gaussian
					// Distribution where the peak is at a 50 / 50 male-female
					// ratio. Forgive me for being so binary here.

					// In this GD, a = 1; b = 1; c = 0.25

					double top = pow(((woMen->first / (double)woMen->second) - 1.0), 2);

					roiMod = exp(-top / 0.125);
				}

				if (roiMod >= 1.0) roiMod = 1.0;
				else if (roiMod <= 0.001) roiMod = 0.001;

				int dPop = (Settings::BaseRateOfNaturalIncrease * roiMod) * pop * (1.0 - (pop / (perc * hex->lcc)));

				double ratio = 0.5 + ((rand() % 10 + 1) / 100.0);

				int dWomen = floor(dPop * ratio);
				int dMen = dPop - dWomen;

				woMen->first += dWomen;
				woMen->second += dMen;

				if (woMen->first < 0) woMen->first = 0;
				if (woMen->second < 0) woMen->second = 0;

				pop = woMen->first + woMen->second;

				if (pop == 0)
				{
					p->subpopulations.erase(hex);
					p->domain--;
					hex->subpopCount--;
				}

				newPopulation += pop;
			}
		}

		// std::cout << "Hex population grew from " << (int)hex->population << " to " << newPopulation << "." << std::endl;
		hex->population = newPopulation;
		CheckPopulation(hex);
	}

	void History::CheckPopulation(Hex* hex)
	{
		// If this hex is not populated, we made a mistake by calling this.
		if (hex->population == 0) planet->SetPopulation(hex->chunk, hex->index, 0);

		// Otherwise, we've gotta see who is the biggest.
		unsigned int largestSubpopulation = 0;
		unsigned int largestSubpopID = 0;

		for (int i = 0; i < populations.size(); i++)
		{
			Population* p = &populations[i];

			if (p->subpopulations.find(hex) != p->subpopulations.end())
			{
				std::pair<int, int>* woMen = &p->subpopulations[hex];

				unsigned int pop = woMen->first + woMen->second;

				if (pop > largestSubpopulation)
				{
					largestSubpopulation = pop;
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
		unsigned int n = ceil(hex->population - (hex->lcc * 0.85));

		// The question is, how do we decide who
		// migrates out? Ideally, this would be some
		// fancy calculation, but for now we'll just
		// grab from random subpopulations until we
		// get enough people.

		// The number of subpopulations that will migrate.
		unsigned int msn = rand() % hex->subpopCount + 1;

		while (true)
		{
			if (msn == 0) break;
			else if (ceil(n / (double)msn) <= 0) msn--;
			else break;
		}

		if (msn <= 0) return;

		n = ceil(n / (double)msn);

		// I don't love this, but oh well.
		std::random_device rd;
		std::mt19937 g(rd());
		std::vector<int> rawValues;
		for (int k = 0; k < hex->subpopCount; k++) rawValues.push_back(k);
		std::shuffle(rawValues.begin(), rawValues.end(), g);
		std::vector<int> values;
		for (int k = 0; k < msn; k++) values.push_back(rawValues[k]);
		unsigned int counted = 0;

		for (int l = 0; l < populations.size(); l++)
		{
			Population* p = &populations[l];

			if (p->subpopulations.find(hex) != p->subpopulations.end())
			{
				bool inValues = false;
				for (int m = 0; m < values.size(); m++)
				{
					if (values[m] == counted)
					{
						inValues = true;
						break;
					}
				}

				if (!inValues) continue;
				if (p->domain >= Settings::DomainLimit) continue;

				// std::cout << "Triggering a migration of " << n << " people from a population of " << hex->population << std::endl;
				ProximalMigration(p, hex, n);
			}
		}
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

				if (h->population != 0)
				{
					GrowPopulation(h);

					// Here, we're gonna see if any migrations are
					// triggered due to populations reaching land carrying
					// capacity limits.

					// double ran = rand() % 100 + 1;
					double r = (rand() % 15 + 1) / 100.0;

					if ((h->population / (double)h->lcc >= 1.0 - r)) // && ran > 75)
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

			pop.subpopulations[hex] = std::make_pair(50, 50);
			hex->population += 100;
			hex->subpopCount++;

			populations.push_back(pop);
			CheckPopulation(hex);
		}
	}
}