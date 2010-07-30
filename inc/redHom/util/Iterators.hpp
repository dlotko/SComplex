#ifndef __ITERATORS_HPP_
#define __ITERATORS_HPP_

#include <functional>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/range.hpp>

namespace Util {
  namespace Iterators {
	 
	 template<bool isConst, typename CollectionT, typename TransformT>
	 class CollectionBeginEnd {
	 public:
		typedef typename boost::mpl::if_c<isConst,
													 const CollectionT&,
													 CollectionT&
													 >::type Collection;
		

		typedef boost::transform_iterator<TransformT, typename boost::mpl::if_c<isConst, typename CollectionT::const_iterator, typename CollectionT::iterator>::type> iterator;
		
		typedef boost::transform_iterator<TransformT, typename CollectionT::const_iterator> const_iterator;
		
		explicit CollectionBeginEnd(Collection _collection): collection(_collection) {}

		iterator begin() const {
		  return iterator(collection.begin(), transform);
		}

		iterator end() const {
		  return iterator(collection.end(), transform);
		}

	 private:
		Collection collection;
		TransformT transform;
	 };

	 template<bool isConst, typename CollectionT, typename TransformT = std::_Identity<typename CollectionT::value_type> >
	 class BeginEnd {
		typedef typename boost::mpl::if_c<isConst, typename CollectionT::const_iterator, typename CollectionT::iterator>::type CollectionIterator;
		typedef typename boost::mpl::if_c<isConst,
													 const CollectionT&,
													 CollectionT&
													 >::type Collection;

	 public:
		typedef boost::transform_iterator<TransformT, CollectionIterator> iterator;
		
		typedef boost::transform_iterator<TransformT, typename CollectionT::const_iterator> const_iterator;
		
		explicit BeginEnd(Collection _collection, const TransformT& _transform = TransformT() ): b(_collection.begin()), e(_collection.end()), transform(_transform) {}
		
		iterator begin() const {
		  return iterator(b, transform);
		}

		iterator end() const {
		  return iterator(e, transform);
		}

	 private:
		CollectionIterator b, e;
		TransformT transform;
	 };

	 template<typename CollectionT, typename TransformT>
	 class RangeTransform {
		typedef boost::sub_range<CollectionT> Range;
		
	 public:
		typedef typename Range::size_type size_type;
		typedef boost::transform_iterator<TransformT, typename Range::iterator> iterator;
		typedef boost::transform_iterator<TransformT, typename Range::const_iterator> const_iterator;

		
		template<typename CollectionT_2>
		RangeTransform(CollectionT_2& _collection, TransformT _transform): range(_collection), transform(_transform) {

		}

		template<typename CollectionT_2>
		RangeTransform(const CollectionT_2& _collection, TransformT _transform): range(_collection), transform(_transform) {
		}

		iterator begin() {
		  return iterator(range.begin(), transform);
		}

		iterator end() {
		  return iterator(range.end(), transform);
		}

		const_iterator begin() const {
		  return const_iterator(range.begin(), transform);
		}

		const_iterator end() const {
		  return const_iterator(range.end(), transform);
		}

	 private:
		Range range;
		TransformT transform;
	 };

  }
}

#endif
