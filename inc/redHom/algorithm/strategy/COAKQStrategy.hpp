#ifndef SCOMPLEX_ALGS_COAKQ_STRATEGY_HPP_
#define SCOMPLEX_ALGS_COAKQ_STRATEGY_HPP_

#include <boost/tuple/tuple.hpp>
#include <boost/optional.hpp>
#include <boost/foreach.hpp>
#include <utility>
#include <map>
#include <vector>
#include <stack>
#include <algorithm>
#include <limits>

#include <iostream>


template<typename SComplexT>
class COAKQStrategyTraits {
public:
  
  template<typename ArgT>
  struct GetReductionPair: public std::unary_function<const ArgT&,
						      boost::optional<typename SComplexT::Cell> > {};

  struct ForceReduction {
    typedef boost::optional<std::pair<typename SComplexT::Cell,
				      typename SComplexT::Cell> > result_type;
  };

  struct Extract {
    typedef boost::optional<typename SComplexT::Cell >  result_type;
  };
};

template<typename SComplexT, typename OutputComplexT>
class COAKQStrategyBase {

public:
  typedef SComplexT SComplex;
  typedef COAKQStrategyTraits<SComplex> Traits;
  typedef typename SComplex::Cell Cell;


  COAKQStrategyBase(SComplex& _complex): complex(_complex), dummyCell2(_complex),  dummyCell3(_complex) {
    maxDim = 0;
    BOOST_FOREACH(typename SComplex::Iterators::AllCells::iterator::value_type v,
		  complex.iterators().allCells()) {
      maxDim = std::max<int>(maxDim, v.getDim());
    }
    dimPhase = maxDim;
  }
  
  SComplex& getComplex() const {
    return complex;
  }

  OutputComplexT& getOutputComplex() {
    return *outputComplex;
  }
  
  template<typename ImplT>
  static bool reduced(const typename SComplex::template CellProxy<ImplT>& cell) {
    return cell.getColor() == 2;
  }

  template<typename ImplT1, typename ImplT2>	 
  void reduce(const typename SComplex::template CellProxy<ImplT1>& a, const typename SComplex::template CellProxy<ImplT2>& b)  {
    BOOST_ASSERT(a.getDim() < b.getDim());

    kingGetsMarried(a, b);

    a.template setColor<2>();
    b.template setColor<2>();
  }

  template<typename ImplT>
  static void reduce(const typename SComplex::template CellProxy<ImplT>& cell) {
    cell.template setColor<2>();
  }
  
  typename Traits::Extract::result_type extract() {
    for (;dimPhase >= 0; dimPhase--) {
      typename SComplex::ColoredIterators::Iterators::DimCells dimCells = complex.iterators(1).dimCells(dimPhase);
      typename SComplex::ColoredIterators::Iterators::DimCells::iterator end = dimCells.end(),
	it = dimCells.begin();

      if (it != end) {
	int v = 0;
	
	if (dimPhase != maxDim) {
	  v = calcMorseValue(*it);
	}
	morse[it->getId()] = v;
	aces.push_back(*it);
	akq[*it] = 'a';

	return typename Traits::Extract::result_type::value_type(*it);
      }
    }

    reportPaths();
    return typename Traits::Extract::result_type();
  }

  typename Traits::ForceReduction::result_type forceReductionPair() {
    for (int d = dimPhase; dimPhase - d <= 1 && d >= 0; d--)
      {

	typedef typename SComplex::ColoredIterators::Iterators::DimCells DimCells;
	DimCells  dimCells = complex.iterators(1).dimCells(d);
	typename DimCells::iterator end = dimCells.end(),
	  it = dimCells.begin();

	while(it != end) {
	  typename Traits::template GetReductionPair<typename DimCells::iterator::value_type>::result_type
	    coface = getReductionPair(*it);
	  if (coface) {
	    return typename Traits::ForceReduction::result_type(std::make_pair(*it, *coface));
	  }
	  ++it;
	}
      }
    return typename Traits::ForceReduction::result_type();
  }

  template<typename ArgT>
  typename Traits::template GetReductionPair<ArgT>::result_type
  getReductionPair(const ArgT& cell)
  {
    int times = 0;
    BOOST_FOREACH(typename SComplex::ColoredIterators::Iterators::CbdCells::iterator::value_type v,
		  complex.iterators(1).cbdCells(cell)) {
      if (times == 0) {
	dummyCell3 = v;
      }
      ++times;
      if (times == 2) {
	break;
      }
    }

    if (times == 1) {
      return typename Traits::template GetReductionPair<ArgT>::result_type(dummyCell3);
    }
    return typename Traits::template GetReductionPair<ArgT>::result_type();
  }


private:

