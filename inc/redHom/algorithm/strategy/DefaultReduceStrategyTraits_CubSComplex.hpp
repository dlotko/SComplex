#ifndef SCOMPLEX_ALGS_CUBSCOMPLEX_REDUCE_STRATEGY_TRAITS_HPP_
#define SCOMPLEX_ALGS_CUBSCOMPLEX_REDUCE_STRATEGY_TRAITS_HPP_

#include "redHom/complex/cubical/CubSComplex.hpp"
#include <functional>
#include <boost/utility/enable_if.hpp>

template<int DIM>
class DefaultReduceStrategyTraits<CubSComplex<DIM> > {
public:

  // template<typename ImplT>
  // struct Proxy: public CubSComplex::CubCellProxy<ImplT> {
  // 	 template<typename ImplT2>
  // 	 Proxy(const ImplT2& impl): CubSComplex::CubCellProxy<ImplT>(impl) {}
  // };

  // template<typename ImplT>
  // struct Proxy<CubSComplex::CubCellProxy<ImplT> >: public CubSComplex::CubCellProxy<ImplT> {
  // 	 template<typename ImplT2>
  // 	 Proxy(const ImplT2& impl): CubSComplex::CubCellProxy<ImplT>(impl) {}
  // };

  // template<typename ImplT>
  // static Proxy<ImplT*> makeProxy(const CubSComplex::CubCellProxy<ImplT>& impl) {
  // 	 return Proxy<ImplT*>(impl.getImpl());
  // }
  typedef CubSComplex<DIM> SComplex;

  template<typename ArgT>
  struct GetReductionPair:  std::unary_function<const ArgT&,
																boost::optional<typename SComplex::template CellProxy<typename SComplex::DynamicCell::Impl*> > > {};
  template<typename ArgT>
  struct GetCoreductionPair:  std::unary_function<const ArgT&,
																  boost::optional<typename SComplex::template CellProxy<typename SComplex::DynamicCell::Impl*> > > {};
  struct ForceCoreduction {
	 typedef boost::optional<std::pair<typename SComplex::template CellProxy<typename SComplex::DynamicCell::Impl*>,
												  typename SComplex::template CellProxy<typename SComplex::DynamicCell::Impl*> > > result_type;
  };

  struct Extract {
	 typedef boost::optional<typename SComplex::template CellProxy<typename SComplex::BitCoordCellImpl> >  result_type;
  };

};

#endif
