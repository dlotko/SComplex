#ifndef _SCOMPLEX_HPP
#define _SCOMPLEX_HPP

#include <algorithm>
#include <vector>
#include <map>
#include <list>
#include <functional>

#include <boost/ref.hpp>
#include <boost/mpl/if.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/construct.hpp>
#include <boost/iterator/indirect_iterator.hpp>
#include <boost/function.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/utility.hpp>

#include "redHom/util/Iterators.hpp"
#include "../BasicCellProxy.hpp"

template<typename TraitsT>
class SComplex: boost::noncopyable {
public:
  typedef TraitsT Traits;

  typedef typename Traits::Color Color;
  typedef typename Traits::Dim Dim;
  typedef typename Traits::Id Id;
  typedef typename Traits::Size Size;

private:
  class CellImpl;
  typedef typename Traits::template ColoredObjectsModel<CellImpl* > Cells;
  typedef std::vector<typename Traits::template ColoredObjectsModel<boost::reference_wrapper<CellImpl> > > CellsByDim;

  class CellImpl: boost::noncopyable {
  public:
	 typedef typename SComplex::Color Color;

	 typename Cells::ObjectPtrsIterator iteratorInCells;
	 typename CellsByDim::value_type::ObjectPtrsIterator iteratorInCellsByDim;

	 explicit CellImpl(const Id& _id, const Dim& _dim, const Color& _color): id(_id), dim(_dim), color(_color) {}

	 const Color& getColor() const { return color; }
	 const Id& getId() const { return id; }
	 const Dim& getDim() const { return dim; }
	 void setColor(const Color& _color) { this->color = _color; }

	 bool operator<(const CellImpl& c) const { return id < c.id; }

  private:
	 const Id id;
	 const Dim dim;
	 Color color;
  };

  typedef CellImpl* CellImplPtr;


  class ConstCellImpl {
  public:
	 typedef typename SComplex::Dim Dim;
	 typedef typename SComplex::Color Color;
	 typedef typename SComplex::Id Id;

	 explicit ConstCellImpl(): complex(NULL), cell(NULL) {} // TODO remove with dummyCells in algorithms
	 explicit ConstCellImpl(const SComplex& _complex): complex(NULL), cell(NULL) {} // TODO remove with dummyCells in algorithms

	 explicit ConstCellImpl(const SComplex* _complex, CellImpl* _cell): complex(_complex), cell(_cell) {}

	 const Color& getColor() const { return cell->getColor(); }
	 const Id& getId() const { return cell->getId(); }
	 const Dim& getDim() const { return cell->getDim(); }

	 bool operator<(const ConstCellImpl& c) const { return *cell < *(c.cell); }

  protected:
	 const SComplex* complex;
	 CellImpl* cell;
  };

  class NonConstCellImpl: public ConstCellImpl {
  public:
  	 explicit NonConstCellImpl(): ConstCellImpl(), complex(NULL) {} // Needed by AKQ
	 explicit NonConstCellImpl(const SComplex& _complex): ConstCellImpl(_complex), complex(NULL) {} // TODO remove with dummyCells in algorithms

	 explicit NonConstCellImpl(SComplex* _complex, CellImpl* _cell): ConstCellImpl(_complex, _cell), complex(_complex) {}

	 void setColor(const Color& newColor) {
		for (typename NeighboursModel::AllObjects::iterator it = complex->boundaries[this->getId()].allObjects().begin(),
				 end = complex->boundaries[this->getId()].allObjects().end(); it != end; ++it) {
		  complex->coboundaries[it->cell->getId()].changeColor(it->iteratorInNeighbour, this->getColor(), newColor);
		}

		for (typename NeighboursModel::AllObjects::iterator it = complex->coboundaries[this->getId()].allObjects().begin(),
				 end = complex->coboundaries[this->getId()].allObjects().end(); it != end; ++it) {
		  complex->boundaries[it->cell->getId()].changeColor(it->iteratorInNeighbour, this->getColor(), newColor);
		}

		complex->cells.changeColor(cell->iteratorInCells, cell->getColor(), newColor);
		complex->cellsByDim[cell->getDim()].changeColor(cell->iteratorInCellsByDim, cell->getColor(), newColor);
		cell->setColor(newColor);
	 }

