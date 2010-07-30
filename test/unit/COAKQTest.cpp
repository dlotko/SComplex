#include <redHom/algorithm/COAKQ.hpp>
#include <redHom/algorithm/Algorithms.hpp>

#include <redHom/complex/scomplex/SComplex.hpp>
#include <redHom/complex/scomplex/SComplexDefaultTraits.hpp>
#include <redHom/complex/scomplex/SComplexBuilderFromSimplices.hpp>

#include <redHom/complex/simplicial/SimplexSubdivision.hpp>
#include <redHom/complex/simplicial/SimplexIO.hpp>


#include <boost/test/unit_test.hpp>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/assign/list_of.hpp>

#include <algorithm>
#include <string>
#include <sstream>
#include<fstream>

using namespace boost;
using namespace boost::assign;
using namespace std;

typedef ElementaryCell ElementaryCellType;
typedef int ScalarType;
typedef FreeModule<int,capd::vectalg::Matrix<int,0,0> > FreeModuleType;
typedef FreeChainComplex<FreeModuleType> FreeChainComplexType;
typedef ReducibleFreeChainComplex<FreeModuleType,int> ReducibleFreeChainComplexType;



// vector<set<int> > parseDat(istream &stream)
// {
  
//   vector<set<int> > simplices;

//   for (string s; std::getline(stream, s); )
//     {
//       if (s.size() == 0 || s[0] == '#')
// 	continue;

//       stringstream ss(s);

//       set<int> simp;

//       for (int v; ss >> v;)
// 	simp.insert(v);

//       simplices.push_back(simp);
//     }

//   return simplices;
// }

BOOST_AUTO_TEST_SUITE(COAKQSuite)


BOOST_AUTO_TEST_CASE(line) {
  typedef SComplex<SComplexDefaultTraits> Complex;

  Complex::Dims dims = list_of(0)(1)(0)(1)(0);
  Complex::KappaMap kappaMap = tuple_list_of(1, 0, 1)(1, 2, 1)(3, 2, 1)(3, 4, 1); // .-.-.
  
  Complex complex(3, dims, kappaMap, 1);

  COAKQAlgorithm<COAKQStrategy<Complex, Complex> > algorithm(new COAKQStrategy<Complex, Complex>(complex));

  algorithm();

  BOOST_FOREACH(Complex::Iterators::AllCells::iterator::value_type v,
					 complex.iterators().allCells()) {
	 BOOST_CHECK_EQUAL(v.getColor(), (Complex::Color)2);
  }

  BOOST_CHECK(complex.iterators(1).allCells().begin() == complex.iterators(1).allCells().end());  

  BOOST_CHECK_EQUAL(algorithm.getStrategy().getOutputComplex().size(), 1);
}


BOOST_AUTO_TEST_CASE(emptyTriangle) {
  typedef SComplex<SComplexDefaultTraits> Complex;
  
  Complex::Dims dims = list_of(0)(1)(0)(1)(0)(1);
  Complex::KappaMap kappaMap = tuple_list_of(1, 0, 1)(1, 2, -1)(3, 2, 1)(3, 4, -1)(5, 4, 1)(5, 0, -1);
  
  Complex complex(3, dims, kappaMap, 1);

  COAKQAlgorithm<COAKQStrategy<Complex, Complex> > algorithm(new COAKQStrategy<Complex, Complex>(complex));

  algorithm();

  BOOST_FOREACH(Complex::Iterators::AllCells::iterator::value_type v,
					 complex.iterators().allCells()) {
	 BOOST_CHECK_EQUAL(v.getColor(), (Complex::Color)2);
  }

  BOOST_CHECK(complex.iterators(1).allCells().begin() == complex.iterators(1).allCells().end());  

  Complex& coAKQ = algorithm.getStrategy().getOutputComplex();
  BOOST_CHECK_EQUAL(coAKQ.size(), 2);

  CRef<ReducibleFreeChainComplexType> RFCComplexCR=
  	 (ReducibleFreeChainComplexOverZFromSComplexAlgorithm<Complex, ReducibleFreeChainComplexType>(coAKQ))();
  CRef<HomologySignature<int> > homSignCR=HomAlgFunctors<FreeModuleType>::homSignViaAR_Random(RFCComplexCR);

  std::string betti = homSignCR().bettiVector();

  BOOST_CHECK_EQUAL(betti, "1,1");

}

