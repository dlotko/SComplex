#define BOOST_TEST_MAIN

#include <capd/auxil/ofstreamcout.h>
ofstreamcout fcout;

#include <redHom/algorithm/Coreduction.hpp>
#include <redHom/algorithm/AKQStrategy.hpp>
#include <redHom/algorithm/Algorithms.hpp>

#include <redHom/complex/scomplex/SComplex.hpp>
#include <redHom/complex/scomplex/SComplexDefaultTraits.hpp>
#include <redHom/complex/scomplex/SComplexBuilderFromSimplices.hpp>

#include <redHom/complex/cubical/CubSComplex.hpp>
#include <redHom/complex/cubical/CubSComplexReader.hpp>


#include <redHom/complex/simplicial/SimplexIO.hpp>
#include <redHom/complex/simplicial/SimplexSubdivision.hpp>

#include <boost/test/included/unit_test.hpp>
#include <boost/bind.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_io.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/assign/list_of.hpp>

#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>

using namespace boost;
using namespace boost::assign;
using namespace std;

typedef ElementaryCell ElementaryCellType;
typedef int ScalarType;
typedef FreeModule<int,capd::vectalg::Matrix<int,0,0> > FreeModuleType;
typedef FreeChainComplex<FreeModuleType> FreeChainComplexType;
typedef ReducibleFreeChainComplex<FreeModuleType,int> ReducibleFreeChainComplexType;

BOOST_AUTO_TEST_SUITE(COAKQSuite)

template<typename Complex>
boost::tuple<std::string, int> performTest(Complex &complex)
{

  CRef<ReducibleFreeChainComplexType> RFCComplexCR_orginal=
  	 (ReducibleFreeChainComplexOverZFromSComplexAlgorithm<Complex, ReducibleFreeChainComplexType>(complex))();
  CRef<HomologySignature<int> > homSignCR_orginal=HomAlgFunctors<FreeModuleType>::homSignViaAR_Random(RFCComplexCR_orginal);
    CoreductionAlgorithm<AKQReduceStrategy<Complex> > algorithm(new AKQReduceStrategy<Complex>(complex));

    algorithm();

    BOOST_FOREACH(typename Complex::Iterators::AllCells::iterator::value_type v,
                  complex.iterators().allCells())
    {
        BOOST_CHECK_EQUAL(v.getColor(), (typename Complex::Color)2);
    }

    BOOST_CHECK(complex.iterators(1).allCells().begin() == complex.iterators(1).allCells().end());

 SComplex<SComplexDefaultTraits>* AKQ = algorithm.getStrategy()->getOutputComplex();

    CRef<ReducibleFreeChainComplexType> RFCComplexCR =
  	 (ReducibleFreeChainComplexOverZFromSComplexAlgorithm<SComplex<SComplexDefaultTraits>, ReducibleFreeChainComplexType>(*AKQ))();
  CRef<HomologySignature<int> > homSignCR=HomAlgFunctors<FreeModuleType>::homSignViaAR_Random(RFCComplexCR);

  std::ostringstream signature;
  signature << "AKQ: " << homSignCR()<< " | org: " << homSignCR_orginal();
  std::string sig = signature.str();
  std::replace(sig.begin(), sig.end(), '\n', '#');

  return boost::make_tuple(sig, AKQ->size());
}

BOOST_AUTO_TEST_CASE(line)
{
    typedef SComplex<SComplexDefaultTraits> Complex;

    Complex::Dims dims = list_of(0)(1)(0)(1)(0);
    Complex::KappaMap kappaMap = tuple_list_of(1, 0, 1)(1, 2, 1)(3, 2, 1)(3, 4, 1); // .-.-.

    Complex complex(3, dims, kappaMap, 1);

    BOOST_CHECK_EQUAL(performTest(complex), boost::make_tuple(std::string("AKQ:   H_0 = Z# | org:   H_0 = Z#"), 1));
}

