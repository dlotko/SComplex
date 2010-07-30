#ifndef ALGS_Optimistic_AKQ_REDUCE_STRATEGY_HPP_
#define ALGS_Optimistic_AKQ_REDUCE_STRATEGY_HPP_

#include <boost/optional.hpp>
#include <boost/foreach.hpp>
#include <boost/iterator/filter_iterator.hpp>

#include <utility>
#include <stack>

#include <map>
#include <vector>

#include "redHom/complex/scomplex/SComplex.hpp"
#include "redHom/complex/scomplex/SComplexDefaultTraits.hpp"

#include <redHom/algorithm/Algorithms.hpp>
#include "redHom/algorithm/strategy/DefaultReduceStrategy.hpp"
#include "redHom/algorithm/strategy/DefaultReduceStrategy_CubSComplex.hpp"
#include "redHom/algorithm/Coreduction.hpp"

#include "redHom/algorithm/CellDimIndexer.hpp"

template<typename SComplexT>
class OptimisticAKQReduceStrategy : public DefaultReduceStrategy<SComplexT>
{
protected:
    using DefaultReduceStrategyBase<SComplexT>::complex;

public:
    enum AKQType {UNSET, KING, QUEEN, ACE};

    typedef ::SComplex<SComplexDefaultTraits> OutputComplexType;
    typedef SComplexT SComplex;
    typedef DefaultReduceStrategyTraits<SComplex> Traits;
    typedef typename SComplex::Cell Cell;

	typedef CellDimIndexer<Cell> DimIndexerType;
	typename DimIndexerType::iterator extractIt;
	typename DimIndexerType::iterator extractEnd;

    OptimisticAKQReduceStrategy(SComplex& _complex): DefaultReduceStrategy<SComplex>(_complex)
    {
        // maxExtractDim = _complex.getDim();

		Stopwatch total;
    	std::cout << "Starting init\n";

        maxExtractDim = getMaxDim();
        extractDim = 0;

        int csize = _complex.size();

        bdSize.resize(csize);

        morse.resize(csize, 0);
        nullPathMemo.resize(csize, -1);

        dimIndexer = DimIndexerType(this->complex.iterators(1).allCells().begin(),
			this->complex.iterators(1).allCells().end(),
			maxExtractDim);

		extractIt = dimIndexer.begin();
		extractEnd = dimIndexer.end();

        akq.resize(csize);
        kerKing.resize(csize, Cell(this->complex));

        std::cout << "Init done in " << total << std::endl;
    }

    SComplex& getComplex() const
    {
        return complex;
    }

    template<typename CellT1, typename CellT2>
    void coreduce(CellT1& a, CellT2& b)
    {
        BOOST_ASSERT(a.getDim() != b.getDim());

        if (a.getDim() > b.getDim())
        {
            kingGetsMarried(a, b);
            --bdSize[a.getId()];
        }
        else
        {
            kingGetsMarried(b, a);
            --bdSize[b.getId()];
        }

        a.template setColor<2>();
        b.template setColor<2>();
    }

    typename Traits::Extract::result_type extract()
    {
		while(extractIt != extractEnd && extractIt->getColor() != 1)
			++extractIt;

		if (extractIt != extractEnd)
		{
			BOOST_ASSERT(extractIt->getColor() == 1);

			int v = (extractIt->getDim() == 0) ? 0 : calcMorseValue(*extractIt);

			morse[extractIt->getId()] = v;
			aces.push_back(*extractIt);
			akq[extractIt->getId()] = ACE;

			return typename Traits::Extract::result_type::value_type(*extractIt);
		}

        reportPaths();
        return typename Traits::Extract::result_type();
    }

    OutputComplexType* getOutputComplex()
    {
        return outputComplex;
    }

protected:

    template<typename cellT1>
    int calcMorseValue(const cellT1 &c)
    {
        int val = 0;
        BOOST_FOREACH(typename SComplex::Iterators::BdCells::iterator::value_type el, complex.iterators().bdCells(c))
        {
            val = max(val, morse[el.getId()]);
        }

        return val + 1;
    }

    template<typename T1, typename T2>
    void kingGetsMarried(const T1 &king, const T2 &queen)
    {
        akq[king.getId()] = KING;
        akq[queen.getId()] = QUEEN;
        int v = calcMorseValue(king);
        morse[king.getId()] = v;
        morse[queen.getId()] = v;
        kerKing[queen.getId()] = king;
    }

    template<typename T1, typename T2>
    int toAceCoeff(T1& x, T2& y)
    {
        BOOST_ASSERT(akq[y.getId()] == ACE);
        BOOST_ASSERT(akq[x.getId()] != QUEEN);

        return complex.coincidenceIndex(x, y);
    }

    template<typename T1, typename T2, typename T3>
    int toKingCoeff(T1& x, T2& y, T3& y_star)
    {
        BOOST_ASSERT(akq[x.getId()] != QUEEN);
        BOOST_ASSERT(akq[y_star.getId()] == QUEEN);
        BOOST_ASSERT(akq[y.getId()] == KING);

        return -1 * complex.coincidenceIndex(x, y_star) / complex.coincidenceIndex(y, y_star);
    }