BOOST_AUTO_TEST_CASE(fullTriangle) {
  typedef SComplex<SComplexDefaultTraits> Complex;


  Complex::Dims dims = list_of(0)(1)(0)(1)(0)(1)(2);
  Complex::KappaMap kappaMap = tuple_list_of(1, 0, -1)(1, 2, 1)(3, 2, -1)(3, 4, 1)(5, 4, -1)(5, 0, 1)
	 (6, 1, -1)(6, 3, 1)(6, 5, -1); 
  
  Complex complex(3, dims, kappaMap, 1);

  COAKQAlgorithm<COAKQStrategy<Complex, Complex> > algorithm(new COAKQStrategy<Complex, Complex>(complex));

  algorithm();

  BOOST_FOREACH(Complex::Iterators::AllCells::iterator::value_type v,
					 complex.iterators().allCells()) {
	 BOOST_CHECK_EQUAL(v.getColor(), (Complex::Color)2);
  }

  BOOST_CHECK(complex.iterators(1).allCells().begin() == complex.iterators(1).allCells().end());  

  Complex& coAKQ = algorithm.getStrategy().getOutputComplex();
  BOOST_CHECK_EQUAL(coAKQ.size(), 1);

  CRef<ReducibleFreeChainComplexType> RFCComplexCR=
  	 (ReducibleFreeChainComplexOverZFromSComplexAlgorithm<Complex, ReducibleFreeChainComplexType>(coAKQ))();
  CRef<HomologySignature<int> > homSignCR=HomAlgFunctors<FreeModuleType>::homSignViaAR_Random(RFCComplexCR);

  std::string betti = homSignCR().bettiVector();

  BOOST_CHECK_EQUAL(betti, "1");

}


BOOST_AUTO_TEST_CASE(torus) {
  typedef SComplex<SComplexDefaultTraits> Complex;

  SComplexBuilderFromSimplices<long, SComplexDefaultTraits> builder(3);
  std::vector<std::set<int> > simplices = makeSpaceFromWelds(makeTorusWelds());

  for (int i = 0; i < 3; i++) {
  	 simplices = subdivide3(simplices);
  }

  boost::shared_ptr<Complex> complex = builder(simplices, 3, 1);

  CRef<ReducibleFreeChainComplexType> RFCComplexCR_orginal=
  	 (ReducibleFreeChainComplexOverZFromSComplexAlgorithm<Complex, ReducibleFreeChainComplexType>(*complex))();
  CRef<HomologySignature<int> > homSignCR_orginal=HomAlgFunctors<FreeModuleType>::homSignViaAR_Random(RFCComplexCR_orginal);

  COAKQAlgorithm<COAKQStrategy<Complex, Complex> > algorithm(new COAKQStrategy<Complex, Complex>(*complex));

  algorithm();


  Complex& coAKQ = algorithm.getStrategy().getOutputComplex();
  BOOST_CHECK_EQUAL(coAKQ.size(), 4);

  CRef<ReducibleFreeChainComplexType> RFCComplexCR=
  	 (ReducibleFreeChainComplexOverZFromSComplexAlgorithm<Complex, ReducibleFreeChainComplexType>(coAKQ))();
  CRef<HomologySignature<int> > homSignCR=HomAlgFunctors<FreeModuleType>::homSignViaAR_Random(RFCComplexCR);

  std::ostringstream signature;
  signature << "coAKQ: " << homSignCR()<< " | org: " << homSignCR_orginal();
  std::string sig = signature.str();
  std::replace(sig.begin(), sig.end(), '\n', '#');
  BOOST_CHECK_EQUAL(sig, "coAKQ:   H_0 = Z#  H_1 = Z^2#  H_2 = Z# | org:   H_0 = Z#  H_1 = Z^2#  H_2 = Z#");
}

