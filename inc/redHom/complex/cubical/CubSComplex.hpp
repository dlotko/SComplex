#ifndef CUB_SCOMPLEX_HPP
#define CUB_SCOMPLEX_HPP

#include <boost/assert.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <utility>
#include "../../RedHomCAPD.h"




template<int DIM>
class CubSComplex {
  class CellImpl;

  typedef BitSetT<BitmapT<unsigned long int> > BitSet;
  typedef EuclBitSetT<BitSet, DIM> EuclBitSet;
  
public:

  typedef CubSetT<EuclBitSet> BCubSet;
  typedef CubCellSetT<EuclBitSet> BCubCellSet;

  class BitCoordCellImpl;
  class DynamicBitCoordCellImpl;
  class BitCoordPtrCellImpl;

  template<typename, typename Enable=void>
  class CellProxy;

  typedef CellProxy<BitCoordCellImpl> Cell;
  typedef CellProxy<BitCoordPtrCellImpl> CellRef;
  typedef CellProxy<DynamicBitCoordCellImpl> DynamicCell;
  
  typedef int Dim;
  typedef int Color;
  //typedef std::pair<const BCubCellSet::BitCoordIterator::WordIterator, int> Id;
  typedef long Id;
  
private:
  
  template<typename NumeratorT, bool isConst>
  class IteratorProvider;

  
  class BasicNumerator;
  class CellNumerator;
  class CellDimNumerator;
  class CbdNumerator;
  class BdNumerator;

  template<Color color>
  struct FixedColorChecker {
    bool operator()(bool b) const {
      return (color == 1 && b) || (color == 2 && !b);
    }
  };

  struct ColorChecker {
    Color color;

    ColorChecker(Color _color): color(_color) {}

    bool operator()(bool b) const {
      return (color == 1 && b) || (color == 2 && !b);
    }
  };

  struct NoBitChecker {
    bool operator()(bool b) const {
      return true;
    }
  };

  template<bool isConst, typename BitChecker>
  class IteratorsImpl;

  template<bool isConst, typename BitChecker>
  friend class IteratorsImpl;

  template<bool isConst>
  class ColoredIteratorsImpl {

  public:
    typedef IteratorsImpl<isConst, ColorChecker> Iterators;

    template<typename CubSComplex::Color color>
    class Color {

    public:
      typedef IteratorsImpl<isConst, FixedColorChecker<color> > Iterators;
    };
  };

public:

  typedef IteratorsImpl<false, NoBitChecker> Iterators;
  typedef IteratorsImpl<true, NoBitChecker> ConstIterators;


  typedef ColoredIteratorsImpl<false> ColoredIterators;
  typedef ColoredIteratorsImpl<true> ColoredConstIterators;
  
  explicit CubSComplex(RepSet<ElementaryCube>& repSet);
  explicit CubSComplex(const CRef<BCubCellSet>& __bCubCellSet);

  size_t cardinality() {  return bCubCellSet.cardinality(); }
  size_t size() const { return const_cast<BCubCellSet&>(bCubCellSet).getBmpSizeInBits(); }

  Dim getDim() { return DIM; }

   ConstIterators iterators() const;
   Iterators iterators();

  typename ColoredConstIterators::Iterators iterators(const Color& color) const;
  typename ColoredIterators::Iterators iterators(const Color& color);

  template<Color color>
  typename ColoredIterators::template Color<color>::Iterators iterators();

  template<Color color>
  typename ColoredConstIterators::template Color<color>::Iterators iterators() const;

  template<typename ImplT>
  bool getUniqueFace(const CellProxy<ImplT>& cell, DynamicCell& coface) const;

  template<typename ImplT>
  bool getUniqueCoFace(const CellProxy<ImplT>& cell, DynamicCell& coface) const;

  template<typename ImplT1, typename ImplT2>
  int coincidenceIndex(const CellProxy<ImplT1> &a, const CellProxy<ImplT2> &b) const;
  
protected:
  //  boost::shared_ptr<RepSet<ElementaryCube> > repSet;
  CRef<BCubCellSet> _bCubCellSet;
  BCubCellSet& bCubCellSet;
  
};

#include "CubSComplex_Cell.hpp"
#include "CubSComplex_IteratorProvider.hpp"
#include "CubSComplex_Iterators.hpp"
#include "CubSComplex_ColoredIterators.hpp"
#include "CubSComplex_Numerators.hpp"


