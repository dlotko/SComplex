/****  CRHomS.cpp            ****/
/**** (c) Marian Mrozek 2009 ****/

#include <iostream>
using namespace std;

#include <redHom/complex/cubical/CubSComplex.hpp>
#include <redHom/complex/cubical/CubSComplexReader.hpp>
#include <redHom/algorithm/Algorithms.hpp>
#include <redHom/algorithm/strategy/DefaultReduceStrategy_CubSComplex.hpp>

#ifndef DIM
//#error "Define dimension"
#define DIM EMBEDDING_DIM
#endif

int COPY = 0;
void DO_COPY(int* d, const int* s, int n) {
  for (int i = 0; i < n; ++i)
	 d[i] = s[i];
  ++COPY;
}

int FAILED = 0;
long INSERTED = 0;
int CALL = 0;

template<typename SComplex>
void CrHomS(int argc,char* argv[]){
  Stopwatch swTot;
  string fileName=argc>1 ? string(argv[1]) : "sphere1d.txt";
  cout << " --- Reading cubical cellular set from  " << fileName  << endl;

  CubSComplexReader<DIM> reader;
  boost::shared_ptr<SComplex> complex = reader(fileName.c_str());

  cout << "Successfully read  " << fileName <<
          " of "  << complex->cardinality() << " cells " << endl;


  Stopwatch swComp,swRed;
  (ShaveAlgorithmFactory::createDefault(*complex))();  
  cout << " --- Shave reduced the size to " << complex->cardinality() << " in " << swRed <<  endl;
  
   Stopwatch swCoRed;
  (*CoreductionAlgorithmFactory::createDefault(*complex))();
   cout << " --- Coreduction reduced the size to " << complex->cardinality() << " in " << swCoRed <<  endl;

  // CRef<ReducibleFreeChainComplexType> RFCComplexCR=
  // 		(ReducibleFreeChainComplexOverZFromSComplexAlgorithm<CubSComplex, ReducibleFreeChainComplexType>(SComplexCR()))();
  // cout << " --- RFCC constructed  " << endl;

  // CRef<HomologySignature> homSignCR=HomAlgFunctors<FreeModuleType>::homSignViaAR_Random(RFCComplexCR);
  // cout << " --- Computation completed in " << swComp  << std::endl;
  // cout << " --- Computed homology is: \n\n" << homSignCR()  << std::endl;

	cout << " --- Total computation time is: " << swTot  << " copy " << COPY << " failed " << FAILED << " inserted :" << INSERTED << "CALL " << CALL << std::endl;

}
ofstreamcout fcout;

int main(int argc,char* argv[]){

  CrHomS<CubSComplex<DIM> >(argc,argv);
}

