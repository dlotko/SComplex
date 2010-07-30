#include <iostream>
using namespace std;

#include <redHom/complex/scomplex/SComplex.hpp>
#include <redHom/complex/scomplex/SComplexDefaultTraits.hpp>


#include <boost/test/unit_test.hpp>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/assign/list_of.hpp>

#include <algorithm>

using namespace boost;
using namespace boost::assign;

BOOST_AUTO_TEST_SUITE(SComplexSuite)


BOOST_AUTO_TEST_CASE(setColor) {
  typedef SComplex<SComplexDefaultTraits> Complex;
  const int size = 300;
  const int colors = size;
  Complex::Dims dims(size);

  for (int i = 0; i < size; ++i) {
	 dims[i] = i;
  }

  Complex complex(colors, dims);

  {
	 for (int i = 0; i < size; ++i) {
		complex[i].setColor(i);
	 }

	 for (int i = 0; i < size; ++i) {
		BOOST_CHECK_EQUAL(complex[i].getColor(), (Complex::Color)i);
	 }

	 int i = 0;
	 BOOST_FOREACH(Complex::Iterators::AllCells::iterator::value_type v,
						complex.iterators().allCells()) {
		BOOST_CHECK_EQUAL(v.getColor(), (Complex::Color) i);
		++i;
	 }
  
	 for (int i = 0; i < size; ++i) {
		BOOST_CHECK_EQUAL( complex.iterators((Complex::Color)i).allCells().begin()->getId(), (Complex::Id) i);
		BOOST_CHECK_EQUAL( complex.iterators((Complex::Color)i).dimCells((Complex::Dim)i).begin()->getId(), (Complex::Id) i);
	 }
  }

  {
	 for (int i = 0; i < size; ++i) {
		complex[i].setColor(size - i - 1);
	 }

	 for (int i = 0; i < size; ++i) {
		BOOST_CHECK_EQUAL(complex[i].getColor(), (Complex::Color)(size - i - 1));
	 }

	 int i = size - 1;
	 BOOST_FOREACH(Complex::Iterators::AllCells::iterator::value_type v,
						complex.iterators().allCells()) {
		BOOST_CHECK_EQUAL(v.getColor(), (Complex::Color) i);
		--i;
	 }
  
	 for (int i = 0; i < size; ++i) {
		BOOST_CHECK_EQUAL( complex.iterators((Complex::Color)i).allCells().begin()->getId(), (Complex::Id) (size - i - 1));
		BOOST_CHECK_EQUAL( complex.iterators((Complex::Color)i).dimCells((Complex::Dim)(size - i - 1)).begin()->getId(), (Complex::Id) (size - i - 1));
	 }
  }
}

BOOST_AUTO_TEST_CASE(setColorCheckNeighbours) {
  typedef SComplex<SComplexDefaultTraits> Complex;
  const int size = 4;
  const int colors = size;

  Complex::KappaMap kappaMap = tuple_list_of(0, 1, 1)(0, 2, 1)(1, 2, 1)(0, 3, 1);
  Complex::Dims dims(size);
  std::vector<int> bdSizes = list_of(3)(1)(0)(0);
  
  Complex complex(colors, dims, kappaMap);
  Complex::Cell cells[size] = {complex[0], complex[1], complex[2], complex[3]};
  
  for (int i = 0; i < size; ++i) {
	 cells[i].setColor(i);
  }

  BOOST_CHECK_EQUAL(complex.iterators((Complex::Color)1).bdCells(complex[0]).begin()->getId(), (Complex::Id) 1);
  BOOST_CHECK_EQUAL(complex.iterators((Complex::Color)2).bdCells(complex[0]).begin()->getId(), (Complex::Id) 2);
  BOOST_CHECK_EQUAL(complex.iterators((Complex::Color)3).bdCells(complex[0]).begin()->getId(), (Complex::Id) 3);
  BOOST_CHECK_EQUAL(complex.iterators((Complex::Color)2).bdCells(complex[1]).begin()->getId(), (Complex::Id) 2);

  BOOST_CHECK_EQUAL(complex.iterators((Complex::Color)0).cbdCells(complex[1]).begin()->getId(), (Complex::Id) 0);
  BOOST_CHECK_EQUAL(complex.iterators((Complex::Color)0).cbdCells(complex[2]).begin()->getId(), (Complex::Id) 0);
  BOOST_CHECK_EQUAL(complex.iterators((Complex::Color)0).cbdCells(complex[3]).begin()->getId(), (Complex::Id) 0);

  BOOST_CHECK_EQUAL(complex.iterators((Complex::Color)1).cbdCells(complex[2]).begin()->getId(), (Complex::Id) 1);
}

BOOST_AUTO_TEST_SUITE_END()
