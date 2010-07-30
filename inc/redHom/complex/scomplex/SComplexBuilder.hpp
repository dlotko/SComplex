#ifndef __SCOMPLEX_BUILDER_HPP_
#define __SCOMPLEX_BUILDER_HPP_

#include "SComplex.hpp"
#include <map>
#include <set>
#include <vector>
#include <iostream>

#include <boost/shared_ptr.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/assign/list_inserter.hpp>
#include <boost/tuple/tuple.hpp>

template<typename Element, typename TraitsT>
class SComplexBuilder {

  
  typedef typename SComplex<TraitsT>::Id Id;
  typedef  std::vector<std::map<Element, Id> > ElementsByHash;

  ElementsByHash elementsByHash;
  Id nextId;
  typename SComplex<TraitsT>::Dims dims;
  typename SComplex<TraitsT>::KappaMap kappa;
  
  Id buildElementsByHash(const Element& element) {
	 using namespace boost::assign;
	 
	 typename ElementsByHash::iterator hashIt = elementsByHash.begin() + element.getHash();	 
	 typename ElementsByHash::value_type::iterator elementIt = hashIt->find(element);
	 bool newElement = false;
	 
	 if (elementIt == hashIt->end()) {
		newElement = true;
		elementIt = hashIt->insert(std::make_pair(element, nextId)).first;
		dims.push_back(element.getSize() - 1);
		//element.print(std::cout) << " " << nextId << " " << dims.back() << std::endl;
		++nextId;
	 
		// int sgn = 1;
		// for (size_t i = 0, end = element.getSize(); element.getSize() > 1 && i < end; ++i) {
		//   Element bound = element.createWithout(i);
		//   Id boundId = buildElementsByHash(bound);
		//   if (newElement) {
		// 	 kappa.push_back(boost::make_tuple(elementIt->second, boundId, sgn));		
		// 	 sgn = -sgn;
		//   }
		// }
	 }
	 
	 return elementIt->second;
  }

public:
  typedef typename SComplex<TraitsT>::Color Color;
  const size_t hashFactor;
  
  SComplexBuilderFromElements(size_t _hashFactor): hashFactor(_hashFactor) {
	 elementsByHash.resize(_hashFactor);
  }
  
  template<typename CollectionT>
  boost::shared_ptr<SComplex<TraitsT> > operator()(const CollectionT& elements, size_t colors, const Color& defaultColor) {
	 nextId = 0;
	 elementsByHash.clear();
	 kappa.clear();
	 dims.clear();
	 
	 for (typename CollectionT::const_iterator it = elements.begin(), end = elements.end(); it != end; ++it) {
		buildElementsByHash(Element(*it, hashFactor));
	 }

	 return boost::shared_ptr<SComplex<TraitsT> >(new SComplex<TraitsT>(colors, dims, kappa, defaultColor));
  }
  
};


#endif
