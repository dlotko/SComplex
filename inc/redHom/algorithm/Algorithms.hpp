#ifndef SCOMPLEX_ALGS_HPP_
#define SCOMPLEX_ALGS_HPP_


#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/optional.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>


#include <utility>
#include <algorithm>
#include <map>
#include <set>

#include "Coreduction.hpp"
#include "Shave.hpp"
#include "strategy/DefaultReduceStrategy.hpp"
#include "../RedHomCAPD.h"


template<typename SComplexT, typename ReducibleFreeChainComplexT>
class ReducibleFreeChainComplexOverZFromSComplexAlgorithm {
public:
  typedef SComplexT SComplex;
  typedef typename DefaultReduceStrategy<SComplexT>::Cell Cell;

  typedef ReducibleFreeChainComplexT ReducibleFreeChainComplex;

private:
  typedef std::map<typename SComplex::Id, boost::shared_ptr<Cell> > CellsById;

public:
  class SComplexChainCell {
    SComplex* complex;
    const Cell* cell;
    const CellsById* cellsById;

    typename SComplex::Dim embededDim;

  public:
    SComplexChainCell(SComplex* _complex, const Cell* _cell, typename SComplex::Dim _embededDim,  const CellsById* _cellsById): 
      complex(_complex), cell(_cell), embededDim(_embededDim), cellsById(_cellsById) {

	 }

	 int embDim() const {
	   return embededDim;
	 }


	 int embeddingDimension() const {
	   return embededDim;
	 }

	 int ownDim() const {
	   return cell->getDim();
	 }

	 int dimension() const {
	   return cell->getDim();
	 }


    void boundary(std::map<SComplexChainCell,int>& A_boundary)  const {
	   typename SComplex::ColoredIterators::Iterators::BdCells bdCells = complex->iterators(1).bdCells(*this->cell);

	   for (typename SComplex::ColoredIterators::Iterators::BdCells::iterator it = bdCells.begin(),
		  end = bdCells.end(); it != end; ++it) {

	     typename CellsById::const_iterator bdCellIt = cellsById->find(it->getId());
	     BOOST_ASSERT(bdCellIt != cellsById->end());
	     Cell* bdCell = bdCellIt->second.get();
	     BOOST_ASSERT(bdCell != NULL);
	     A_boundary.insert(std::make_pair(SComplexChainCell(complex, bdCell, embededDim, cellsById),
					      complex->coincidenceIndex(*(this->cell), *bdCell) ));
	   }
	 }

	 bool operator<(const SComplexChainCell& b) const {
	   //return this->cell < b.cell;
	   return this->cell->getId() < b.cell->getId();
	 }
  };

public:


  ReducibleFreeChainComplexOverZFromSComplexAlgorithm(SComplex& _s): s(_s) {}
  CRef<ReducibleFreeChainComplex> operator()();

private:
  SComplex& s;
  CellsById cellsById;
};

template<typename SComplexT, typename ReducibleFreeChainComplexT>
inline CRef<ReducibleFreeChainComplexT> ReducibleFreeChainComplexOverZFromSComplexAlgorithm<SComplexT, ReducibleFreeChainComplexT>::operator()(){

  std::vector<SComplexChainCell> cells;
  cellsById.clear();

  typename SComplex::Dim maxDim = s.getDim();

  for (typename SComplex::ColoredIterators::Iterators::AllCells::iterator it = s.iterators(1).allCells().begin(),
			end = s.iterators(1).allCells().end();
		 it != end; ++it) {
    Cell* cell = cellsById[it->getId()].get();
    if (cell == NULL) {
      cell = new Cell(*it);
      cellsById[it->getId()].reset(cell);
    }
    //cells.insert(SComplexChainCell(s, *it, maxDim));
    cells.push_back(SComplexChainCell(&s, cell, maxDim, &cellsById));
  }

  CRef<ReducibleFreeChainComplex> rfccCR( new ReducibleFreeChainComplex(cells));

  return rfccCR;
}


#endif // SCOMPLEX_ALGS_HPP_
