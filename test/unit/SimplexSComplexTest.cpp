#include <iostream>
using namespace std;

#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp> // ?
// #include <boost/test/unit_test.hpp>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <algorithm>
#include <set>
#include <queue>

#include <redHom/RedHomCAPD.h>
#include <redHom/algorithm/Algorithms.hpp>

typedef ElementaryCell ElementaryCellType;
typedef int ScalarType;
typedef FreeModule<int,capd::vectalg::Matrix<int,0,0> > FreeModuleType;
typedef FreeChainComplex<FreeModuleType> FreeChainComplexType;
typedef ReducibleFreeChainComplex<FreeModuleType,int> ReducibleFreeChainComplexType;

typedef ElementaryCell ElementaryCellType;
typedef int ScalarType;
typedef FreeModule<int,capd::vectalg::Matrix<int,0,0> > FreeModuleType;
typedef FreeChainComplex<FreeModuleType> FreeChainComplexType;
typedef ReducibleFreeChainComplex<FreeModuleType,int> ReducibleFreeChainComplexType;

#include "redHom/complex/simplicial/Simplex.hpp"
#include "redHom/complex/simplicial/SimplexSComplex.hpp"
#include "redHom/complex/simplicial/SimplexSubdivision.hpp" // make_int_set

typedef SimplexSComplex::Simplex Simplex;
typedef SimplexSComplex::CellProxy<Simplex*> SimplexCell;

BOOST_AUTO_TEST_SUITE(SimplexSComplex_basic)

BOOST_AUTO_TEST_CASE(add_get_simple_test_2d) {
	SimplexSComplex comp;

	comp.addSimplex(make_int_set(0,1));
	BOOST_CHECK(comp.getSimplex(make_int_set(0,1)) != 0);
	BOOST_CHECK(comp.getSimplex(make_int_set(0)) != 0);
	BOOST_CHECK(comp.getSimplex(make_int_set(1)) != 0);
	BOOST_CHECK_EQUAL(comp.cardinality(), 3);
}

BOOST_AUTO_TEST_CASE(add_get_simple_test_3d) {
	SimplexSComplex comp;

	comp.addSimplex(make_int_set(0,1,2));
	BOOST_CHECK(comp.getSimplex(make_int_set(0,1,2)) != 0);
	BOOST_CHECK(comp.getSimplex(make_int_set(0,1)) != 0);
	BOOST_CHECK(comp.getSimplex(make_int_set(1,2)) != 0);
	BOOST_CHECK(comp.getSimplex(make_int_set(0,2)) != 0);
	BOOST_CHECK(comp.getSimplex(make_int_set(0)) != 0);
	BOOST_CHECK(comp.getSimplex(make_int_set(1)) != 0);
	BOOST_CHECK(comp.getSimplex(make_int_set(2)) != 0);
	BOOST_CHECK_EQUAL(comp.cardinality(), 7);
}

BOOST_AUTO_TEST_CASE(shave_and_coreduce_test) {
	SimplexSComplex comp;

	// An example from: COREDUCTION HOMOLOGY ALGORITHM
	// MARIAN MROZEK AND BOGDAN BATKO

	comp.addSimplex(make_int_set(1,2,3));
	comp.addSimplex(make_int_set(0,1));
	comp.addSimplex(make_int_set(0,3));
	comp.addSimplex(make_int_set(0,4));
	comp.addSimplex(make_int_set(3,4));
	comp.addSimplex(make_int_set(0,5));

	(ShaveAlgorithmFactory::createDefault(comp))();
	BOOST_CHECK_EQUAL(9, comp.cardinality());

	(*CoreductionAlgorithmFactory::createDefault(comp))();
	BOOST_CHECK_EQUAL(2, comp.cardinality());
}

BOOST_AUTO_TEST_CASE(coincidence_index1) {
	SimplexSComplex comp;

    Simplex *a = comp.addSimplex(make_int_set(0,1,2));
    Simplex *b = comp.addSimplex(make_int_set(0,2));

	BOOST_CHECK_EQUAL(-1, comp.coincidenceIndex(SimplexCell(a), SimplexCell(b)));
	BOOST_CHECK_EQUAL(0, comp.coincidenceIndex(SimplexCell(b), SimplexCell(a)));
}

BOOST_AUTO_TEST_CASE(coincidence_index2) {
	SimplexSComplex comp;

    Simplex *a = comp.addSimplex(make_int_set(0,1,2));
    Simplex *b = comp.addSimplex(make_int_set(1,2));

    BOOST_CHECK_EQUAL(1, comp.coincidenceIndex(SimplexCell(a), SimplexCell(b)));
    BOOST_CHECK_EQUAL(0, comp.coincidenceIndex(SimplexCell(b), SimplexCell(a)));
}

BOOST_AUTO_TEST_CASE(coincidence_index3) {
	SimplexSComplex comp;

    Simplex *a = comp.addSimplex(make_int_set(1,2,3));
    Simplex *b = comp.addSimplex(make_int_set(1,3));

	BOOST_CHECK_EQUAL(-1, comp.coincidenceIndex(SimplexCell(a), SimplexCell(b)));
	BOOST_CHECK_EQUAL(0, comp.coincidenceIndex(SimplexCell(b), SimplexCell(a)));
}

BOOST_AUTO_TEST_CASE(coincidence_index4) {
	SimplexSComplex comp;

    Simplex *a = comp.addSimplex(make_int_set(1,2,3));
    Simplex *b = comp.addSimplex(make_int_set(2,3));

    BOOST_CHECK_EQUAL(1, comp.coincidenceIndex(SimplexCell(a), SimplexCell(b)));
    BOOST_CHECK_EQUAL(0, comp.coincidenceIndex(SimplexCell(b), SimplexCell(a)));
}

BOOST_AUTO_TEST_CASE(border_iterator) {
	SimplexSComplex comp;

    Simplex *a = comp.addSimplex(make_int_set(1,2,3));

    typedef SimplexSComplex::ColoredIterators::Iterators::BdCells::iterator BdIt;

    BdIt begin = comp.iterators<1>().bdCells(SimplexCell(a)).begin();
    BdIt end = comp.iterators<1>().bdCells(SimplexCell(a)).end();

    BOOST_CHECK_EQUAL(3, distance(begin, end));
}

BOOST_AUTO_TEST_CASE(border_iterator2) {
	SimplexSComplex comp;

    Simplex *a = comp.addSimplex(make_int_set(1,2,3));
    comp.getSimplex(make_int_set(1,2))->setColor<2>();

    typedef SimplexSComplex::ColoredIterators::Iterators::BdCells::iterator BdIt;

    BdIt begin = comp.iterators<1>().bdCells(SimplexCell(a)).begin();
    BdIt end = comp.iterators<1>().bdCells(SimplexCell(a)).end();

    BOOST_CHECK_EQUAL(2, distance(begin, end));
}


BOOST_AUTO_TEST_SUITE_END()

