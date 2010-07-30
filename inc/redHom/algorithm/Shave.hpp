#ifndef _SCOMPLEX_ALGS_SHAVE_HPP
#define _SCOMPLEX_ALGS_SHAVE_HPP

#include "strategy/DefaultReduceStrategy.hpp"
#include "AKQStrategy.hpp"
#include "strategy/DefaultReduceStrategy_CubSComplex.hpp"

template<typename StrategyT>
class ShaveAlgorithm {

public:
  typedef StrategyT Strategy;
  typedef typename Strategy::SComplex SComplex;
  typedef typename SComplex::Cell Cell;

  ShaveAlgorithm(Strategy* _strategy): strategy(_strategy) {}

  ~ShaveAlgorithm() {
	 delete strategy;
  }

  size_t operator()();

private:
  Strategy* strategy;
};

class ShaveAlgorithmFactory {

public:
  template<typename SComplex>
  static ShaveAlgorithm<DefaultReduceStrategy<SComplex> > createDefault(SComplex& s) {
	 return ShaveAlgorithm<DefaultReduceStrategy<SComplex> >(new DefaultReduceStrategy<SComplex>(s));
  }
};

template<typename StrategyT>
inline size_t ShaveAlgorithm<StrategyT>::operator()(){

  size_t reduced = 0;

  for(typename SComplex::Dim d = strategy->getComplex().getDim(); d >= 0; --d){
	 typedef typename SComplex::ColoredIterators::Iterators::DimCells::iterator DimIt;

	 typename SComplex::ColoredIterators::Iterators::DimCells dimCells = strategy->getComplex().iterators(1).dimCells(d);
	 for (DimIt it = dimCells.begin(),
	 		  end = dimCells.end();
	 		it != end; ) {
		typename StrategyT::Traits::template GetReductionPair<typename DimIt::value_type>::result_type reductionPair =
		  strategy->getReductionPair(*it);
		if (reductionPair) {
		  strategy->reduce(*reductionPair);
		  Cell v = *it; 
		  ++it;
		  strategy->reduce(v);
		  reduced += 2;
		} else {
		  ++it;
		}
	 }
  }

  return reduced;
}

#endif
