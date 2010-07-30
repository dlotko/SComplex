#include <boost/test/unit_test.hpp>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <algorithm>

#include <iostream>

#include <redHom/complex/cubical/CubSComplex.hpp>
#include <redHom/complex/cubical/CubSComplexReader.hpp>

using namespace std;

BOOST_AUTO_TEST_SUITE(CubSComplex_iterators)


boost::shared_ptr<CubSComplex<3> > createComplex() {
  CubSComplexReader<3> reader;
  
  return reader(PROJECT_SOURCE_DIR"data/cubical/torus.cub");
}

  
BOOST_AUTO_TEST_CASE(iterators_AllCells_init_test) {
	 boost::shared_ptr<CubSComplex<3> > s = createComplex();
	 
	 BOOST_CHECK(s->iterators(1).allCells().begin() != s->iterators(1).allCells().end());
	 BOOST_CHECK(s->iterators(1).allCells().begin() == s->iterators(1).allCells().begin());
	 BOOST_CHECK(s->iterators(1).allCells().end() == s->iterators(1).allCells().end());
	 BOOST_CHECK( s->iterators(1).allCells().begin() != s->iterators(1).allCells().end());
	 BOOST_CHECK( s->iterators(1).allCells().begin() == s->iterators(1).allCells().begin());
	 BOOST_CHECK( s->iterators(1).allCells().end() == s->iterators(1).allCells().end());
}

BOOST_AUTO_TEST_CASE(iterators_DimCells_init_test) {
  boost::shared_ptr<CubSComplex<3> > s = createComplex();

	 BOOST_CHECK(s->iterators(1).dimCells(0).begin() != s->iterators(1).dimCells(0).end());
	 BOOST_CHECK(s->iterators(1).dimCells(0).begin() == s->iterators(1).dimCells(0).begin());
	 BOOST_CHECK(s->iterators(1).dimCells(0).end() == s->iterators(1).dimCells(0).end());
	 BOOST_CHECK( s->iterators(1).dimCells(0).begin() != s->iterators(1).dimCells(0).end());
	 BOOST_CHECK( s->iterators(1).dimCells(0).begin() == s->iterators(1).dimCells(0).begin());
	 BOOST_CHECK( s->iterators(1).dimCells(0).end() == s->iterators(1).dimCells(0).end());
}

BOOST_AUTO_TEST_CASE(iterators_AllCells_forEach_test) {
  typedef CubSComplex<3> SComplex;
  boost::shared_ptr<SComplex> s = createComplex();
	 const int initSize = s->cardinality();

	 BOOST_CHECK_EQUAL(s->cardinality(), 1536);

	 int cnt = 0;
	 BOOST_FOREACH(SComplex::Iterators::AllCells::iterator::value_type v, s->iterators(1).allCells()) {
		if (v.getColor() == 1) {
		  ++cnt;
		}
	 }
	 BOOST_CHECK_EQUAL(cnt, initSize);
	 
// 	 SComplex::Iterators::AllCells allCells = s->iterators(1).allCells();
// 	 std::for_each(allCells.begin(), allCells.end(), boost::bind(&SComplex::Cell::setColor, _1, 2));
// 	 BOOST_CHECK_EQUAL(s->cardinality(), 0);

// 	 cnt = 0;
// 	 std::for_each(s->iterators(1).allCells().begin(), s->iterators(1).allCells().end(), boost::lambda::var(cnt)++);
	 //	 BOOST_CHECK_EQUAL(cnt, initSize);	 
}


BOOST_AUTO_TEST_CASE(iterators_BdCells_forEach_test) {
  typedef CubSComplex<3> SComplex;
  boost::shared_ptr<SComplex> s = createComplex();

  
  SComplex::Iterators::DimCells::iterator::value_type dim0Cell = *(s->iterators(1).dimCells(0).begin());
  int dim0CellBdSize = 0;

  BOOST_FOREACH(SComplex::Iterators::BdCells::iterator::value_type v, s->iterators(1).bdCells(dim0Cell)) {
    dim0CellBdSize++;
  }
  BOOST_CHECK_EQUAL(dim0CellBdSize, 0);

  SComplex::Iterators::DimCells::iterator::value_type dim1Cell = *(s->iterators(1).dimCells(1).begin());
  int dim1CellBdSize = 0;

  BOOST_FOREACH(SComplex::Iterators::BdCells::iterator::value_type v, s->iterators(1).bdCells(dim1Cell)) {
    dim1CellBdSize++;
  }
  BOOST_CHECK_EQUAL(dim1CellBdSize, 2);


  SComplex::Iterators::DimCells::iterator::value_type dim2Cell = *(s->iterators(1).dimCells(2).begin());
  int dim2CellBdSize = 0;

  BOOST_FOREACH(SComplex::Iterators::BdCells::iterator::value_type v, s->iterators(1).bdCells(dim2Cell)) {
    dim2CellBdSize++;
  }
  BOOST_CHECK_EQUAL(dim2CellBdSize, 4);

}


BOOST_AUTO_TEST_CASE(iterators_CbdCells_forEach_test) {
  typedef CubSComplex<3> SComplex;
  boost::shared_ptr<SComplex> s = createComplex();

  
  SComplex::Iterators::DimCells::iterator::value_type dim0Cell = *(s->iterators(1).dimCells(0).begin());
  int dim0CellCbdSize = 0;

  BOOST_FOREACH(SComplex::Iterators::CbdCells::iterator::value_type v, s->iterators(1).cbdCells(dim0Cell)) {
    dim0CellCbdSize++;
  }
  BOOST_CHECK_EQUAL(dim0CellCbdSize, 3);

  SComplex::Iterators::DimCells::iterator::value_type dim1Cell = *(s->iterators(1).dimCells(1).begin());
  int dim1CellCbdSize = 0;

  BOOST_FOREACH(SComplex::Iterators::CbdCells::iterator::value_type v, s->iterators(1).cbdCells(dim1Cell)) {
    dim1CellCbdSize++;
  }
  BOOST_CHECK_EQUAL(dim1CellCbdSize, 2);


  SComplex::Iterators::DimCells::iterator::value_type dim2Cell = *(s->iterators(1).dimCells(2).begin());
  int dim2CellCbdSize = 0;

  BOOST_FOREACH(SComplex::Iterators::CbdCells::iterator::value_type v, s->iterators(1).cbdCells(dim2Cell)) {
    dim2CellCbdSize++;
  }
  BOOST_CHECK_EQUAL(dim2CellCbdSize, 1);

}

BOOST_AUTO_TEST_SUITE_END()