	 template<Color color>
	 void setColor() { setColor(color); }

  private:
	 using ConstCellImpl::cell;
	 SComplex* complex;
  };

public:

  template<typename ImplT>
  class CellProxy: public BasicCellProxy<ImplT> {
  public:
	 CellProxy() {}
  	 CellProxy(const ImplT& impl): BasicCellProxy<ImplT>(impl) {}
	 CellProxy(const SComplex& s): BasicCellProxy<ImplT>(ImplT(s)) {}
  	 Id getId() const { return BasicCellProxy<ImplT>::getImpl()->getId(); }
  };

  typedef CellProxy<NonConstCellImpl> Cell;
  typedef CellProxy<ConstCellImpl> ConstCell;

  struct NeighbourLink;
  typedef typename Traits::template ColoredObjectsModel<NeighbourLink> NeighboursModel;

  struct NeighbourLink {
	 typedef typename NeighboursModel::ObjectPtrsIterator IteratorInNeighbour;
	 CellImpl* cell;
	 IteratorInNeighbour iteratorInNeighbour; //an iterator to NeighbourLinkPtrs not in this, but in an instance of the class for a neighbour.

	 explicit NeighbourLink(const CellImplPtr& _cell) : cell(_cell) {}
  };

private:

  template<typename CellType>
  struct CellFromNeighbourLinkExtractor: public std::unary_function<const NeighbourLink&, CellProxy<CellType> >
  {
	 SComplex* complex;
	 explicit CellFromNeighbourLinkExtractor(SComplex* _complex): complex(_complex) {}

	 CellProxy<CellType> operator()(const NeighbourLink&  link) const {
		return CellType(complex, link.cell);
	 }
  };

  template<typename CellType>
  struct CellFromCellsExtractor: public std::unary_function<const CellImplPtr&, CellProxy<CellType> >
  {
	 SComplex* complex;
	 explicit CellFromCellsExtractor(SComplex* _complex): complex(_complex) {}

	 CellProxy<CellType> operator()(const CellImplPtr& cell) const {
		return CellType(complex, cell);
	 }
  };

  template<typename CellType>
  struct CellFromCellsByDimExtractor: public std::unary_function<const boost::reference_wrapper<CellImpl>&, CellProxy<CellType> >
  {
	 SComplex* complex;
	 explicit CellFromCellsByDimExtractor(SComplex* _complex): complex(_complex) {}

	 CellProxy<CellType> operator()(const boost::reference_wrapper<CellImpl>& cell) const {
		return CellType(complex, cell.get_pointer());
	 }
  };

  template<bool isConst>
  class IteratorsImpl {
	 typedef typename boost::mpl::if_c<isConst, ConstCellImpl, NonConstCellImpl>::type CellType;
  public:

	 typedef Util::Iterators::RangeTransform<const typename Cells::AllObjects, CellFromCellsExtractor<CellType> > AllCells;
	 typedef Util::Iterators::RangeTransform<const typename CellsByDim::value_type::AllObjects, CellFromCellsByDimExtractor<CellType> > DimCells;
	 typedef Util::Iterators::RangeTransform<const typename NeighboursModel::AllObjects, CellFromNeighbourLinkExtractor<CellType> > BdCells, CbdCells;

	 explicit IteratorsImpl(SComplex* _complex): complex(_complex) {}

	 AllCells allCells() {
		return AllCells(complex->cells.allObjects(), CellFromCellsExtractor<CellType>(complex));
	 }

	 DimCells dimCells(const Dim& dim) {
		return DimCells(complex->cellsByDim[dim].allObjects(), CellFromCellsByDimExtractor<CellType>(complex));
	 }

