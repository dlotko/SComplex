/****  CRHomS.cpp            ****/
/**** (c) Marian Mrozek 2009 ****/

#include <iostream>
using namespace std;

#include <capd/auxil/Stopwatch.h>
#include <capd/auxil/CRef.h>
#include <capd/homologicalAlgebra/embeddingDim.h>

#include <capd/vectalg/MatrixSlice.h>
#include <capd/matrixAlgorithms/intMatrixAlgorithms.hpp>

#include <capd/homologicalAlgebra/homologicalAlgebra.hpp>
#include <capd/homologicalAlgebra/homAlgFunctors.hpp>
#include <capd/homologicalAlgebra/cubSetFunctors.hpp>
#include <capd/homologicalAlgebra/ReducibleFreeChainComplex.hpp>

#include "CubSComplex.hpp"
#include "SComplexAlgs.hpp"

#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_io.hpp>
#include <boost/tuple/tuple_comparison.hpp>

#include <boost/test/unit_test.hpp>



typedef ElementaryCell ElementaryCellType;
typedef int ScalarType;
typedef FreeModule<int,capd::vectalg::Matrix<int,0,0> > FreeModuleType;
typedef FreeChainComplex<FreeModuleType> FreeChainComplexType;
typedef ReducibleFreeChainComplex<FreeModuleType,int> ReducibleFreeChainComplexType;

using namespace boost::unit_test;

template<typename SComplex>
boost::tuple<int, int, int, std::string>  CrHomS(const std::string &fileName) {
	 boost::tuple<int, int, int, std::string> result;
	 using boost::tuples::get;
	 
  CRef<SComplex> SComplexCR(new SComplex(readCubCellSet<BCubSet,BCubCelSet>(fileName.c_str())));
  get<0>(result) = SComplexCR().cardinality();
  (ShaveAlgorithmFactory::createDefault(SComplexCR()))();  
  get<1>(result) = SComplexCR().cardinality();
  
  (CoreductionAlgorithmFactory::createDefault(SComplexCR()))();
  get<2>(result) = SComplexCR().cardinality();
  
  CRef<ReducibleFreeChainComplexType> RFCComplexCR=
	 (ReducibleFreeChainComplexOverZFromSComplexAlgorithm<CubSComplex, ReducibleFreeChainComplexType,ElementaryCellType>(SComplexCR()))();
  CRef<HomologySignature> homSignCR=HomAlgFunctors<FreeModuleType>::homSignViaAR_Random(RFCComplexCR);
  get<3>(result) = homSignCR().bettiVector();
  
  return result;
}


void reduction_test() {
  BOOST_CHECK_EQUAL(CrHomS<CubSComplex>("test/input_1.bmd"), boost::make_tuple(71639264, 817510, 3815, "0,1057"));
}