template<int DIM>
inline CubSComplex<DIM>::CubSComplex(RepSet<ElementaryCube>& _repSet): 
  _bCubCellSet(new BCubCellSet(_repSet)),
  bCubCellSet(_bCubCellSet()) {

  bCubCellSet.addEmptyCollar();
}

template<int DIM>
inline CubSComplex<DIM>::CubSComplex(const CRef<BCubCellSet>& __bCubCellSet): 
  _bCubCellSet(__bCubCellSet),
  bCubCellSet(_bCubCellSet()) {

  bCubCellSet.addEmptyCollar();
}

template<int DIM>
inline typename CubSComplex<DIM>::Iterators CubSComplex<DIM>::iterators() {
  return Iterators(*this);
}

template<int DIM>
inline typename CubSComplex<DIM>::ConstIterators CubSComplex<DIM>::iterators() const {
  return ConstIterators(*this);
}

template<int DIM>
inline typename CubSComplex<DIM>::ColoredConstIterators::Iterators CubSComplex<DIM>::iterators(const Color& color) const {
  BOOST_ASSERT(color == 1);
  return typename ColoredConstIterators::Iterators(*this, ColorChecker(color));
}

template<int DIM>
inline typename CubSComplex<DIM>::ColoredIterators::Iterators CubSComplex<DIM>::iterators(const Color& color) {
  BOOST_ASSERT(color == 1);
  return typename ColoredIterators::Iterators(*this, ColorChecker(color));
}

template<int DIM>
template<typename CubSComplex<DIM>::Color color>
inline typename CubSComplex<DIM>::ColoredIterators::template Color<color>::Iterators CubSComplex<DIM>::iterators() {
  return typename ColoredIterators::template Color<color>::Iterators(*this);
}

template<int DIM>
template<typename CubSComplex<DIM>::Color color>
inline typename CubSComplex<DIM>::ColoredConstIterators::template Color<color>::Iterators CubSComplex<DIM>::iterators() const {
  return typename ColoredConstIterators::template Color<color>::Iterators(*this);
}

template<int DIM>
template<typename ImplT>
inline bool CubSComplex<DIM>::getUniqueCoFace(const CellProxy<ImplT>& cell, DynamicCell& coface) const {
  if (bCubCellSet.isFreeFace(const_cast<typename CubSComplex::BCubCellSet::BitCoordIterator&>(cell.getBitCoordIt()), coface.getBitCoordIt())) {
  	 return true;
  } else {
  	 return false;
  }
}

template<int DIM>
template<typename ImplT>
inline bool CubSComplex<DIM>::getUniqueFace(const CellProxy<ImplT>& cell, DynamicCell& coface) const {
  if (bCubCellSet.isFreeCoFace(const_cast<typename CubSComplex::BCubCellSet::BitCoordIterator&>(cell.getBitCoordIt()), coface.getBitCoordIt())) {
  	 return true;
  } else {
  	 return false;
  }
}

template<int DIM>
template<typename ImplT1, typename ImplT2>
inline int CubSComplex<DIM>::coincidenceIndex(const CellProxy<ImplT1> &_a, const CellProxy<ImplT2> &_b) const {
  int res = 0;
  int sgn = 1;

  const typename CubSComplex::BCubCellSet::BitCoordIterator& a = _a.getBitCoordIt();
  const typename CubSComplex::BCubCellSet::BitCoordIterator& b = _b.getBitCoordIt();

  for (Dim i = 0, end = DIM; i < end; ++i) {
  	 if (! (a[i]/2 == b[i]/2 || a[i]/2 + (a[i]%2) == b[i]/2)) {
  		return 0; // b[i] left side doesn't intersect a[i] interval
  	 }
	 
  	 if (a[i] % 2 == 1 && b[i] % 2 == 1) {
  		sgn = -sgn; //both nondegenerated, go to next
  	 } else if (a[i] % 2 == 0 && b[i] % 2 == 0) {
  		// both degenerated
  	 } else if (b[i] % 2 == 1) {		
  		return 0; // a[i] is inside b[i]
  	 } else { // b[i] is inside a[i]
  		if (res != 0) {
  		  return 0; // second time, so not proper face
  		}
  		res = sgn;
  		if (a[i] / 2 != b[i]/2) { // b[i] is the right face of a[i]
  		  res = -res;
  		}
  	 }
  }
  return res;
}


#endif

