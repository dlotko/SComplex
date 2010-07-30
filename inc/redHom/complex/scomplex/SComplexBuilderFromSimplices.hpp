#ifndef __SCOMPLEX_BUILDER_FROM_SIMPLICES_HPP_
#define __SCOMPLEX_BUILDER_FROM_SIMPLICES_HPP_

#include "SComplex.hpp"
#include <map>
#include <set>
#include <vector>
#include <iostream>

#include <boost/shared_ptr.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/assign/list_inserter.hpp>
#include <boost/tuple/tuple.hpp>

template<typename T, typename TraitsT>
class SComplexBuilderFromSimplices {

  
  class Simplex {
	 std::vector<T> coords;
	 size_t hash;
	 const size_t hashFactor;
  public:
	 
	 template<typename CoordsT>
	 explicit Simplex(const CoordsT& _coords, size_t _hashFactor): coords(_coords.begin(), _coords.end()), hashFactor(_hashFactor) {
		hash = 0;
		BOOST_FOREACH(typename CoordsT::value_type v, coords) {
		  hash = (hash + v) % hashFactor;
		}

	 }

	 size_t getSize() const {
		return coords.size();
	 }

	 Simplex createWithout(size_t without) const {
		BOOST_ASSERT(coords.size() > 0 && without >= 0 && without < coords.size());
		std::vector<T> newCoords(coords.size() - 1);
		for (size_t i = 0, j = 0; i < coords.size(); ++i) {
		  if (i != without) {
			 newCoords[j++] = coords[i];
		  }
		}
		return Simplex(newCoords, hashFactor);
	 }
	 
	 size_t getHash() const {
		return hash;
	 }
	 
	 bool operator<(const Simplex& s) const {
		return this->coords < s.coords;
	 }

	 bool operator==(const Simplex& s) const {
		return this->hash == s.hash && this->coords == s.coords;
	 }

	 std::ostream& print(std::ostream& out) const {
		out << "[ ";
		for (size_t i = 0; i < coords.size(); ++i) {
		  out << coords[i] << " ";
		}
		out << "]";
		return out;
	 }
  };

  typedef typename SComplex<TraitsT>::Id Id;
  typedef  std::vector<std::map<Simplex, Id> > SimplicesByHash;

  SimplicesByHash simplicesByHash;
  Id nextId;
  typename SComplex<TraitsT>::Dims dims;
  typename SComplex<TraitsT>::KappaMap kappa;

  Id getId(const Simplex& s) const {
	 typename SimplicesByHash::value_type::const_iterator it = simplicesByHash[s.getHash()].find(s);
	 BOOST_ASSERT (it != simplicesByHash[s.getHash()].end());
	 return it->second;
  }
  
  Id buildSimplicesByHash(const Simplex& simplex) {
	 using namespace boost::assign;
	 
	 typename SimplicesByHash::iterator hashIt = simplicesByHash.begin() + simplex.getHash();	 
	 typename SimplicesByHash::value_type::iterator simplexIt = hashIt->find(simplex);
	 bool newSimplex = false;
	 
	 if (simplexIt == hashIt->end()) {
		newSimplex = true;
		simplexIt = hashIt->insert(std::make_pair(simplex, nextId)).first;
		dims.push_back(simplex.getSize() - 1);
		//simplex.print(std::cout) << " " << nextId << " " << dims.back() << std::endl;
		++nextId;
	 
		int sgn = 1;
		for (size_t i = 0, end = simplex.getSize(); simplex.getSize() > 1 && i < end; ++i) {
		  Simplex bound = simplex.createWithout(i);
		  Id boundId = buildSimplicesByHash(bound);
		  if (newSimplex) {
			 kappa.push_back(boost::make_tuple(simplexIt->second, boundId, sgn));		
			 sgn = -sgn;
		  }
		}
	 }
	 
	 return simplexIt->second;
  }

public:
  typedef typename SComplex<TraitsT>::Color Color;
  const size_t hashFactor;
  
  SComplexBuilderFromSimplices(size_t _hashFactor): hashFactor(_hashFactor) {
	 simplicesByHash.resize(_hashFactor);
  }
  
  template<typename CollectionT>
  boost::shared_ptr<SComplex<TraitsT> > operator()(const CollectionT& simplices, size_t colors, const Color& defaultColor) {
	 nextId = 0;
	 simplicesByHash.clear();
	 kappa.clear();
	 dims.clear();
	 
	 for (typename CollectionT::const_iterator it = simplices.begin(), end = simplices.end(); it != end; ++it) {
		buildSimplicesByHash(Simplex(*it, hashFactor));
	 }

	 return boost::shared_ptr<SComplex<TraitsT> >(new SComplex<TraitsT>(colors, dims, kappa, defaultColor));
  }
  
};


#endif