    template<typename T1>
    void followPath(T1& c)
    {
        std::stack<std::pair<Cell, int> > S;

        S.push(std::make_pair(c, 1));

        while (S.size())
        {
            Cell curr = S.top().first;
            int accumulatedWeight = S.top().second;
            S.pop();

            BOOST_ASSERT(akq[curr.getId()] != QUEEN);

            if (curr.getId() != c.getId() && akq[curr.getId()] == ACE)
            {
                coeffs[std::make_pair(c.getId(), curr.getId())] += accumulatedWeight;
                continue;
            }

            int ourMorseValue = morse[curr.getId()];

            // case 1: to ace
            BOOST_FOREACH(typename SComplex::Iterators::BdCells::iterator::value_type to, complex.iterators().bdCells(curr))
            {
                if (akq[to.getId()] == ACE && morse[to.getId()] < ourMorseValue)
                {
                    S.push(std::make_pair(to, accumulatedWeight * toAceCoeff(curr, to)));
                }
            }

            // case 2: to king
            BOOST_FOREACH(typename SComplex::Iterators::BdCells::iterator::value_type bd, complex.iterators().bdCells(curr))
            {
                if (akq[bd.getId()] != QUEEN)
                    continue;

                Cell& to = kerKing[bd.getId()];

                if (akq[to.getId()] == KING && morse[to.getId()] < ourMorseValue)
                {
                    S.push(std::make_pair(to, accumulatedWeight * toKingCoeff(curr, to, bd)));
                }
            }
        }
    }

    template<typename T1, typename T2>
    bool markNullPaths(T1& curr, T2& from)
    {
        int& ok = nullPathMemo[curr.getId()];
        if (ok != -1)
            return static_cast<bool>(ok); // true or false

        if (akq[curr.getId()] == ACE && curr.getId() != from.getId())
            return true;

        if (akq[curr.getId()] == UNSET)
            return false;

        int ourMorseValue = morse[curr.getId()];

        ok = false;

        // case 1: to ace
        BOOST_FOREACH(typename SComplex::Iterators::BdCells::iterator::value_type to, this->complex.iterators().bdCells(curr))
        {
            if (akq[to.getId()] == ACE && morse[to.getId()] < ourMorseValue)
            {
                ok = true;
                break;
            }
        }
        // case 2: to king
        BOOST_FOREACH(typename SComplex::Iterators::BdCells::iterator::value_type bd, this->complex.iterators().bdCells(curr))
        {
            if (akq[bd.getId()] != QUEEN)
                continue;

            Cell& to = kerKing[bd.getId()];

            if (akq[to.getId()] != KING)
            {
                continue;
            }

            BOOST_ASSERT(akq[to.getId()] == KING);

            if (morse[to.getId()] < ourMorseValue)
            {
                if (markNullPaths(to, from))
                {
                    ok = true;
                }
            }
        }

        if (!ok && akq[curr.getId()] != ACE)
        {
            akq[curr.getId()] = UNSET;
        }

        return ok;
    }

    void reportPaths()
    {
    	Stopwatch total;

    	std::cout << "Starting coefficient calculation\n";

    	Stopwatch null;

        BOOST_FOREACH(Cell ace, aces)
        {
            markNullPaths(ace, ace);
        }

        std::cout << "Null paths removed after: " << null << std::endl;

        Stopwatch follow;

        BOOST_FOREACH(Cell ace, aces)
        {
        	followPath(ace);
        }

        std::cout << "All paths followed: " << follow << std::endl;
        std::cout << "All coefficients in: " << total << std::endl;

        typedef std::pair<std::pair<int,int>,int> Triple;

        std::vector<OutputComplexType::Dim> dims(aces.size());

        std::map<size_t, size_t> from0;

        BOOST_FOREACH(Cell ace, aces)
        {
            size_t next = from0.size();
            dims[next] = ace.getDim();
            from0[ace.getId()] = next;
        }

        std::cout << "After remapping numbers: " << total << std::endl;

        OutputComplexType::KappaMap kap;

        BOOST_FOREACH(Triple p, coeffs)
        {
            int coef = p.second;
            kap.push_back(boost::make_tuple(from0[p.first.first], from0[p.first.second], coef));
        }

        std::cout << "After kappa map construction: " << total << std::endl;

        outputComplex = new OutputComplexType(3, dims, kap, 1);

        std::cout << "After SComplex allocation: " << total << std::endl;
    }

    std::vector<int> morse;
    std::vector<AKQType> akq;
    std::vector<Cell> kerKing;
    std::vector<Cell> aces;

    DimIndexerType dimIndexer;

    std::vector<int> nullPathMemo;

    int extractDim;
    int maxExtractDim;
    std::map<std::pair<int,int>, int> coeffs;

    vector<int> bdSize;

    OutputComplexType *outputComplex;
};

#endif

