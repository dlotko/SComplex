#ifndef SCOMPLEX_ALGS_DEFAULT_REDUCE_STRATEGY_HPP_
#define SCOMPLEX_ALGS_DEFAULT_REDUCE_STRATEGY_HPP_

#include <boost/optional.hpp>
#include <boost/foreach.hpp>
#include <utility>

template<typename SComplexT>
class DefaultReduceStrategyTraits {
public:

  // template<typename ImplT>
  // struct Proxy: public CellProxy<ImplT> {
  // 	 template<typename ImplT2>
  // 	 Proxy(const ImplT2& impl): CellProxy<ImplT>(impl) {}

  // 	 Proxy(const SComplexT& c): CellProxy<ImplT>(ImplT(c)) {}
  // };

  // template<typename ImplT>
  // struct Proxy<CellProxy<ImplT> >: public CellProxy<ImplT> {
  // 	 template<typename ImplT2>
  // 	 Proxy(const ImplT2& impl): CellProxy<ImplT>(impl) {}
  // };

  // template<typename ImplT>
  // static Proxy<ImplT*> makeProxy(const CellProxy<ImplT>& impl) {
  // 	 return Proxy<ImplT*>(impl.getImpl());
  // }


    template<typename ArgT>
struct GetReductionPair : public std::unary_function<const ArgT&,
                boost::optional<typename SComplexT::Cell> > {};

    template<typename ArgT>
struct GetCoreductionPair : public std::unary_function<const ArgT&,
                boost::optional<typename SComplexT::Cell> > {};

    struct ForceCoreduction
    {
        typedef boost::optional<std::pair<typename SComplexT::Cell,
        typename SComplexT::Cell> > result_type;
    };

    struct Extract
    {
        typedef boost::optional<typename SComplexT::Cell >  result_type;
    };

    /*

  template<typename ArgT>
  struct GetReductionPair : public std::unary_function<ArgT,
														boost::optional<typename SComplexT::Cell> > {};;

  template<typename ArgT>
  struct GetCoreductionPair : public std::unary_function<const ArgT&,
														boost::optional<typename SComplexT::Cell> > {};

  struct ForceCoreduction {
	 typedef boost::optional<std::pair<typename SComplexT::Cell,
												  typename SComplexT::Cell> > result_type;


  };

  struct Extract {
	 typedef boost::optional<typename SComplexT::Cell >  result_type;
  };

  */
};

template<typename SComplexT>
class DefaultReduceStrategyBase {

public:
  typedef SComplexT SComplex;
  typedef DefaultReduceStrategyTraits<SComplex> Traits;
  typedef typename SComplex::Cell Cell;


  DefaultReduceStrategyBase(SComplex& _complex): complex(_complex), dummyCell2(_complex),  dummyCell3(_complex),
						 extracted(false) {}

  SComplex& getComplex() const {
	 return complex;
  }

  template<typename ImplT>
  static bool reduced(const typename SComplex::template CellProxy<ImplT>& cell) {
  	 return cell.getColor() == 2;
  }

  template<typename ImplT1, typename ImplT2>
  static void coreduce(const typename SComplex::template CellProxy<ImplT1>& a, const typename SComplex::template CellProxy<ImplT2>& b)  {
  	 a.template setColor<2>();
  	 b.template setColor<2>();
  }

  template<typename ImplT1, typename ImplT2>
  static void reduce(const typename SComplex::template CellProxy<ImplT1>& a, const typename SComplex::template CellProxy<ImplT2>& b)  {
  	 a.template setColor<2>();
  	 b.template setColor<2>();
  }

  template<typename ImplT>
  static void reduce(const typename SComplex::template CellProxy<ImplT>& cell) {
	 cell.template setColor<2>();
  }

  typename Traits::Extract::result_type extract() {
    if (!extracted) {
      extracted = true;
      typename SComplex::ColoredIterators::Iterators::DimCells dimCells = complex.iterators(1).dimCells(0);
      typename SComplex::ColoredIterators::Iterators::DimCells::iterator end = dimCells.end(),
	it = dimCells.begin();
      
      if (it != end) {
	return typename Traits::Extract::result_type::value_type(*it);
      }
    }
    return typename Traits::Extract::result_type();
  }

  static typename Traits::ForceCoreduction::result_type forceCoreductionPair() {
  	 return typename Traits::ForceCoreduction::result_type();
  }

  template<typename ArgT>
  typename Traits::template GetCoreductionPair<ArgT>::result_type
  getCoreductionPair(const ArgT& cell)
  {
  	 int times = 0;
  	 BOOST_FOREACH(typename SComplex::ColoredIterators::Iterators::BdCells::iterator::value_type v,
  						complex.iterators(1).bdCells(cell)) {
  		if (times == 0) {
  		  dummyCell3 = v;
  		}
  		++times;
  		if (times == 2) {
		  break;
  		}
  	 }

  	 if (times == 1) {
  		return typename Traits::template GetCoreductionPair<ArgT>::result_type(dummyCell3);
  	 }
  	 return typename Traits::template GetCoreductionPair<ArgT>::result_type();
  }

  template<typename ArgT>
  typename Traits::template GetCoreductionPair<ArgT>::result_type
  getReductionPair(const ArgT& cell)
  {
  	 int times = 0;
  	 BOOST_FOREACH(typename SComplex::ColoredIterators::Iterators::CbdCells::iterator::value_type v,
  						complex.iterators(1).cbdCells(cell)) {
  		if (times == 0) {
  		  dummyCell2 = v;
  		}
  		++times;
  		if (times == 2) {
  		  break;
  		}
  	 }

  	 if (times == 1) {
  		return typename Traits::template GetReductionPair<ArgT >::result_type(dummyCell2);
  	 }
  	 return typename Traits::template GetReductionPair<ArgT>::result_type();
  }

protected:
  SComplex& complex;
  Cell dummyCell2, dummyCell3;
  bool extracted;
};

template<typename SComplexT>
class DefaultReduceStrategy: public DefaultReduceStrategyBase<SComplexT> {
public:
  DefaultReduceStrategy(SComplexT& _complex): DefaultReduceStrategyBase<SComplexT>(_complex) {}

};

#endif

