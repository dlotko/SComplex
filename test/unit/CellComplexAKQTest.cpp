#define BOOST_TEST_MAIN

#include <redHom/complex/rectCellComplex/cell.hpp>
#include <redHom/complex/rectCellComplex/cellComplex.hpp>

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

#include <boost/foreach.hpp>

#include <boost/mpl/if.hpp>

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

BOOST_AUTO_TEST_SUITE(CellComplexAKQSuite)



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

    signature << "AKQ: " << homSignCR() << " | org: " << homSignCR_orginal();

    cerr << "AKQ: " << homSignCR() << " | org: " << homSignCR_orginal();


    std::string sig = signature.str();
    std::replace(sig.begin(), sig.end(), '\n', '#');

    return boost::make_tuple(sig, AKQ->size());
}



template<typename Complex>
boost::tuple<std::string, int> performTestCR(Complex &complex)
{

  CRef<ReducibleFreeChainComplexType> RFCComplexCR_orginal=
  	 (ReducibleFreeChainComplexOverZFromSComplexAlgorithm<Complex, ReducibleFreeChainComplexType>(complex))();
  CRef<HomologySignature<int> > homSignCR_orginal=HomAlgFunctors<FreeModuleType>::homSignViaAR_Random(RFCComplexCR_orginal);


  RFCComplexCR_orginal();
  std::ostringstream signature;
  signature << homSignCR_orginal();

  std::cout << "\n\n a" << signature.str() << "a\n\n";
  return boost::make_tuple(signature.str(), 0);
}



    struct Transform : public unary_function<cell<double>*, typename cellComplex<double>::Cell>
    {
        typename cellComplex<double>::Cell operator ()(cell<double> *c) const
        {
            return typename cellComplex<double>::Cell(c);
        }
    };



