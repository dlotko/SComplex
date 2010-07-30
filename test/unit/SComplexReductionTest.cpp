#include <iostream>

#include <redHom/complex/scomplex/SComplex.hpp>
#include <redHom/complex/scomplex/SComplexReader.hpp>
#include <redHom/complex/scomplex/SComplexDefaultTraits.hpp>
#include <redHom/complex/scomplex/SComplexBuilderFromSimplices.hpp>
#include <redHom/algorithm/Algorithms.hpp>

#include <redHom/complex/simplicial/SimplexSubdivision.hpp>


#include <boost/test/unit_test.hpp>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/assign/list_inserter.hpp>


#include <algorithm>


using namespace boost;
using namespace boost::assign;


typedef ElementaryCell ElementaryCellType;
typedef int ScalarType;
typedef FreeModule<int,capd::vectalg::Matrix<int,0,0> > FreeModuleType;
typedef FreeChainComplex<FreeModuleType> FreeChainComplexType;
typedef ReducibleFreeChainComplex<FreeModuleType,int> ReducibleFreeChainComplexType;

BOOST_AUTO_TEST_SUITE(SComplexSuite)


BOOST_AUTO_TEST_CASE(coreduction_line) {
  typedef SComplex<SComplexDefaultTraits> Complex;
  
  Complex::Dims dims = list_of(0)(1)(0)(1)(0);
  Complex::KappaMap kappaMap = tuple_list_of(1, 0, 1)(1, 2, 1)(3, 2, 1)(3, 4, 1); // .-.-.
  
  Complex complex(3, dims, kappaMap, 1);

  (*CoreductionAlgorithmFactory::createDefault(complex))();

  BOOST_FOREACH(Complex::Iterators::AllCells::iterator::value_type v,
					 complex.iterators().allCells()) {
	 BOOST_CHECK_EQUAL(v.getColor(), (Complex::Color)2);
  }

  BOOST_CHECK(complex.iterators(1).allCells().begin() == complex.iterators(1).allCells().end());  
}

BOOST_AUTO_TEST_CASE(coreduction_emptyTriangle) {
  typedef SComplex<SComplexDefaultTraits> Complex;
  
  Complex::Dims dims = list_of(0)(1)(0)(1)(0)(1);
  Complex::KappaMap kappaMap = tuple_list_of(1, 0, 1)(1, 2, 1)(3, 2, 1)(3, 4, 1)(5, 4, 1)(5, 0, 1);
  
  Complex complex(3, dims, kappaMap, 1);

  (*CoreductionAlgorithmFactory::createDefault(complex))();

  BOOST_CHECK( ++(complex.iterators(1).dimCells((Complex::Dim)1).begin()) == complex.iterators(1).dimCells((Complex::Dim)1).end());
  BOOST_CHECK( complex.iterators(1).dimCells((Complex::Dim)0).begin() == complex.iterators(1).dimCells((Complex::Dim)0).end());
}

BOOST_AUTO_TEST_CASE(coreduction_fullTriangle) {
  typedef SComplex<SComplexDefaultTraits> Complex;
  
  Complex::Dims dims = list_of(0)(1)(0)(1)(0)(1)(2);
  Complex::KappaMap kappaMap = tuple_list_of(1, 0, 1)(1, 2, 1)(3, 2, 1)(3, 4, 1)(5, 4, 1)(5, 0, 1)
	 (6, 1, 1)(6, 3, 1)(6, 5, 1); 
  
  Complex complex(3, dims, kappaMap, 1);

  (*CoreductionAlgorithmFactory::createDefault(complex))();

  BOOST_FOREACH(Complex::Iterators::AllCells::iterator::value_type v,
					 complex.iterators().allCells()) {
	 BOOST_CHECK_EQUAL(v.getColor(), (Complex::Color)2);
  }

  BOOST_CHECK(complex.iterators(1).allCells().begin() == complex.iterators(1).allCells().end());  
}

BOOST_AUTO_TEST_CASE(coreduction_simplicialFullTriangle) {
  typedef SComplex<SComplexDefaultTraits> Complex;
  
  SComplexBuilderFromSimplices<long, SComplexDefaultTraits> builder(3);
  std::set<std::vector<int> > simplices;

  insert(simplices)( list_of(0)(1)(2) );

	 
  boost::shared_ptr<Complex> complex = builder(simplices, 3, 1);

  BOOST_CHECK_EQUAL(complex->size(), (size_t)7);

  (*CoreductionAlgorithmFactory::createDefault(*complex))();

  BOOST_FOREACH(Complex::Iterators::AllCells::iterator::value_type v,
  					 complex->iterators().allCells()) {
  	 BOOST_CHECK_EQUAL(v.getColor(), (Complex::Color)2);
  }

  BOOST_CHECK(complex->iterators(1).allCells().begin() == complex->iterators(1).allCells().end());  
}

