#ifndef _SCOMPLEX_ALGS_COREDUCTION_HPP
#define _SCOMPLEX_ALGS_COREDUCTION_HPP

#include "strategy/DefaultReduceStrategy.hpp"
#include "AKQStrategy.hpp"
//#include "CancellationStrategy.hpp"

#include <deque>
#include <set>
#include <boost/pool/object_pool.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <boost/shared_ptr.hpp>



template<typename StrategyT>
class CoreductionAlgorithm {

public:
  typedef StrategyT Strategy;

  typedef typename Strategy::SComplex SComplex;
  typedef typename Strategy::Cell Cell;

  CoreductionAlgorithm(Strategy* _strategy): strategy(_strategy) {}

  ~CoreductionAlgorithm() {
	 if (strategy)
		delete strategy;
  }

  size_t operator()();

  Strategy* getStrategy() { return strategy; }

private:

  template<typename ImplT>
  void storeGenerator(const typename Strategy::SComplex::template CellProxy<ImplT>& a) {
	 collectedHomGenerators.push_back(a);
  }

  template<typename ImplT1, typename ImplT2>
  void storeCoreductionPair(const typename Strategy::SComplex::template CellProxy<ImplT1>& a, const typename Strategy::SComplex::template CellProxy<ImplT2>& b) {}

  bool coreduceNextPair();

  template<typename ImplT>
  void addCellsToProcess(const typename Strategy::SComplex::template CellProxy<ImplT>& sourceFace) {
	 // Finally, put all present cofaces of the source face
	 // into the queue
	 typename SComplex::ColoredIterators::Iterators::CbdCells cbdCells = strategy->getComplex().iterators(1).cbdCells(sourceFace);
	 for (typename SComplex::ColoredIterators::Iterators::CbdCells::iterator cbdn = cbdCells.begin(),
			  end = cbdCells.end(); cbdn != end; ++cbdn) {
		typename SComplex::ColoredIterators::Iterators::CbdCells::iterator::value_type v = *cbdn;

// std::cerr << "got " << v.getId() << " size: " << cellIdsToProcess.size() << std::endl;
		if (!cellIdsToProcess[v.getId()]) {
		  cellsToProcess.push_back(v);
		  cellIdsToProcess[v.getId()] = true;
		}
	 }
  }


  template<typename ImplT1, typename ImplT2>
  void doCoreduction(const typename Strategy::SComplex::template CellProxy<ImplT1>& a, const typename Strategy::SComplex::template CellProxy<ImplT2>& b) {
	 storeCoreductionPair(a, b);
	 strategy->coreduce(a, b);
	 addCellsToProcess(a);
  }

  Strategy* strategy;

  std::vector<Cell> collectedHomGenerators;
  std::deque<Cell> cellsToProcess;
  std::vector<bool> cellIdsToProcess;
};

class CoreductionAlgorithmFactory {

public:
  template<typename SComplex>
  static boost::shared_ptr< CoreductionAlgorithm<DefaultReduceStrategy<SComplex> > > createDefault(SComplex& s) {
	 return boost::shared_ptr< CoreductionAlgorithm<DefaultReduceStrategy<SComplex> > >(new CoreductionAlgorithm<DefaultReduceStrategy<SComplex> >(new DefaultReduceStrategy<SComplex>(s)));
  }


//   template<typename SComplex>
//   static boost::shared_ptr< CoreductionAlgorithm<AKQCancellationReduceStrategy<SComplex> > > createAKQCancellation(SComplex& s) {
// 	 return boost::shared_ptr< CoreductionAlgorithm<AKQCancellationReduceStrategy<SComplex> > >(new CoreductionAlgorithm<AKQCancellationReduceStrategy<SComplex> >(new AKQCancellationReduceStrategy<SComplex>(s)));
//   }


  template<typename SComplex>
  static boost::shared_ptr< CoreductionAlgorithm<AKQReduceStrategy<SComplex> > > createAKQ(SComplex& s) {
	 return boost::shared_ptr< CoreductionAlgorithm<AKQReduceStrategy<SComplex> > >(new CoreductionAlgorithm<AKQReduceStrategy<SComplex> >(new AKQReduceStrategy<SComplex>(s)));
  }

};

template<typename StrategyT>
inline bool CoreductionAlgorithm<StrategyT>::coreduceNextPair() {

  while (! cellsToProcess.empty() ) {
	 Cell* cell = &cellsToProcess.front();

	 if (! strategy->reduced(*cell)) {
		typename StrategyT::Traits::template GetCoreductionPair<Cell>::result_type coreductionPair = strategy->getCoreductionPair(*cell);

		if (coreductionPair) {
		  doCoreduction(*coreductionPair, *cell);
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

  // Originally there are no candidates in the queue
  // They may also disappear when a connected component
  // is exhausted
  // If we know that a coreduction may be there,
  // For instance when treating a non-compact set
  typename StrategyT::Traits::ForceCoreduction::result_type force = strategy->forceCoreductionPair();
  if (force) {
	 doCoreduction(force->first, force->second);
	 return true;
  }
  return false;
}


template<typename StrategyT>
inline size_t CoreductionAlgorithm<StrategyT>::operator()(){
  cellIdsToProcess.resize(strategy->getComplex().maxId());

  size_t reduced=0;

  for(;;){
	 if (coreduceNextPair()) {
	   reduced += 2;
	 } else {
		// If the search failed or when we even did not try to search
		// and we know that a cell of lowest dimension is always
		// a homology generator like in the case of a vertex in
		// a compact set, we just pick up such a cell and
		// remove it from the complex
		typename StrategyT::Traits::Extract::result_type sourceFace = strategy->extract();

		if(sourceFace){
		  storeGenerator(*sourceFace);
		  addCellsToProcess(*sourceFace);

		  strategy->reduce(*sourceFace);
		  ++reduced;
		}else{
		  break; // no base face left: quit any further processing
		}
	 }
  }

  return reduced; // the number of cells removed
}

#endif
