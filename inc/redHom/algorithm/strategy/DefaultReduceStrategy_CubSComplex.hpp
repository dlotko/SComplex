#ifndef SCOMPLEX_ALGS_CUBSCOMPLEX_REDUCE_STRATEGY_HPP_
#define SCOMPLEX_ALGS_CUBSCOMPLEX_REDUCE_STRATEGY_HPP_


#include "../../RedHomCAPD.h"

#include "DefaultReduceStrategy.hpp"
#include "DefaultReduceStrategyTraits_CubSComplex.hpp"
#include "redHom/complex/cubical/CubSComplex.hpp"

template<int DIM>
class DefaultReduceStrategy<CubSComplex<DIM> >: public DefaultReduceStrategyBase<CubSComplex<DIM> >
{
protected:
    typename CubSComplex<DIM>::DynamicCell dynamicCell;
    using DefaultReduceStrategyBase<CubSComplex<DIM> >::complex;
public:
    typedef typename DefaultReduceStrategyBase<CubSComplex<DIM> >::Traits Traits;

    DefaultReduceStrategy(CubSComplex<DIM>& complex): DefaultReduceStrategyBase<CubSComplex<DIM> >(complex), dynamicCell(complex) {}

    template<typename ArgT>
    typename Traits::template GetCoreductionPair<ArgT>::result_type
    getCoreductionPair(const ArgT& cell)
    {
        if (complex.getUniqueFace(cell, dynamicCell))
        {
            return typename Traits::template GetCoreductionPair<ArgT>::result_type(dynamicCell.getImpl());
        }
        else
        {
            return typename Traits::template GetCoreductionPair<ArgT>::result_type();
        }
    }

    template<typename ArgT>
    typename Traits::template GetReductionPair<ArgT>::result_type
    getReductionPair(const ArgT& cell)
    {
        if (complex.getUniqueCoFace(cell, dynamicCell))
        {
            return typename Traits::template GetReductionPair<ArgT>::result_type(dynamicCell.getImpl());
        }
        else
        {
            return typename Traits::template GetReductionPair<ArgT>::result_type();
        }
    }

    size_t getMaxDim()
    {
        return complex.getDim();
    }
};

#endif