template<typename TraitsT>
std::string reduction(SComplex<TraitsT>& complex) {
  (*CoreductionAlgorithmFactory::createDefault(complex))();
  
  CRef<ReducibleFreeChainComplexType> RFCComplexCR=
  	 (ReducibleFreeChainComplexOverZFromSComplexAlgorithm<SComplex<TraitsT>, ReducibleFreeChainComplexType>(complex))();

  //std::cerr << RFCComplexCR() << std::endl;

  BOOST_MESSAGE("Computing homology signature");
  CRef<HomologySignature<int> > homSignCR=HomAlgFunctors<FreeModuleType>::homSignViaAR_Random(RFCComplexCR);

  std::ostringstream signature;
  signature << homSignCR();

  std::string sig = signature.str();
  std::replace(sig.begin(), sig.end(), '\n', '#');

  return sig;
}

template<typename T>
std::string reduction(const T& simplices) {
  typedef SComplex<SComplexDefaultTraits> Complex;  
  SComplexBuilderFromSimplices<long, SComplexDefaultTraits> builder(300);

  boost::shared_ptr<Complex> complex = builder(simplices, 3, 1);
  
  return reduction(*complex);
}

BOOST_AUTO_TEST_CASE(coreduction_simplicialEmptyTetrahedron) {
  typedef SComplex<SComplexDefaultTraits> Complex;  
  SComplexBuilderFromSimplices<long, SComplexDefaultTraits> builder(3);
  std::set<std::vector<int> > simplices;

  insert(simplices)( list_of(0)(1)(2)(3) );
	 
  boost::shared_ptr<Complex> complex = builder(simplices, 3, 1);
  complex->iterators().dimCells(3).begin()->setColor(0);
  
  BOOST_CHECK_EQUAL(complex->size(), (size_t) 15);
  BOOST_CHECK(complex->iterators(1).dimCells(3).begin() == complex->iterators(1).dimCells(3).end());
  BOOST_CHECK_EQUAL(reduction(*complex), "  H_0 = 0#  H_1 = 0#  H_2 = Z#");
}

BOOST_AUTO_TEST_CASE(coreduction_simplicialTorus) {
  typedef SComplex<SComplexDefaultTraits> Complex;
  
  SComplexBuilderFromSimplices<long, SComplexDefaultTraits> builder(3);
  std::vector<std::set<int> > simplices = makeSpaceFromWelds(makeTorusWelds());

  for (int i = 0; i < 3; i++) {
  	 simplices = subdivide3(simplices);
  }

  BOOST_CHECK_EQUAL(reduction(simplices), "  H_0 = 0#  H_1 = Z^2#  H_2 = Z#");
}

BOOST_AUTO_TEST_CASE(coreduction_simplicialKlein) {
  typedef SComplex<SComplexDefaultTraits> Complex;
  
  std::vector<std::set<int> > simplices = makeSpaceFromWelds(makeKleinWelds());

  BOOST_CHECK_EQUAL(reduction(simplices), "  H_0 = 0#  H_1 = Z + Z/2#");
}


BOOST_AUTO_TEST_CASE(cubicalSets) {
  typedef SComplexReader<SComplexDefaultTraits> Reader;
  typedef Reader::Complex Complex;
  Reader reader;

  BOOST_CHECK_EQUAL(reduction(*reader(PROJECT_SOURCE_DIR"data/cubical/rectangle.cub", 3, 1)), "  H_0 = 0#  H_1 = Z#");
  BOOST_CHECK_EQUAL(reduction(*reader(PROJECT_SOURCE_DIR"data/cubical/qtorus.cub", 3, 1)), "  H_0 = 0#  H_1 = Z^2#  H_2 = Z#");
  BOOST_CHECK_EQUAL(reduction(*reader(PROJECT_SOURCE_DIR"data/cubical/qklein.cub", 3, 1)), "  H_0 = 0#  H_1 = Z + Z/2#");
  BOOST_CHECK_EQUAL(reduction(*reader(PROJECT_SOURCE_DIR"data/cubical/qprojpln.cub", 3, 1)), "  H_0 = 0#  H_1 = Z/2#");
   BOOST_CHECK_EQUAL(reduction(*reader(PROJECT_SOURCE_DIR"data/cubical/kleinbot.cub", 3, 1)), "  H_0 = 0#  H_1 = Z + Z/2#");
  BOOST_CHECK_EQUAL(reduction(*reader(PROJECT_SOURCE_DIR"data/cubical/torus.cub", 3, 1)), "  H_0 = 0#  H_1 = Z^2#  H_2 = Z#");
  //BOOST_CHECK_EQUAL(reduction(*reader(PROJECT_SOURCE_DIR"data/cubical/sparseSet.txt", 3, 1)), "");
}

BOOST_AUTO_TEST_SUITE_END()

