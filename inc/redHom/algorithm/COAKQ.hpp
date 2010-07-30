#ifndef _SCOMPLEX_ALGS_COAKQ_HPP
#define _SCOMPLEX_ALGS_COAKQ_HPP

#include "strategy/COAKQStrategy.hpp"
#include <deque>
#include <set>
#include <vector>
#include <boost/pool/object_pool.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <boost/shared_ptr.hpp>


template<typename StrategyT>
class COAKQAlgorithm {

public:

  typedef StrategyT Strategy;

  typedef typename Strategy::SComplex SComplex;
  typedef typename Strategy::Cell Cell;

  COAKQAlgorithm(Strategy* _strategy): strategy(_strategy) {}


  inline bool reduceNextPair() {

    while (! cellsToProcess.empty() ) {
      Cell* cell = &cellsToProcess.front();

      if (! strategy->reduced(*cell)) {
	typename StrategyT::Traits::template GetReductionPair<Cell>::result_type reductionPair = strategy->getReductionPair(*cell);

	if (reductionPair) {
	  doReduction(*cell, *reductionPair);
	  cellIdsToProcess[cell->getId()] = false;
	  cellsToProcess.pop_front();
	  return true;
	} else {
	  addCellsToProcess(*cell);
	}
      }
      cellIdsToProcess[cell->getId()] = false;
      cellsToProcess.pop_front();
    }

    typename StrategyT::Traits::ForceReduction::result_type force = strategy->forceReductionPair();
    if (force) {
      doReduction(force->first, force->second);
      return true;
    }
    return false;
  }


  int operator()(){
    cellIdsToProcess.resize(strategy->getComplex().size());

    int cnt=0;

    for(;;){
      if (reduceNextPair()) {
	++cnt;++cnt;
      } else {
	typename StrategyT::Traits::Extract::result_type sourceFace = strategy->extract();

	if(sourceFace){
	  addCellsToProcess(*sourceFace);

	  strategy->reduce(*sourceFace);
	  ++cnt;
	}else{
	  break; // no base face left: quit any further processing
	}
      }
    }

    return cnt; // the number of cells removed
  }


  Strategy& getStrategy() {
    return *strategy;
  }

private:


  template<typename ImplT1, typename ImplT2>
  void doReduction(const typename Strategy::SComplex::template CellProxy<ImplT1>& a, const typename Strategy::SComplex::template CellProxy<ImplT2>& b) {
    //storeReductionPair(a, b);
	 strategy->reduce(a, b);
	 addCellsToProcess(b);
  }

  template<typename ImplT>
  void addCellsToProcess(const typename Strategy::SComplex::template CellProxy<ImplT>& sourceFace) {
    typename SComplex::ColoredIterators::Iterators::BdCells bdCells = strategy->getComplex().template iterators<1>().bdCells(sourceFace);
    for (typename SComplex::ColoredIterators::Iterators::BdCells::iterator bdn = bdCells.begin(),
	   end = bdCells.end(); bdn != end; ++bdn) {
      typename SComplex::ColoredIterators::Iterators::BdCells::iterator::value_type v = *bdn;
      if (!cellIdsToProcess[v.getId()]) {
	cellsToProcess.push_back(v);
	cellIdsToProcess[v.getId()] = true;
      }
    }
  }

  Strategy* strategy;
  std::deque<Cell> cellsToProcess;
  std::vector<bool> cellIdsToProcess;

};


#endif
