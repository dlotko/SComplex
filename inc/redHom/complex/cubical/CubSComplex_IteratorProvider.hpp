#ifndef CUB_SCOMPLEX_ITERATOR_PROVIDER_HPP
#define CUB_SCOMPLEX_ITERATOR_PROVIDER_HPP

#include "CubSComplex.hpp"
#include <boost/mpl/if.hpp>
#include <iterator>


template<int DIM>
template<typename NumeratorT, bool isConst>
class CubSComplex<DIM>::IteratorProvider {
public:
	 typedef NumeratorT Numerator;

private:
	 template<typename ValueT>
	 class IteratorFromNumeratorAdapter;

	 typedef typename boost::mpl::if_c<isConst, const typename Numerator::value_type, typename Numerator::value_type>::type ValueT;
public:
	 
	 typedef IteratorFromNumeratorAdapter<ValueT>  iterator;

	 IteratorProvider(const Numerator& numerator): first(numerator), last(findLast(numerator)) {}
	 
	 iterator begin() const {
		  Numerator tmp = first;
		  tmp.MoveNext();
		  return iterator(tmp);
	 }

	 iterator end() const { return iterator(last); }

private:

	 static Numerator findLast(Numerator numerator) {
		  numerator.toEnd();
		  return numerator;
	 }
	 
	 const Numerator first, last;
};


template<int DIM>
template<typename NumeratorT, bool isConst>
template<typename ValueT>
class CubSComplex<DIM>::IteratorProvider<NumeratorT, isConst>::IteratorFromNumeratorAdapter: public std::iterator<forward_iterator_tag, ValueT> {

	 typedef std::iterator<forward_iterator_tag, ValueT> Base;
	 typedef typename IteratorProvider<NumeratorT, isConst>::Numerator Numerator;
	 Numerator currentNumerator;


  
public:

	 explicit IteratorFromNumeratorAdapter(const Numerator& numerator): currentNumerator(numerator) {
	 }

	 IteratorFromNumeratorAdapter& operator++() {
		  currentNumerator.MoveNext();
		  return *this;
	 }

	 IteratorFromNumeratorAdapter operator++(int) {
		  IteratorFromNumeratorAdapter tmp(*this);
		  currentNumerator.MoveNext();
		  return tmp;
	 }

  //typename Base::reference operator*() {
  typename Base::value_type operator*() {
		  return currentNumerator.Current();
	 }

  //typename Base::pointer operator->()  {
  typename Base::value_type operator->()  {
		  return currentNumerator.Current();
	 }

	 bool operator==(const IteratorFromNumeratorAdapter& o) const {
		  return this->currentNumerator == o.currentNumerator;
	 }

	 bool operator!=(const IteratorFromNumeratorAdapter& o) const {
		  return this->currentNumerator != o.currentNumerator;
	 }

};

#endif