BOOST_AUTO_TEST_CASE(klein) {
  typedef SComplex<SComplexDefaultTraits> Complex;

  SComplexBuilderFromSimplices<long, SComplexDefaultTraits> builder(3);
  std::vector<std::set<int> > simplices = makeSpaceFromWelds(makeKleinWelds());

  for (int i = 0; i < 0; i++) {
    simplices = subdivide3(simplices);
  }

  boost::shared_ptr<Complex> complex = builder(simplices, 3, 1);

  CRef<ReducibleFreeChainComplexType> RFCComplexCR_orginal=
  	 (ReducibleFreeChainComplexOverZFromSComplexAlgorithm<Complex, ReducibleFreeChainComplexType>(*complex))();

  CRef<HomologySignature<int> > homSignCR_orginal=HomAlgFunctors<FreeModuleType>::homSignViaAR_Random(RFCComplexCR_orginal);

  COAKQAlgorithm<COAKQStrategy<Complex, Complex> > algorithm(new COAKQStrategy<Complex, Complex>(*complex));

  algorithm();

  Complex& coAKQ = algorithm.getStrategy().getOutputComplex();
  BOOST_CHECK_EQUAL(coAKQ.size(), 4);

  CRef<ReducibleFreeChainComplexType> RFCComplexCR=
  	 (ReducibleFreeChainComplexOverZFromSComplexAlgorithm<Complex, ReducibleFreeChainComplexType>(coAKQ))();
  CRef<HomologySignature<int> > homSignCR=HomAlgFunctors<FreeModuleType>::homSignViaAR_Random(RFCComplexCR);

  std::ostringstream signature;
  signature << "coAKQ: " << homSignCR()<< " | org: " << homSignCR_orginal();
  std::string sig = signature.str();
  std::replace(sig.begin(), sig.end(), '\n', '#');
  BOOST_CHECK_EQUAL(sig, "coAKQ:   H_0 = Z#  H_1 = Z + Z/2# | org:   H_0 = Z#  H_1 = Z + Z/2#");
}


BOOST_AUTO_TEST_CASE(projective) {
  typedef SComplex<SComplexDefaultTraits> Complex;

  fstream file(PROJECT_SOURCE_DIR"/data/spaces/projective.dat");
  std::vector<std::set<int> > simplices = parseDat(file);

  SComplexBuilderFromSimplices<long, SComplexDefaultTraits> builder(3);
  boost::shared_ptr<Complex> complex = builder(simplices, 3, 1);

  CRef<ReducibleFreeChainComplexType> RFCComplexCR_orginal=
  	 (ReducibleFreeChainComplexOverZFromSComplexAlgorithm<Complex, ReducibleFreeChainComplexType>(*complex))();
  CRef<HomologySignature<int> > homSignCR_orginal=HomAlgFunctors<FreeModuleType>::homSignViaAR_Random(RFCComplexCR_orginal);

  COAKQAlgorithm<COAKQStrategy<Complex, Complex> > algorithm(new COAKQStrategy<Complex, Complex>(*complex));

  algorithm();

  BOOST_FOREACH(Complex::Iterators::AllCells::iterator::value_type v,
					 complex->iterators().allCells()) {
	 BOOST_CHECK_EQUAL(v.getColor(), (Complex::Color)2);
  }

  BOOST_CHECK(complex->iterators(1).allCells().begin() == complex->iterators(1).allCells().end());  

  Complex& coAKQ = algorithm.getStrategy().getOutputComplex();
  BOOST_CHECK_EQUAL(coAKQ.size(), 3);

  CRef<ReducibleFreeChainComplexType> RFCComplexCR=
  	 (ReducibleFreeChainComplexOverZFromSComplexAlgorithm<Complex, ReducibleFreeChainComplexType>(coAKQ))();
  CRef<HomologySignature<int> > homSignCR=HomAlgFunctors<FreeModuleType>::homSignViaAR_Random(RFCComplexCR);

  std::ostringstream signature;
  signature << "coAKQ: " << homSignCR()<< " | org: " << homSignCR_orginal();
  std::string sig = signature.str();
  std::replace(sig.begin(), sig.end(), '\n', '#');
  BOOST_CHECK_EQUAL(sig, "coAKQ:   H_0 = Z#  H_1 = Z/2# | org:   H_0 = Z#  H_1 = Z/2#");
}