BOOST_AUTO_TEST_CASE(cell1)
{
    //dla 2d
    std::vector< std::pair<double,double> > point1;
    point1.push_back( std::make_pair(0,1) );
    point1.push_back( std::make_pair(0,1) );

    cellComplex<double>* cmplx1 = new cellComplex<double>( point1 );
    (*cmplx1->elemen()[2].begin())->color() = 2;

    BOOST_CHECK_EQUAL(get<0>( performTestCR(*cmplx1) ), std::string("  H_0 = Z\n  H_1 = Z\n"));
    BOOST_CHECK_EQUAL(get<0>( performTest(*cmplx1) ), std::string("AKQ:   H_0 = Z#  H_1 = Z# | org:   H_0 = Z#  H_1 = Z#"));


    //dla 3d
    std::vector< std::pair<double,double> > point2;
    point2.push_back( std::make_pair(0,1) );
    point2.push_back( std::make_pair(0,1) );
    point2.push_back( std::make_pair(0,1) );
    cellComplex<double>* cmplx2= new cellComplex<double>( point2 );
    cell<double>* cube2 = *cmplx2->elemen()[3].begin();
    BOOST_CHECK_EQUAL(get<0>( performTestCR(*cmplx2) ), std::string("  H_0 = Z\n"));
    BOOST_CHECK_EQUAL(get<0>( performTest(*cmplx2) ), std::string("AKQ:   H_0 = Z# | org:   H_0 = Z#"));

    std::vector< std::pair<double,double> > point3;
    point3.push_back( std::make_pair(0,1) );
    point3.push_back( std::make_pair(0,1) );
    point3.push_back( std::make_pair(0,1) );
    cellComplex<double>* cmplx3= new cellComplex<double>( point3 );
    cell<double>* cube3 = *cmplx3->elemen()[3].begin();
    cube3->color() = 2;
    BOOST_CHECK_EQUAL(get<0>( performTestCR(*cmplx3) ), std::string("  H_0 = Z\n  H_1 = 0\n  H_2 = Z\n"));
    BOOST_CHECK_EQUAL(get<0>( performTest(*cmplx3) ), std::string("AKQ:   H_0 = Z#  H_1 = 0#  H_2 = Z# | org:   H_0 = Z#  H_1 = 0#  H_2 = Z#"));



//dla 3d
    std::vector< std::pair<double,double> > point4;
    point4.push_back( std::make_pair(0.5,1) );
    point4.push_back( std::make_pair(0.5,1) );
    point4.push_back( std::make_pair(0.5,1) );
    cellComplex<double>* cmplx4 = new cellComplex<double>( point4 );
    cell<double>* cube4 = *cmplx4->elemen()[3].begin();
    cmplx4->divideTheCubeByGivenFactor(cube4 , 2);
    cube4 = *cmplx4->elemen()[3].begin();
    cube4->color() = 2;
    BOOST_CHECK_EQUAL(get<0>( performTestCR(*cmplx4) ), std::string("  H_0 = Z\n  H_1 = 0\n  H_2 = Z\n"));
    BOOST_CHECK_EQUAL(get<0>( performTest(*cmplx4) ), std::string("AKQ:   H_0 = Z#  H_1 = 0#  H_2 = Z# | org:   H_0 = Z#  H_1 = 0#  H_2 = Z#"));


    //dla 3d
    std::vector< std::pair<double,double> > point5;
    point5.push_back( std::make_pair(0,1) );
    point5.push_back( std::make_pair(0,1) );
    point5.push_back( std::make_pair(0,1) );
    cellComplex<double>* cmplx5 = new cellComplex<double>( point5 );
    cell<double>* cube5 = *cmplx5->elemen()[3].begin();
    cmplx5->divideTheCubeByGivenFactor(cube5 , 8);
    cube5 = *cmplx5->elemen()[3].begin();
    cube5->color() = 2;
    BOOST_CHECK_EQUAL(get<0>( performTestCR(*cmplx5) ), std::string("  H_0 = Z\n  H_1 = 0\n  H_2 = Z\n"));
    BOOST_CHECK_EQUAL(get<0>( performTest(*cmplx5) ), std::string("AKQ:   H_0 = Z#  H_1 = 0#  H_2 = Z# | org:   H_0 = Z#  H_1 = 0#  H_2 = Z#"));


    //dla 3d
    std::vector< std::pair<double,double> > point6;
    point6.push_back( std::make_pair(0,1) );
    point6.push_back( std::make_pair(0,1) );
    point6.push_back( std::make_pair(0,1) );
    cellComplex<double>* cmplx6 = new cellComplex<double>( point6 );
    cell<double>* cube6 = *cmplx6->elemen()[3].begin();
    cmplx6->divideTheCubeByGivenFactor(cube6 , 8);
    std::set< cell<double>* >::iterator it1 = cmplx6->elemen()[3].begin();
    cell<double>* cube16 = *it1;
    ++it1;
    cell<double>* cube26 = *it1;
    ++it1;
    cell<double>* cube36 = *it1;
    cube16->color() = 2;
    cube26->color() = 2;
    cube36->color() = 2;
    BOOST_CHECK_EQUAL(get<0>( performTestCR(*cmplx6) ), std::string("  H_0 = Z\n  H_1 = 0\n  H_2 = Z^3\n"));
    BOOST_CHECK_EQUAL(get<0>( performTest(*cmplx6) ), std::string("AKQ:   H_0 = Z#  H_1 = 0#  H_2 = Z^3# | org:   H_0 = Z#  H_1 = 0#  H_2 = Z^3#"));


    //dla 4d
    std::vector< std::pair<double,double> > point7;
    point7.push_back( std::make_pair(0,1) );
    point7.push_back( std::make_pair(0,1) );
    point7.push_back( std::make_pair(0,1) );
    point7.push_back( std::make_pair(0,1) );
    cellComplex<double>* cmplx7 = new cellComplex<double>( point7 );
    BOOST_CHECK_EQUAL(get<0>( performTestCR(*cmplx7) ), std::string("  H_0 = Z\n"));
    BOOST_CHECK_EQUAL(get<0>( performTest(*cmplx7) ), std::string("AKQ:   H_0 = Z# | org:   H_0 = Z#"));


    //dla 4d
    std::vector< std::pair<double,double> > point8;
    point8.push_back( std::make_pair(0,1) );
    point8.push_back( std::make_pair(0,1) );
    point8.push_back( std::make_pair(0,1) );
    point8.push_back( std::make_pair(0,1) );
    cellComplex<double>* cmplx8 = new cellComplex<double>( point8 );
    cell<double>* cube8 = *cmplx8->elemen()[4].begin();
    cube8->color() = 2;
    BOOST_CHECK_EQUAL(get<0>( performTestCR(*cmplx8) ), std::string("  H_0 = Z\n  H_1 = 0\n  H_2 = 0\n  H_3 = Z\n"));
    BOOST_CHECK_EQUAL(get<0>( performTest(*cmplx8) ), std::string("AKQ:   H_0 = Z#  H_1 = 0#  H_2 = 0#  H_3 = Z# | org:   H_0 = Z#  H_1 = 0#  H_2 = 0#  H_3 = Z#"));


    //dla 4d
    std::vector< std::pair<double,double> > point9;
    point9.push_back( std::make_pair(0,1) );
    point9.push_back( std::make_pair(0,1) );
    point9.push_back( std::make_pair(0,1) );
    point9.push_back( std::make_pair(0,1) );
    cellComplex<double>* cmplx9 = new cellComplex<double>( point9 );
    cell<double>* cube9 = *cmplx9->elemen()[4].begin();
    cmplx9->divideTheCubeByGivenFactor(cube9 , 3);
    cube9 = *cmplx9->elemen()[4].begin();
    cube9->color() = 2;
    BOOST_CHECK_EQUAL(get<0>( performTestCR(*cmplx9) ), std::string("  H_0 = Z\n  H_1 = 0\n  H_2 = 0\n  H_3 = Z\n"));
    BOOST_CHECK_EQUAL(get<0>( performTest(*cmplx9) ), std::string("AKQ:   H_0 = Z#  H_1 = 0#  H_2 = 0#  H_3 = Z# | org:   H_0 = Z#  H_1 = 0#  H_2 = 0#  H_3 = Z#"));



    //dla 4d
    std::vector< std::pair<double,double> > point10;
    point10.push_back( std::make_pair(0,1) );
    point10.push_back( std::make_pair(0,1) );
    point10.push_back( std::make_pair(0,1) );
    point10.push_back( std::make_pair(0,1) );
    cellComplex<double>* cmplx10 = new cellComplex<double>( point10 );
    cell<double>* cube10 = *cmplx10->elemen()[4].begin();
    cmplx10->divideTheCubeByGivenFactor(cube10 , 8);
    std::set< cell<double>* >::iterator it110 = cmplx10->elemen()[4].begin();
    cell<double>* cube110 = *it110;
    ++it110;
    cell<double>* cube210 = *it110;
    ++it110;
    cell<double>* cube310 = *it110;
    cube110->color() = 2;
    cube210->color() = 2;
    cube310->color() = 2;
    BOOST_CHECK_EQUAL(get<0>( performTestCR(*cmplx10) ), std::string("  H_0 = Z\n  H_1 = 0\n  H_2 = 0\n  H_3 = Z^3\n"));
    BOOST_CHECK_EQUAL(get<0>( performTest(*cmplx10) ), std::string("AKQ:   H_0 = Z#  H_1 = 0#  H_2 = 0#  H_3 = Z^3# | org:   H_0 = Z#  H_1 = 0#  H_2 = 0#  H_3 = Z^3#"));


    //dla 5
    std::vector< std::pair<double,double> > point11;
    point11.push_back( std::make_pair(0,1) );
    point11.push_back( std::make_pair(0,1) );
    point11.push_back( std::make_pair(0,1) );
    point11.push_back( std::make_pair(0,1) );
    point11.push_back( std::make_pair(0,1) );
    cellComplex<double>* cmplx11 = new cellComplex<double>( point11 );
    BOOST_CHECK_EQUAL(get<0>( performTestCR(*cmplx11) ), std::string("  H_0 = Z\n"));
    BOOST_CHECK_EQUAL(get<0>( performTest(*cmplx11) ), std::string("AKQ:   H_0 = Z# | org:   H_0 = Z#"));


    //dla 5d
    std::vector< std::pair<double,double> > point12;
    point12.push_back( std::make_pair(0,1) );
    point12.push_back( std::make_pair(0,1) );
    point12.push_back( std::make_pair(0,1) );
    point12.push_back( std::make_pair(0,1) );
    point12.push_back( std::make_pair(0,1) );
    cellComplex<double>* cmplx12 = new cellComplex<double>( point12 );
    cell<double>* cube12 = *cmplx12->elemen()[5].begin();
    cube12->color() = 2;
    BOOST_CHECK_EQUAL(get<0>( performTestCR(*cmplx12) ), std::string("  H_0 = Z\n  H_1 = 0\n  H_2 = 0\n  H_3 = 0\n  H_4 = Z\n"));
    BOOST_CHECK_EQUAL(get<0>( performTest(*cmplx12) ), std::string("AKQ:   H_0 = Z#  H_1 = 0#  H_2 = 0#  H_3 = 0#  H_4 = Z# | org:   H_0 = Z#  H_1 = 0#  H_2 = 0#  H_3 = 0#  H_4 = Z#"));


    //dla 5d
    std::vector< std::pair<double,double> > point13;
    point13.push_back( std::make_pair(0,1) );
    point13.push_back( std::make_pair(0,1) );
    point13.push_back( std::make_pair(0,1) );
    point13.push_back( std::make_pair(0,1) );
    point13.push_back( std::make_pair(0,1) );
    cellComplex<double>* cmplx13 = new cellComplex<double>( point13 );
    cell<double>* cube13 = *cmplx13->elemen()[5].begin();
    cmplx13->divideTheCubeByGivenFactor(cube13 , 3);
    cube13 = *cmplx13->elemen()[5].begin();
    cube13->color() = 2;
    BOOST_CHECK_EQUAL(get<0>( performTestCR(*cmplx13) ), std::string("  H_0 = Z\n  H_1 = 0\n  H_2 = 0\n  H_3 = 0\n  H_4 = Z\n"));
    BOOST_CHECK_EQUAL(get<0>( performTest(*cmplx13) ), std::string("AKQ:   H_0 = Z#  H_1 = 0#  H_2 = 0#  H_3 = 0#  H_4 = Z# | org:   H_0 = Z#  H_1 = 0#  H_2 = 0#  H_3 = 0#  H_4 = Z#"));



    //dla 5d
    std::vector< std::pair<double,double> > point14;
    point14.push_back( std::make_pair(0,1) );
    point14.push_back( std::make_pair(0,1) );
    point14.push_back( std::make_pair(0,1) );
    point14.push_back( std::make_pair(0,1) );
    point14.push_back( std::make_pair(0,1) );
    cellComplex<double>* cmplx14 = new cellComplex<double>( point14 );
    cell<double>* cube14 = *cmplx14->elemen()[5].begin();
    cmplx14->divideTheCubeByGivenFactor(cube14 , 3);
    std::set< cell<double>* >::iterator it114 = cmplx14->elemen()[5].begin();
    cell<double>* cube114 = *it114;
    ++it114;
    cell<double>* cube214 = *it114;
    ++it114;
    cell<double>* cube314 = *it114;
    cube114->color() = 2;
    cube214->color() = 2;
    cube314->color() = 2;
    BOOST_CHECK_EQUAL(get<0>( performTestCR(*cmplx14) ), std::string("  H_0 = Z\n  H_1 = 0\n  H_2 = 0\n  H_3 = 0\n  H_4 = Z^3\n"));
    BOOST_CHECK_EQUAL(get<0>( performTest(*cmplx14) ), std::string("AKQ:   H_0 = Z#  H_1 = 0#  H_2 = 0#  H_3 = 0#  H_4 = Z^3# | org:   H_0 = Z#  H_1 = 0#  H_2 = 0#  H_3 = 0#  H_4 = Z^3#"));

}

BOOST_AUTO_TEST_SUITE_END()
