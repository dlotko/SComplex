
#include <boost/assign/list_of.hpp>
#include <boost/assign/list_inserter.hpp>

vector<set<int> > makeTest() {
  using namespace boost::assign;
  vector<set<int> > tris;

  tris = makeSpaceFromWelds(makeKleinWelds());
  // for (int i = 0; i < 8; i++)
  // 	 tris = subdivide3(tris);

  /*for (int i = 0; i < 1; ++i) {
	 push_back(tris)(list_of(i)(i+1)(i+2)(i+3)(i+4)(i+5)(i+6)(i+7)(i+8)(i+9)(i+10)(i+11)(i+12));
  }*/
  return tris;
}


template<typename SComplex>
void testReduce(SComplex& complex) {
	 Stopwatch swComp,swRed;
	 //(ShaveAlgorithmFactory::createDefault(SComplexCR()))();
    //cout << " --- Shave reduced the size to " << SComplexCR().cardinality() << " in " << swRed <<  endl;

    Stopwatch swCoRed;
    //(*CoreductionAlgorithmFactory::createDefault(complex))();
    //cout << " --- Coreduction reduced the size to " << complex.size() << " in " << swCoRed <<  endl;

	 CRef<ReducibleFreeChainComplexType> RFCComplexCR=
		(ReducibleFreeChainComplexOverZFromSComplexAlgorithm<SComplex, ReducibleFreeChainComplexType>(complex))();
	 cout << " --- RFCC constructed  " << endl;

	 CRef<HomologySignature> homSignCR=HomAlgFunctors<FreeModuleType>::homSignViaAR_Random(RFCComplexCR);
	 cout << " --- Computation completed in " << swComp  << std::endl;
	 cout << " --- Computed homology is: \n\n" << homSignCR()  << std::endl;
}