BOOST_AUTO_TEST_CASE(bing) {
  typedef SComplex<SComplexDefaultTraits> Complex;

  fstream file(PROJECT_SOURCE_DIR"/data/spaces/bing.dat");
  std::vector<std::set<int> > simplices = parseDat(file);

  SComplexBuilderFromSimplices<long, SComplexDefaultTraits> builder(3);
  boost::shared_ptr<Complex> complex = builder(simplices, 3, 1);

  CRef<ReducibleFreeChainComplexType> RFCComplexCR_orginal=
  	 (ReducibleFreeChainComplexOverZFromSComplexAlgorithm<Complex, ReducibleFreeChainComplexType>(*complex))();
  CRef<HomologySignature<int> > homSignCR_orginal=HomAlgFunctors<FreeModuleType>::homSignViaAR_Random(RFCComplexCR_orginal);

  COAKQAlgorithm<COAKQStrategy<Complex, Complex> > algorithm(new COAKQStrategy<Complex, Complex>(*complex));

  algorithm();

  BOOST_FOREACH(Complex::Iterators::AllCells::iterator::value_type v,
					 complex->iterators().allCells()) {
	 BOOST_CHECK_EQUAL(v.getColor(), (Complex::Color)2);
  }

  BOOST_CHECK(complex->iterators(1).allCells().begin() == complex->iterators(1).allCells().end());  

  Complex& coAKQ = algorithm.getStrategy().getOutputComplex();
  BOOST_CHECK_EQUAL(coAKQ.size(), 1);

  CRef<ReducibleFreeChainComplexType> RFCComplexCR=
  	 (ReducibleFreeChainComplexOverZFromSComplexAlgorithm<Complex, ReducibleFreeChainComplexType>(coAKQ))();
  CRef<HomologySignature<int> > homSignCR=HomAlgFunctors<FreeModuleType>::homSignViaAR_Random(RFCComplexCR);

  std::ostringstream signature;
  signature << "coAKQ: " << homSignCR()<< " | org: " << homSignCR_orginal();
  std::string sig = signature.str();
  std::replace(sig.begin(), sig.end(), '\n', '#');
  BOOST_CHECK_EQUAL(sig, "coAKQ:   H_0 = Z# | org:   H_0 = Z#");
}


