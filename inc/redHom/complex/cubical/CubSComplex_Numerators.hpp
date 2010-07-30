#ifndef CUB_SCOMPLEX_NUMERATORS_HPP
#define CUB_SCOMPLEX_NUMERATORS_HPP

#include "CubSComplex.hpp"
#include <limits>
#include <boost/shared_ptr.hpp>

// template<int DIM>
// class CubSComplex<DIM>::BasicNumerator {
// protected:
//   explicit BasicNumerator(const CubSComplex& s): bitCoordIt(s.bCubCellSet) {}
  
// public:
//   void toEnd() {
// 	 this->bitCoordIt.wIt=const_cast<typename CubSComplex::BCubCellSet::BitCoordIterator::WordIterator>(this->bitCoordIt.getBitmap().end().wIt);
//   }
		
//   CellProxy<BitCoordPtrCellImpl> Current(){
// 	 return CellProxy<BitCoordPtrCellImpl>(BitCoordPtrCellImpl(&bitCoordIt));
//   }

//   bool operator==(const BasicNumerator& o) const {
// 	 return bitCoordIt.wIt == o.bitCoordIt.wIt;
//   }

//   bool operator!=(const BasicNumerator& o) const {
// 	 return this->bitCoordIt.wIt != o.bitCoordIt.wIt;
//   }

// protected:
//   typename CubSComplex::BCubCellSet::BitCoordIterator bitCoordIt;
  
// };

// template<int DIM>
// class CubSComplex<DIM>::CellNumerator: public BasicNumerator{
//   using BasicNumerator::bitCoordIt;
  
// public:
// 	 typedef Cell value_type;
  
//   CellNumerator(const CubSComplex& s): BasicNumerator(s) {
//   }
  
// 	 bool MoveNext(){
// 		++(bitCoordIt);
// 		bitCoordIt.findPoint();
// 		return bitCoordIt.wIt < bitCoordIt.getBitmap().end().wIt;
// 	 }
// };


// template<int DIM>
// class CubSComplex<DIM>::BdNumerator: public BasicNumerator{
//   using BasicNumerator::bitCoordIt;  
// public:
//   typedef Cell value_type;

//   using BasicNumerator::toEnd;

//   template<typename ImplT>
//   BdNumerator(const CubSComplex& s, const CellProxy<ImplT>& c): BasicNumerator(s), center(c.getBitCoordIt()),i(0),downDir(true), dim(s.bCubCellSet.embDim()) {
// 	 }
		
//   bool MoveNext(){
// 	 while(i < dim){
// 		// process only directions in which cell is degenerate
// 		if(!downDir || center.odd(i)){
// 		  ((typename CubSComplex::BCubCellSet::BitCoordIterator&)(bitCoordIt)) = center;
// 		  // First check the bottom face
// 		  if(downDir){
// 			 bitCoordIt.decInDir(i);
// 			 downDir=false;
// 			 // and now go to the top face
// 		  }else{
// 			 bitCoordIt.incInDir(i);;
// 			 downDir=true;
// 			 ++i;
// 		  }
// 		  return true;
// 		}else{
// 		  ++i;
// 		}
// 	 }
// 	 //cCell=false;
// 	 toEnd();
// 	 return false;
//   }

// protected:
//   typename CubSComplex::BCubCellSet::BitCoordIterator center;
//   int i;
//   bool downDir;
//   const int dim;
// };

#endif // CUB_SCOMPLEX_NUMERATORS_HPP
