#ifndef CUB_SCOMPLEX_CELL_HPP
#define CUB_SCOMPLEX_CELL_HPP

#include "CubSComplex.hpp"
#include "../BasicCellProxy.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <boost/type_traits.hpp>

template<int DIM>
template<typename CellImplT, typename Enable>
class CubSComplex<DIM>::CellProxy: public Enable {
};

template<int DIM>
template<typename CellImplT>
class CubSComplex<DIM>::CellProxy<CellImplT,
				  typename boost::enable_if<typename boost::is_base_of<typename CubSComplex<DIM>::CellImpl,
										       typename boost::remove_pointer<CellImplT>::type >::type
							    >::type > : public BasicCellProxy<CellImplT> {
protected:
  using BasicCellProxy<CellImplT>::impl;

public:

  CellProxy(): BasicCellProxy<CellImplT>() {}
  template<typename CubCellImplT2>
  CellProxy(const CubCellImplT2& _impl): BasicCellProxy<CellImplT>(_impl) {}

  template<typename CubCellImplT2>
  CellProxy(const CellProxy<CubCellImplT2>& other): BasicCellProxy<CellImplT>(*other.getImpl()) {}

  const typename CubSComplex<DIM>::BCubCellSet::BitCoordIterator& getBitCoordIt() const {
	 return CellProxy<CellImplT>::getImpl()->getBitCoordIt();
  }

  typename CubSComplex<DIM>::BCubCellSet::BitCoordIterator& getBitCoordIt() {
  	 return CellProxy<CellImplT>::getImpl()->getBitCoordIt();
  }

};

template<int DIM>
class CubSComplex<DIM>::CellImpl {
public:
  typedef typename CubSComplex::Dim Dim;
  typedef typename CubSComplex::Color Color;
  typedef typename CubSComplex::Id Id;
};

template<int DIM>
class CubSComplex<DIM>::BitCoordCellImpl: public CellImpl {
protected:
  typename CubSComplex<DIM>::BCubCellSet::BitCoordIterator bitIt;
private:
  //Dim dim;
public:

  BitCoordCellImpl(const typename CubSComplex<DIM>::BCubCellSet::BitCoordIterator& b): bitIt(b)/*, dim(b.ownDim())*/ {}

  BitCoordCellImpl(const CubSComplex& s): bitIt(s.bCubCellSet)/*, dim(std::numeric_limits<Dim>::max())*/ {}

  Color getColor() const{
	 return bitIt.getBit() ? 1 : 2;
  }

  template<Color color>
  void setColor() {
	 setColor(color);
  }

  void setColor(const Color& color) {
	 BOOST_ASSERT(color == 2);
	 // BOOST_ASSERT(getColor() == 1);
	 bitIt.clearBit();
  }

  Dim getDim() const {
	 //return dim;
	 return bitIt.ownDim();
  }

  Id getId() const {
	 //return Id(bitIt.wIt, bitIt.bit);
	 return bitIt.position();
  }

  const typename CubSComplex<DIM>::BCubCellSet::BitCoordIterator& getBitCoordIt() const {
	 return bitIt;
  }

  typename CubSComplex<DIM>::BCubCellSet::BitCoordIterator& getBitCoordIt() {
	 return bitIt;
  }

  bool operator<(const BitCoordCellImpl& b) const {
	 return bitIt < b.bitIt;
  }

};

template<int DIM>
class CubSComplex<DIM>::BitCoordPtrCellImpl: public CellImpl {
protected:
  mutable typename CubSComplex<DIM>::BCubCellSet::BitCoordIterator* bitIt;

public:
  BitCoordPtrCellImpl(typename CubSComplex<DIM>::BCubCellSet::BitCoordIterator* b): bitIt(b) {}

  explicit BitCoordPtrCellImpl(const CubSComplex& s): bitIt(NULL) {}

  template<typename ImplT>
  BitCoordPtrCellImpl(CellProxy<ImplT>& b): bitIt(& b.getBitCoordIt()) {}

  operator const typename CubSComplex<DIM>::BCubCellSet::BitCoordIterator& () const {
	 return *bitIt;
  }

  Color getColor() const{
	 return bitIt->getBit() ? 1 : 2;
  }

  template<Color color>
  void setColor() {
	 setColor(color);
  }

  void setColor(const Color& color) {
	 BOOST_ASSERT(color == 2);
	 BOOST_ASSERT(getColor() == 1);
	 bitIt->clearBit();
  }

  Dim getDim() const {
	 return bitIt->ownDim();
  }

  Id getId() const {
	 //return Id(bitIt->wIt, bitIt->bit);
	 return bitIt->position();
  }

  const typename CubSComplex<DIM>::BCubCellSet::BitCoordIterator& getBitCoordIt() const {
	 return *bitIt;
  }

  typename CubSComplex<DIM>::BCubCellSet::BitCoordIterator& getBitCoordIt() {
	 return *bitIt;
  }

  bool operator<(const BitCoordPtrCellImpl& b) const {
	 return *bitIt < *b.bitIt;
  }

};

template<int DIM>
class CubSComplex<DIM>::DynamicBitCoordCellImpl: public BitCoordCellImpl  {

  using BitCoordCellImpl::bitIt;

public:
 DynamicBitCoordCellImpl(const BitCoordCellImpl& b): BitCoordCellImpl(b) {}
 DynamicBitCoordCellImpl(const typename CubSComplex<DIM>::BCubCellSet::BitCoordIterator& b): BitCoordCellImpl(b) {}

 DynamicBitCoordCellImpl(const CubSComplex& s): BitCoordCellImpl(s.bCubCellSet) {}

  Dim getDim() const {
	 return bitIt.ownDim();
  }

  typename CubSComplex<DIM>::BCubCellSet::BitCoordIterator& getBitCoordIt() {
	 return bitIt;
  }

};

#endif