	 template<typename ImplT>
	 BdCells bdCells(const CellProxy<ImplT>& cell) {
		return BdCells(complex->boundaries[cell.getImpl()->getId()].allObjects(), CellFromNeighbourLinkExtractor<CellType>(complex));
	 }

	 template<typename ImplT>
	 CbdCells cbdCells(const CellProxy<ImplT>& cell) {
		return CbdCells(complex->coboundaries[cell.getImpl()->getId()].allObjects(), CellFromNeighbourLinkExtractor<CellType>(complex));
	 }

  private:
	 SComplex* complex;
  };

  template<bool isConst>
  class ColoredIteratorsImpl {

	 class IteratorsImpl {
		typedef typename boost::mpl::if_c<isConst, ConstCellImpl, NonConstCellImpl>::type CellType;
	 public:

		typedef Util::Iterators::RangeTransform<const typename Cells::ObjectsInColor, CellFromCellsExtractor<CellType> > AllCells;
		typedef Util::Iterators::RangeTransform<const typename CellsByDim::value_type::ObjectsInColor, CellFromCellsByDimExtractor<CellType> > DimCells;
		typedef Util::Iterators::RangeTransform<const typename NeighboursModel::ObjectsInColor, CellFromNeighbourLinkExtractor<CellType> > BdCells, CbdCells;

		explicit IteratorsImpl(SComplex* _complex, const Color& _color): complex(_complex), color(_color)  {}

		AllCells allCells() {
		  return AllCells(complex->cells.objectsInColor(color), CellFromCellsExtractor<CellType>(complex));
		}

		DimCells dimCells(const Dim& dim) {
		  return DimCells(complex->cellsByDim[dim].objectsInColor(color), CellFromCellsByDimExtractor<CellType>(complex));
		}

		template<typename ImplT>
		BdCells bdCells(const CellProxy<ImplT>& cell) {
		  return BdCells(complex->boundaries[cell.getImpl()->getId()].objectsInColor(color), CellFromNeighbourLinkExtractor<CellType>(complex));
		}

		template<typename ImplT>
		CbdCells cbdCells(const CellProxy<ImplT>& cell) {
		  return CbdCells(complex->coboundaries[cell.getImpl()->getId()].objectsInColor(color), CellFromNeighbourLinkExtractor<CellType>(complex));
		}

	 private:
		SComplex* complex;
		Color color;
	 };

  public:
	 typedef IteratorsImpl Iterators;

	 template<Color color>
	 class Color {
	 public:
		typedef IteratorsImpl Iterators;
	 };
  };

public:

  typedef IteratorsImpl<false> Iterators;
  typedef IteratorsImpl<true> ConstIterators;


  typedef ColoredIteratorsImpl<false> ColoredIterators;
  typedef ColoredIteratorsImpl<true> ColoredConstIterators;

  typedef std::vector<boost::tuple<Id, Id, int> > KappaMap;
  typedef std::vector<Dim> Dims;