BOOST_AUTO_TEST_CASE(emptyTriangle)
{
    typedef SComplex<SComplexDefaultTraits> Complex;

    Complex::Dims dims = list_of(0)(1)(0)(1)(0)(1);
    Complex::KappaMap kappaMap = tuple_list_of(1, 0, 1)(1, 2, -1)(3, 2, 1)(3, 4, -1)(5, 4, 1)(5, 0, -1);

    Complex complex(3, dims, kappaMap, 1);
    BOOST_CHECK_EQUAL(performTest(complex), boost::make_tuple(std::string("AKQ:   H_0 = Z#  H_1 = Z# | org:   H_0 = Z#  H_1 = Z#"), 2));
}


BOOST_AUTO_TEST_CASE(fullTriangle)
{
    typedef SComplex<SComplexDefaultTraits> Complex;

    Complex::Dims dims = list_of(0)(1)(0)(1)(0)(1)(2);
    Complex::KappaMap kappaMap = tuple_list_of(1, 0, -1)(1, 2, 1)(3, 2, -1)(3, 4, 1)(5, 4, -1)(5, 0, 1)
                                 (6, 1, -1)(6, 3, 1)(6, 5, -1);

    Complex complex(3, dims, kappaMap, 1);

  BOOST_CHECK_EQUAL(performTest(complex), boost::make_tuple(std::string("AKQ:   H_0 = Z# | org:   H_0 = Z#"), 1));
}


BOOST_AUTO_TEST_CASE(torus)
{
    typedef SComplex<SComplexDefaultTraits> Complex;

    SComplexBuilderFromSimplices<long, SComplexDefaultTraits> builder(3);
    std::vector<std::set<int> > simplices = makeSpaceFromWelds(makeTorusWelds());

    for (int i = 0; i < 3; i++)
    {
        simplices = subdivide3(simplices);
    }

    boost::shared_ptr<Complex> complex = builder(simplices, 3, 1);

  BOOST_CHECK_EQUAL(performTest(*complex), boost::make_tuple(std::string("AKQ:   H_0 = Z#  H_1 = Z^2#  H_2 = Z# | org:   H_0 = Z#  H_1 = Z^2#  H_2 = Z#"), 4));
}

BOOST_AUTO_TEST_CASE(cubical_sets) {

  CubSComplexReader<3> reader3;
  CubSComplexReader<4> reader4;
  CubSComplexReader<5> reader5;
  CubSComplexReader<8> reader8;

  BOOST_CHECK_EQUAL(performTest(*reader5(PROJECT_SOURCE_DIR"data/cubical/qprojpln.cub")), boost::make_tuple(std::string("AKQ:   H_0 = Z#  H_1 = Z/2# | org:   H_0 = Z#  H_1 = Z/2#"), 3));
  //  BOOST_CHECK_EQUAL(performTest(*reader8(PROJECT_SOURCE_DIR"data/cubical/qtorus.cub")), boost::make_tuple(std::string("AKQ:   H_0 = Z#  H_1 = Z^2#  H_2 = Z# | org:   H_0 = Z#  H_1 = Z^2#  H_2 = Z#"), 4));
  //  BOOST_CHECK_EQUAL(performTest(*reader8(PROJECT_SOURCE_DIR"data/cubical/qklein.cub")), boost::make_tuple(std::string(""), -1));
  BOOST_CHECK_EQUAL(performTest(*reader3(PROJECT_SOURCE_DIR"data/cubical/torus.cub")), boost::make_tuple(std::string("AKQ:   H_0 = Z#  H_1 = Z^2#  H_2 = Z# | org:   H_0 = Z#  H_1 = Z^2#  H_2 = Z#"), 4));
  BOOST_CHECK_EQUAL(performTest(*reader4(PROJECT_SOURCE_DIR"data/cubical/kleinbot.cub")), boost::make_tuple(std::string("AKQ:   H_0 = Z#  H_1 = Z + Z/2# | org:   H_0 = Z#  H_1 = Z + Z/2#"), 4));
  //  BOOST_CHECK_EQUAL(performTest(*reader4(PROJECT_SOURCE_DIR"data/cubical/RFCubSet_d4s16f50.cub")), boost::make_tuple(std::string(""), -1));
}





BOOST_AUTO_TEST_SUITE_END()