BOOST_AUTO_TEST_CASE(dunce_hat) {
  typedef SComplex<SComplexDefaultTraits> Complex;

  fstream file(PROJECT_SOURCE_DIR"/data/spaces/dunce_hat.dat");
  std::vector<std::set<int> > simplices = parseDat(file);

  SComplexBuilderFromSimplices<long, SComplexDefaultTraits> builder(3);
  boost::shared_ptr<Complex> complex = builder(simplices, 3, 1);

  CRef<ReducibleFreeChainComplexType> RFCComplexCR_orginal=
  	 (ReducibleFreeChainComplexOverZFromSComplexAlgorithm<Complex, ReducibleFreeChainComplexType>(*complex))();
  CRef<HomologySignature<int> > homSignCR_orginal=HomAlgFunctors<FreeModuleType>::homSignViaAR_Random(RFCComplexCR_orginal);

  COAKQAlgorithm<COAKQStrategy<Complex, Complex> > algorithm(new COAKQStrategy<Complex, Complex>(*complex));

  algorithm();

  BOOST_FOREACH(Complex::Iterators::AllCells::iterator::value_type v,
					 complex->iterators().allCells()) {
	 BOOST_CHECK_EQUAL(v.getColor(), (Complex::Color)2);
  }

  BOOST_CHECK(complex->iterators(1).allCells().begin() == complex->iterators(1).allCells().end());  

  Complex& coAKQ = algorithm.getStrategy().getOutputComplex();
  BOOST_CHECK_EQUAL(coAKQ.size(), 3);

  CRef<ReducibleFreeChainComplexType> RFCComplexCR=
  	 (ReducibleFreeChainComplexOverZFromSComplexAlgorithm<Complex, ReducibleFreeChainComplexType>(coAKQ))();
  CRef<HomologySignature<int> > homSignCR=HomAlgFunctors<FreeModuleType>::homSignViaAR_Random(RFCComplexCR);

  std::ostringstream signature;
  signature << "coAKQ: " << homSignCR()<< " | org: " << homSignCR_orginal();
  std::string sig = signature.str();
  std::replace(sig.begin(), sig.end(), '\n', '#');
  BOOST_CHECK_EQUAL(sig, "coAKQ:   H_0 = Z# | org:   H_0 = Z#");
}


BOOST_AUTO_TEST_CASE(nc_sphere) {
  typedef SComplex<SComplexDefaultTraits> Complex;

  fstream file(PROJECT_SOURCE_DIR"/data/spaces/nc_sphere.dat");
  std::vector<std::set<int> > simplices = parseDat(file);

  SComplexBuilderFromSimplices<long, SComplexDefaultTraits> builder(3);
  boost::shared_ptr<Complex> complex = builder(simplices, 3, 1);

  CRef<ReducibleFreeChainComplexType> RFCComplexCR_orginal=
  	 (ReducibleFreeChainComplexOverZFromSComplexAlgorithm<Complex, ReducibleFreeChainComplexType>(*complex))();
  CRef<HomologySignature<int> > homSignCR_orginal=HomAlgFunctors<FreeModuleType>::homSignViaAR_Random(RFCComplexCR_orginal);

  COAKQAlgorithm<COAKQStrategy<Complex, Complex> > algorithm(new COAKQStrategy<Complex, Complex>(*complex));

  algorithm();

  BOOST_FOREACH(Complex::Iterators::AllCells::iterator::value_type v,
					 complex->iterators().allCells()) {
	 BOOST_CHECK_EQUAL(v.getColor(), (Complex::Color)2);
  }

  BOOST_CHECK(complex->iterators(1).allCells().begin() == complex->iterators(1).allCells().end());  

  Complex& coAKQ = algorithm.getStrategy().getOutputComplex();
  BOOST_CHECK_EQUAL(coAKQ.size(), 4);

  CRef<ReducibleFreeChainComplexType> RFCComplexCR=
  	 (ReducibleFreeChainComplexOverZFromSComplexAlgorithm<Complex, ReducibleFreeChainComplexType>(coAKQ))();
  CRef<HomologySignature<int> > homSignCR=HomAlgFunctors<FreeModuleType>::homSignViaAR_Random(RFCComplexCR);

  std::ostringstream signature;
  signature << "coAKQ: " << homSignCR()<< " | org: " << homSignCR_orginal();
  std::string sig = signature.str();
  std::replace(sig.begin(), sig.end(), '\n', '#');
  BOOST_CHECK_EQUAL(sig, "coAKQ:   H_0 = Z#  H_1 = 0#  H_2 = 0#  H_3 = Z# | org:   H_0 = Z#  H_1 = 0#  H_2 = 0#  H_3 = Z#");
}



