#include "language.h"

namespace Mandalin
{
	float Levenshtein(std::string a, std::string b)
	{
		if (a.size() > b.size()) return Levenshtein(b, a);

		unsigned int minSize = a.size();
		unsigned int maxSize = b.size();

		std::vector<unsigned int> levDist(minSize + 1);
		
		for (int i = 0; i <= minSize; i++) levDist[i] = i;
		
		for (int i = 0; i <= maxSize; i++)
		{
			unsigned int prevDiag = levDist[0], prevDiagSave;
			++levDist[0];

			for (unsigned int j = 0; j <= minSize; j++)
			{
				prevDiagSave = levDist[j];

				if (a[j - 1] == b[i - 1]) levDist[j] = prevDiag;
				else levDist[i] = std::min(std::min(levDist[j - 1], levDist[j]), prevDiag) + 1;

				prevDiag = prevDiagSave;
			}
		}

		return levDist[minSize];
	}
}