  //TODO do it as a template <KappaMap, Dims>
  SComplex(Size colors, const Dims& dims, const KappaMap& kappaMap = KappaMap(), const Color& defaultColor = 0): boundaries(dims.size()), coboundaries(dims.size()), nonConstThis(*this) {
	 using boost::get;

	 size_t _size = dims.size();

	 // init dimension related structures
	 dim = _size > 0 ? *std::max_element(dims.begin(), dims.end()) : 0;
	 std::vector<Size> cellsInDim(dim + 1);

	 cellsByDim.resize(dim + 1);

	 for (Id id = 0; id < _size; ++id) {
		++cellsInDim[dims[id]];
	 }

	 for (Dim d = 0; d <= dim; ++d) {
		cellsByDim[d].init(colors, cellsInDim[d]);
	 }

	 //init cells
	 cells.init(colors, _size);

	 //    TODO !!!!
	 //
	 // DON"T USE NEW, use local memory !!!!!!!!!!

	 for (Id id = 0; id < _size; ++id) {
		CellImplPtr cell(new CellImpl(id, dims[id], defaultColor));
		cell->iteratorInCells = cells.add(cell, defaultColor);
		cell->iteratorInCellsByDim = cellsByDim[cell->getDim()].add(boost::ref(*(cell)), defaultColor);
	 }

	 // init neighbours
	 std::vector<Size> boundariesSize(_size);
	 std::vector<Size> coboundariesSize(_size);
	 BOOST_FOREACH(typename KappaMap::value_type kappa, kappaMap) {
		++boundariesSize[get<0>(kappa)];
		++coboundariesSize[get<1>(kappa)];
	 }

	 for (Size i = 0; i < _size; ++i) {
		boundaries[i].init(colors, boundariesSize[i]);
		coboundaries[i].init(colors, coboundariesSize[i]);
	 }

	 BOOST_FOREACH(typename KappaMap::value_type kappa, kappaMap) {
		Id coface = get<0>(kappa);
		Id face = get<1>(kappa);
// 		std::cout << "Kappa " << coface << "[" << dims[coface] << "]"
// 			  << face << "[" << dims[face] << "]" << get<2>(kappa) << std::endl;
		BOOST_ASSERT(coface != face);
		typename NeighbourLink::IteratorInNeighbour bdIteratorInNeighbour = boundaries[coface].add(NeighbourLink(cells.allObjects()[face]), cells.allObjects()[face]->getColor());
		typename NeighbourLink::IteratorInNeighbour cbdIteratorInNeighbour = coboundaries[face].add(NeighbourLink(cells.allObjects()[coface]), cells.allObjects()[coface]->getColor());
		boundaries[coface].allObjects().back().iteratorInNeighbour = cbdIteratorInNeighbour;
		coboundaries[face].allObjects().back().iteratorInNeighbour = bdIteratorInNeighbour;

		bool was = coincidence.insert(std::make_pair(std::make_pair(coface, face), get<2>(kappa))).second;
		BOOST_ASSERT(was);
	 }

  }

  Size size() const { return nonConstThis.get().cells.allObjects().size(); }

  Dim getDim() const { return dim; }

  Cell operator[](const Id id) {
	 return NonConstCellImpl(this, cells.allObjects()[id]);
  }

  ConstCell operator[](const Id id) const {
	 return ConstCellImpl(this, cells.allObjects()[id]);
  }

  template<typename ImplT1, typename ImplT2>
  int coincidenceIndex(const CellProxy<ImplT1> &a, const CellProxy<ImplT2> &b) const {
	 typename std::map<std::pair<Id, Id>, int>::const_iterator it = coincidence.find(std::make_pair(a.getImpl()->getId(), b.getImpl()->getId()));
	 if (it != coincidence.end()) {
		return it->second;
	 }
	 return 0;
  }

  Iterators iterators() { return Iterators(nonConstThis.get_pointer()); }
  ConstIterators iterators() const { return ConstIterators(nonConstThis.get_pointer()); }

  typename ColoredConstIterators::Iterators iterators(const Color& color) const { return typename ColoredConstIterators::Iterators(nonConstThis.get_pointer(), color); }
  typename ColoredIterators::Iterators iterators(const Color& color) { return typename ColoredIterators::Iterators(nonConstThis.get_pointer(), color); }

  template<Color color>
  typename ColoredIterators::template Color<color>::Iterators iterators() { return iterators(color); }

  template<Color color>
  typename ColoredConstIterators::template Color<color>::Iterators iterators() const { return iterators(color); }

private:
  Dim dim;
  Cells cells;
  CellsByDim cellsByDim;
  std::vector<NeighboursModel> boundaries, coboundaries; // (co)boundaries by cell id
  boost::reference_wrapper<SComplex> nonConstThis;
  std::map<std::pair<Id, Id>, int> coincidence;
};


#endif // _SCOMPLEX_HPP