BOOST_AUTO_TEST_CASE(poincare) {
  typedef SComplex<SComplexDefaultTraits> Complex;

  fstream file(PROJECT_SOURCE_DIR"/data/spaces/poincare.dat");
  std::vector<std::set<int> > simplices = parseDat(file);

  SComplexBuilderFromSimplices<long, SComplexDefaultTraits> builder(3);
  boost::shared_ptr<Complex> complex = builder(simplices, 3, 1);

  CRef<ReducibleFreeChainComplexType> RFCComplexCR_orginal=
  	 (ReducibleFreeChainComplexOverZFromSComplexAlgorithm<Complex, ReducibleFreeChainComplexType>(*complex))();
  CRef<HomologySignature<int> > homSignCR_orginal=HomAlgFunctors<FreeModuleType>::homSignViaAR_Random(RFCComplexCR_orginal);

  COAKQAlgorithm<COAKQStrategy<Complex, Complex> > algorithm(new COAKQStrategy<Complex, Complex>(*complex));

  algorithm();

  BOOST_FOREACH(Complex::Iterators::AllCells::iterator::value_type v,
					 complex->iterators().allCells()) {
	 BOOST_CHECK_EQUAL(v.getColor(), (Complex::Color)2);
  }

  BOOST_CHECK(complex->iterators(1).allCells().begin() == complex->iterators(1).allCells().end());  

  Complex& coAKQ = algorithm.getStrategy().getOutputComplex();
  BOOST_CHECK_EQUAL(coAKQ.size(), 6);

  CRef<ReducibleFreeChainComplexType> RFCComplexCR=
  	 (ReducibleFreeChainComplexOverZFromSComplexAlgorithm<Complex, ReducibleFreeChainComplexType>(coAKQ))();
  CRef<HomologySignature<int> > homSignCR=HomAlgFunctors<FreeModuleType>::homSignViaAR_Random(RFCComplexCR);

  std::ostringstream signature;
  signature << "coAKQ: " << homSignCR()<< " | org: " << homSignCR_orginal();
  std::string sig = signature.str();
  std::replace(sig.begin(), sig.end(), '\n', '#');
  BOOST_CHECK_EQUAL(sig, "coAKQ:   H_0 = Z#  H_1 = 0#  H_2 = 0#  H_3 = Z# | org:   H_0 = Z#  H_1 = 0#  H_2 = 0#  H_3 = Z#");
}



BOOST_AUTO_TEST_CASE(nonpl_sphere) {
  typedef SComplex<SComplexDefaultTraits> Complex;

  fstream file(PROJECT_SOURCE_DIR"/data/spaces/nonpl_sphere.dat");
  std::vector<std::set<int> > simplices = parseDat(file);

  SComplexBuilderFromSimplices<long, SComplexDefaultTraits> builder(3);
  boost::shared_ptr<Complex> complex = builder(simplices, 3, 1);

  CRef<ReducibleFreeChainComplexType> RFCComplexCR_orginal=
  	 (ReducibleFreeChainComplexOverZFromSComplexAlgorithm<Complex, ReducibleFreeChainComplexType>(*complex))();
  CRef<HomologySignature<int> > homSignCR_orginal=HomAlgFunctors<FreeModuleType>::homSignViaAR_Random(RFCComplexCR_orginal);

  COAKQAlgorithm<COAKQStrategy<Complex, Complex> > algorithm(new COAKQStrategy<Complex, Complex>(*complex));

  algorithm();

  BOOST_FOREACH(Complex::Iterators::AllCells::iterator::value_type v,
					 complex->iterators().allCells()) {
	 BOOST_CHECK_EQUAL(v.getColor(), (Complex::Color)2);
  }

  BOOST_CHECK(complex->iterators(1).allCells().begin() == complex->iterators(1).allCells().end());  

  Complex& coAKQ = algorithm.getStrategy().getOutputComplex();
  BOOST_CHECK_EQUAL(coAKQ.size(), 6);

  CRef<ReducibleFreeChainComplexType> RFCComplexCR=
  	 (ReducibleFreeChainComplexOverZFromSComplexAlgorithm<Complex, ReducibleFreeChainComplexType>(coAKQ))();
  CRef<HomologySignature<int> > homSignCR=HomAlgFunctors<FreeModuleType>::homSignViaAR_Random(RFCComplexCR);

  std::ostringstream signature;
  signature << "coAKQ: " << homSignCR()<< " | org: " << homSignCR_orginal();
  std::string sig = signature.str();
  std::replace(sig.begin(), sig.end(), '\n', '#');
  BOOST_CHECK_EQUAL(sig, "coAKQ:   H_0 = Z#  H_1 = 0#  H_2 = 0#  H_3 = 0#  H_4 = 0#  H_5 = Z# | org:   H_0 = Z#  H_1 = 0#  H_2 = 0#  H_3 = 0#  H_4 = 0#  H_5 = Z#");
}


