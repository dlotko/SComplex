#ifndef ALGS_AKQ_REDUCE_STRATEGY_HPP_
#define ALGS_AKQ_REDUCE_STRATEGY_HPP_

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
class AKQReduceStrategy : public DefaultReduceStrategy<SComplexT>
{
protected:
    using DefaultReduceStrategyBase<SComplexT>::complex;

public:
    enum AKQType {UNSET, KING, QUEEN, ACE};

    typedef std::map<int,int> PathsInfo;

    typedef ::SComplex<SComplexDefaultTraits> OutputComplexType;
    typedef SComplexT SComplex;
    typedef DefaultReduceStrategyTraits<SComplex> Traits;
    typedef typename SComplex::Cell Cell;

    typedef CellDimIndexer<Cell> DimIndexerType;
    DimIndexerType dimIndexer;
    typename DimIndexerType::iterator extractIt;
    typename DimIndexerType::iterator extractEnd;

    AKQReduceStrategy(SComplex& _complex): DefaultReduceStrategy<SComplex>(_complex)
    {
        Stopwatch total;
    	// std::cout << "Starting init\n";

        maxExtractDim = _complex.getDim(); // std::cout << __LINE__ << " @ " << total << std::endl;

        extractDim = 0;

        int csize = _complex.maxId(); // std::cout << __LINE__ << " @ " << total << std::endl;
        //int csize = _complex.size(); // std::cout << __LINE__ << " @ " << total << std::endl;
        int card = distance(_complex.iterators(1).allCells().begin(), _complex.iterators(1).allCells().end());

        morse.resize(csize, 0);

        kingsUsed = 0;

        {
        Cell king(_complex);
        kingPool.resize(card/2 + 10, king); // std::cout << __LINE__ << " [kingPool allocation] @ " << total << std::endl;
        }

        infosUsed = 0;
        {
        PathsInfo pi;
        pathsInfoPool.resize(card, pi); // std::cout << __LINE__ << " [pathsInfoPool allocation] @ " << total << std::endl;
        }

        dimIndexer = DimIndexerType(this->complex.iterators(1).allCells().begin(),
                                    this->complex.iterators(1).allCells().end(),
                                    maxExtractDim);

		 std::cout << __LINE__ << " @ " << total << std::endl;

        extractIt = dimIndexer.begin(); std::cout << __LINE__ << " @ " << total << std::endl;
        extractEnd = dimIndexer.end(); std::cout << __LINE__ << " @ " << total << std::endl;

        followMemoTable.resize(csize, 0); std::cout << __LINE__ << " @ " << total << std::endl;

        akq.resize(csize); std::cout << __LINE__ << "[AKQ resize] @ " << total << std::endl;
        // kerKing.resize(csize, Cell(this->complex)); // std::cout << __LINE__ << " [herKing allocation] @ " << total << std::endl;

		kerKing.resize(csize, 0); std::cout << __LINE__ << " [herKing allocation] @ " << total << std::endl;

        // std::cout << "Init done in " << total << std::endl;
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
        }
        else
        {
            kingGetsMarried(b, a);
        }

        a.template setColor<2>();
        b.template setColor<2>();
    }

    typename Traits::Extract::result_type extract()
    {
        while (extractIt != extractEnd && extractIt->getColor() != 1)
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
        kerKing[queen.getId()] = &(kingPool[kingsUsed++] = king);
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

    template<typename T1, typename T2>
    const PathsInfo& followPathMemo(T1& curr, T2& from)
    {
    	PathsInfo *&coeffs = followMemoTable[curr.getId()];

    	if (coeffs != 0)
            return *coeffs;

        if (akq[curr.getId()] == UNSET)
            return emptyPathsInfo;

        BOOST_ASSERT(akq[curr.getId()] != ACE || curr.getId() == from.getId());

        // coeffs = new PathsInfo();'
        coeffs = &pathsInfoPool[infosUsed++];

        int ourMorseValue = morse[curr.getId()];

        // case 1: to ace
        BOOST_FOREACH(typename SComplex::Iterators::BdCells::iterator::value_type to, this->complex.iterators().bdCells(curr))
        {
            if (akq[to.getId()] == ACE && morse[to.getId()] < ourMorseValue)
            {
                int newCoeff = toAceCoeff(curr, to);
                (*coeffs)[to.getId()] += newCoeff;
            }
        }
        // case 2: to king
        BOOST_FOREACH(typename SComplex::Iterators::BdCells::iterator::value_type bd, this->complex.iterators().bdCells(curr))
        {
            if (akq[bd.getId()] != QUEEN)
                continue;

            Cell& to = *(kerKing[bd.getId()]);

            if (akq[to.getId()] != KING)
            {
                continue;
            }

            BOOST_ASSERT(akq[to.getId()] == KING);

            if (morse[to.getId()] < ourMorseValue)
            {
                int newCoeff = toKingCoeff(curr, to, bd);

                const PathsInfo &fromHere = followPathMemo(to, from);

                for (PathsInfo::const_iterator it = fromHere.begin(); it != fromHere.end(); ++it)
                {
                    (*coeffs)[it->first] += it->second*newCoeff;
                }
            }
        }

        return *coeffs;
    }

    void reportPaths()
    {
		Stopwatch total;
        std::cout << "Starting memoized coefficient calculation\n";

        std::cerr << "#aces: " << aces.size() << std::endl;

        BOOST_FOREACH(Cell ace, aces)
        {
            std::cerr << ace.getDim() << std::endl;
            const PathsInfo &cf = followPathMemo(ace, ace);
            for (PathsInfo::const_iterator it = cf.begin(); it != cf.end(); ++it)
            {
                coeffs[std::make_pair(ace.getId(), it->first)] += it->second;
            }
        }

        // std::cout << "done in " << total << std::endl;

        typedef std::pair<std::pair<int,int>,int> Triple;

        std::vector<OutputComplexType::Dim> dims(aces.size());

        std::map<size_t, size_t> from0;

        BOOST_FOREACH(Cell ace, aces)
        {
            size_t next = from0.size();
            dims[next] = ace.getDim();
            from0[ace.getId()] = next;
        }

        OutputComplexType::KappaMap kap;

        BOOST_FOREACH(Triple p, coeffs)
        {
            int coef = p.second;
            kap.push_back(boost::make_tuple(from0[p.first.first], from0[p.first.second], coef));
        }

        outputComplex = new OutputComplexType(3, dims, kap, 1);
    }

    std::vector<int> morse;
    std::vector<AKQType> akq;
    /// std::vector<boost::optional<Cell> > kerKing;

    int kingsUsed;
    std::vector<Cell> kingPool;
    std::vector<Cell*> kerKing;

    std::vector<Cell> aces;

    int infosUsed;
    std::vector<PathsInfo> pathsInfoPool;

    std::vector<PathsInfo*> followMemoTable;

    int extractDim;
    int maxExtractDim;
    std::map<std::pair<int,int>, int> coeffs;

    PathsInfo emptyPathsInfo;
    OutputComplexType *outputComplex;
};

#endif