  template<typename T1, typename T2>
  void kingGetsMarried(const T1 &king, const T2 &queen)
  {
    akq[king] = 'k';
    akq[queen] = 'q';
    int v = calcMorseValue(king);
    morse[king.getId()] = v;
    morse[queen.getId()] = v;
    her_king.insert(make_pair(queen, king));
  }



  template<typename cellT1>
  int calcMorseValue(const cellT1 &c)
  {
    int val = 0;
    BOOST_FOREACH(Cell el, complex.iterators().cbdCells(c)) {
      val = std::min(val, morse[el.getId()]);
    }

    return val - 1;
  }
  
  int toAceCoeff(Cell x, Cell y)
  {
    BOOST_ASSERT(akq[y] == 'a');
    BOOST_ASSERT(akq[x] != 'q');

    return complex.coincidenceIndex(y, x);
  }

  int toKingCoeff(Cell x, Cell y, Cell y_star)
  {
    BOOST_ASSERT(akq[x] != 'q');
    BOOST_ASSERT(akq[y_star] == 'q');
    BOOST_ASSERT(akq[y] == 'k');

    return -1 * complex.coincidenceIndex(y_star, x) / complex.coincidenceIndex(y_star, y);
  }

  void followPath(Cell c)
  {
    std::stack<std::pair<Cell, int> > S; // no cycles!
    S.push(std::make_pair(c, 1));    

    while(S.size())
      {
	Cell curr = S.top().first;
	int accumulated_weight = S.top().second;
	S.pop();

	BOOST_ASSERT(akq[curr] != 'q');

	if (curr.getId() != c.getId() && akq[curr] == 'a') {
	  ++num_paths_between[make_pair(c.getId(), curr.getId())];
	  coeffs[make_pair(curr.getId(), c.getId())] += accumulated_weight;
	  continue;
	}

	int our_value = morse[curr.getId()];

	BOOST_FOREACH(Cell to, complex.iterators().cbdCells(curr)) {
	  if (akq[to] == 'a' /*&& morse[to.getId()] > our_value*/) {
	    S.push(make_pair(to, accumulated_weight * toAceCoeff(curr, to)));
	  }
	}

	BOOST_FOREACH(Cell cbd, complex.iterators().cbdCells(curr)){
	  if (akq[cbd] != 'q')
	    continue;
	  Cell to = her_king.find(cbd)->second;
	  BOOST_ASSERT(akq[to] == 'k');
	  if (morse[to.getId()] > our_value) {
	    S.push(make_pair(to, accumulated_weight * toKingCoeff(curr, to, cbd)));
	  }
	}
      }
  }

  void reportPaths()
  {

    BOOST_FOREACH(Cell ace, aces) {
      followPath(ace);
    }

    typedef std::pair<std::pair<int,int>,int> Pair;

    std::vector<typename OutputComplexT::Dim> dims(aces.size());

    std::map<size_t, size_t> from0;

    BOOST_FOREACH(Cell ace, aces) {
      size_t next = from0.size();
      dims[next] = ace.getDim();
      from0[ace.getId()] = next;
    }
    
    typename OutputComplexT::KappaMap kap;

    BOOST_FOREACH(Pair p, coeffs) {
      int coef = p.second;
      kap.push_back(boost::make_tuple(from0[p.first.first], from0[p.first.second], coef));
    }

    outputComplex = new OutputComplexT(3, dims, kap, 1);
  }

protected:
  SComplex& complex;
  Cell dummyCell2, dummyCell3;

  OutputComplexT *outputComplex;
  std::map<int, int> morse;
  std::map<Cell, char> akq;
  std::vector<Cell> aces;
  std::map<std::pair<int,int>, int> coeffs;
  std::map<Cell, Cell> her_king;
  std::map<std::pair<int,int>, int> num_paths_between;
  int maxDim;
  int dimPhase;

};

template<typename SComplexT, typename OutputComplexT>
class COAKQStrategy: public COAKQStrategyBase<SComplexT, OutputComplexT> {

public:
  COAKQStrategy(SComplexT& _complex): COAKQStrategyBase<SComplexT, OutputComplexT>(_complex) {}
};

#endif