BOOST_AUTO_TEST_CASE(knot) {
  typedef SComplex<SComplexDefaultTraits> Complex;

  fstream file(PROJECT_SOURCE_DIR"/data/spaces/knot.dat");
  std::vector<std::set<int> > simplices = parseDat(file);

  SComplexBuilderFromSimplices<long, SComplexDefaultTraits> builder(3);
  boost::shared_ptr<Complex> complex = builder(simplices, 3, 1);

  CRef<ReducibleFreeChainComplexType> RFCComplexCR_orginal=
  	 (ReducibleFreeChainComplexOverZFromSComplexAlgorithm<Complex, ReducibleFreeChainComplexType>(*complex))();
  CRef<HomologySignature<int> > homSignCR_orginal=HomAlgFunctors<FreeModuleType>::homSignViaAR_Random(RFCComplexCR_orginal);

  COAKQAlgorithm<COAKQStrategy<Complex, Complex> > algorithm(new COAKQStrategy<Complex, Complex>(*complex));

  algorithm();

  BOOST_FOREACH(Complex::Iterators::AllCells::iterator::value_type v,
					 complex->iterators().allCells()) {
	 BOOST_CHECK_EQUAL(v.getColor(), (Complex::Color)2);
  }

  BOOST_CHECK(complex->iterators(1).allCells().begin() == complex->iterators(1).allCells().end());  

  Complex& coAKQ = algorithm.getStrategy().getOutputComplex();
  BOOST_CHECK_EQUAL(coAKQ.size(), 3);

  CRef<ReducibleFreeChainComplexType> RFCComplexCR=
  	 (ReducibleFreeChainComplexOverZFromSComplexAlgorithm<Complex, ReducibleFreeChainComplexType>(coAKQ))();
  CRef<HomologySignature<int> > homSignCR=HomAlgFunctors<FreeModuleType>::homSignViaAR_Random(RFCComplexCR);

  std::ostringstream signature;
  signature << "coAKQ: " << homSignCR()<< " | org: " << homSignCR_orginal();
  std::string sig = signature.str();
  std::replace(sig.begin(), sig.end(), '\n', '#');
  BOOST_CHECK_EQUAL(sig, "coAKQ:   H_0 = Z# | org:   H_0 = Z#");
}


void checkFile(const std::string& fileName) {

}


