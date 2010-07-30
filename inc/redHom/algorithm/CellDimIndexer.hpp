#ifndef CELLDIMINDEXER_HPP_INCLUDED
#define CELLDIMINDEXER_HPP_INCLUDED

#include <vector>

// No filtering for now, so it requires manuas skipping of cells of
// color != 1
template<typename CellT>
class CellDimIndexer
{
	std::vector<std::vector<CellT> > cellsByDim;
	std::vector<CellT> sortedCells;

	public:
	typedef typename std::vector<CellT>::const_iterator iterator;

	CellDimIndexer()
	{
	}

	template<typename iterT>
	CellDimIndexer(iterT begin, iterT end, int maxDim)
	{
		Stopwatch total;
    	std::cout << "Starting CellDimIndexer ctor\n";

		cellsByDim.resize(maxDim + 1, std::vector<CellT>());

		for (; begin != end; ++begin)
		{
			cellsByDim[begin->getDim()].push_back(*begin);
		}

		for (size_t d = 0; d < cellsByDim.size(); d++)
		{
			const int n = cellsByDim[d].size();
			for (size_t i = 0; i < n; i++)
			{
				sortedCells.push_back(cellsByDim[d][i]);
			}
		}

		std::cout << "CellDimIndexer ctor done in " << total << std::endl;
	}

	iterator begin()
	{
		return sortedCells.begin();
	}

	iterator end()
	{
		return sortedCells.end();
	}
};

#endif