BOOST_AUTO_TEST_CASE(bjorner) {
  typedef SComplex<SComplexDefaultTraits> Complex;

  fstream file(PROJECT_SOURCE_DIR"/data/spaces/bjorner.dat");
  std::vector<std::set<int> > simplices = parseDat(file);

  SComplexBuilderFromSimplices<long, SComplexDefaultTraits> builder(3);
  boost::shared_ptr<Complex> complex = builder(simplices, 3, 1);

  CRef<ReducibleFreeChainComplexType> RFCComplexCR_orginal=
  	 (ReducibleFreeChainComplexOverZFromSComplexAlgorithm<Complex, ReducibleFreeChainComplexType>(*complex))();
  CRef<HomologySignature<int> > homSignCR_orginal=HomAlgFunctors<FreeModuleType>::homSignViaAR_Random(RFCComplexCR_orginal);

  COAKQAlgorithm<COAKQStrategy<Complex, Complex> > algorithm(new COAKQStrategy<Complex, Complex>(*complex));

  algorithm();

  BOOST_FOREACH(Complex::Iterators::AllCells::iterator::value_type v,
					 complex->iterators().allCells()) {
	 BOOST_CHECK_EQUAL(v.getColor(), (Complex::Color)2);
  }

  BOOST_CHECK(complex->iterators(1).allCells().begin() == complex->iterators(1).allCells().end());  

  Complex& coAKQ = algorithm.getStrategy().getOutputComplex();
  BOOST_CHECK_EQUAL(coAKQ.size(), 4);

  CRef<ReducibleFreeChainComplexType> RFCComplexCR=
  	 (ReducibleFreeChainComplexOverZFromSComplexAlgorithm<Complex, ReducibleFreeChainComplexType>(coAKQ))();
  CRef<HomologySignature<int> > homSignCR=HomAlgFunctors<FreeModuleType>::homSignViaAR_Random(RFCComplexCR);

  std::ostringstream signature;
  signature << "coAKQ: " << homSignCR()<< " | org: " << homSignCR_orginal();
  std::string sig = signature.str();
  std::replace(sig.begin(), sig.end(), '\n', '#');
  BOOST_CHECK_EQUAL(sig, "coAKQ:   H_0 = Z#  H_1 = 0#  H_2 = Z# | org:   H_0 = Z#  H_1 = 0#  H_2 = Z#");
}


// BOOST_AUTO_TEST_CASE(randomSimplices) {
//   typedef SComplex<SComplexDefaultTraits> Complex;

//   fstream file(PROJECT_SOURCE_DIR"/data/randomSimplices.dat");
//   std::vector<std::set<int> > simplices = parseDat(file);

//   SComplexBuilderFromSimplices<long, SComplexDefaultTraits> builder(3);
//   boost::shared_ptr<Complex> complex = builder(simplices, 3, 1);

//   CRef<ReducibleFreeChainComplexType> RFCComplexCR_orginal=
//   	 (ReducibleFreeChainComplexOverZFromSComplexAlgorithm<Complex, ReducibleFreeChainComplexType>(*complex))();
//   CRef<HomologySignature<int> > homSignCR_orginal=HomAlgFunctors<FreeModuleType>::homSignViaAR_Random(RFCComplexCR_orginal);

//   COAKQAlgorithm<COAKQStrategy<Complex, Complex> > algorithm(new COAKQStrategy<Complex, Complex>(*complex));

//   algorithm();

//   BOOST_FOREACH(Complex::Iterators::AllCells::iterator::value_type v,
// 					 complex->iterators().allCells()) {
// 	 BOOST_CHECK_EQUAL(v.getColor(), (Complex::Color)2);
//   }

//   BOOST_CHECK(complex->iterators(1).allCells().begin() == complex->iterators(1).allCells().end());  

//   Complex& coAKQ = algorithm.getStrategy().getOutputComplex();
//   BOOST_CHECK_EQUAL(coAKQ.size(), 174);

//   CRef<ReducibleFreeChainComplexType> RFCComplexCR=
//   	 (ReducibleFreeChainComplexOverZFromSComplexAlgorithm<Complex, ReducibleFreeChainComplexType>(coAKQ))();
//   CRef<HomologySignature<int> > homSignCR=HomAlgFunctors<FreeModuleType>::homSignViaAR_Random(RFCComplexCR);

//   std::ostringstream signature;
//   signature << "coAKQ: " << homSignCR()<< " | org: " << homSignCR_orginal();
//   std::string sig = signature.str();
//   std::replace(sig.begin(), sig.end(), '\n', '#');
//   BOOST_CHECK_EQUAL(sig, "coAKQ:   H_0 = Z#  H_1 = Z^39#  H_2 = Z^84# | org:   H_0 = Z#  H_1 = Z^39#  H_2 = Z^84#");
// }


BOOST_AUTO_TEST_SUITE_END()

