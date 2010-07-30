#include <iostream>
#include <cmath>
#include <cstdlib>
#include <iterator>
#include <vector>
#include <set>
#include <map>
#include <cassert>
// #include <array>
#include <list>
using namespace std;

#include "redHom/complex/simplicial/simple_set.h"

#include "redHom/complex/simplicial/Simplex.hpp"
#include "redHom/complex/simplicial/SimplexSComplex.hpp"

#include <iostream>
#include <queue>
#include <deque>

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


ofstreamcout fcout; // ?

// #include "SComplexAlgs.hpp"

typedef int ScalarType;
typedef FreeModule<int,capd::vectalg::Matrix<int,0,0> > FreeModuleType;
typedef FreeChainComplex<FreeModuleType> FreeChainComplexType;
typedef ReducibleFreeChainComplex<FreeModuleType,int> ReducibleFreeChainComplexType;

#include <SComplex.hpp>
#include <SComplexDefaultTraits.hpp>
#include <SComplexAlgs.hpp>
#include <SComplexBuilderFromSimplices.hpp>

#include "SimplexSubdivision.hpp"
#include <CrHomS.hpp>
template<typename SComplex>
void testReduceMS(SComplex& complex) {

	cout << "\n\ntesting on morse-smale complex\n\n";
	 Stopwatch swComp,swRed;
	 //(ShaveAlgorithmFactory::createDefault(SComplexCR()))();
    //cout << " --- Shave reduced the size to " << SComplexCR().cardinality() << " in " << swRed <<  endl;


	 CRef<ReducibleFreeChainComplexType> RFCComplexCR=
		(ReducibleFreeChainComplexOverZFromSComplexAlgorithm<SComplex, ReducibleFreeChainComplexType>(complex))();
	 cout << " --- RFCC constructed  " << endl;

	 CRef<HomologySignature> homSignCR=HomAlgFunctors<FreeModuleType>::homSignViaAR_Random(RFCComplexCR);
	 cout << " --- Computation completed in " << swComp  << std::endl;
	 cout << " --- Computed homology is: \n\n" << homSignCR()  << std::endl;
}


void CrHomS_torus(int argc,char* argv[])
{
    Stopwatch swTot;
	 vector<set<int> > tris = makeTest();

	Stopwatch swBuild;
	typedef SComplex<SComplexDefaultTraits> Complex;
	SComplexBuilderFromSimplices<long, SComplexDefaultTraits> builder(1234567);

	boost::shared_ptr<Complex> complex = builder(tris, 3, 1);
	cout << " --- built in " << swBuild << std::endl;

	boost::shared_ptr<CoreductionAlgorithm<DefaultReduceStrategy<Complex> > >
    cored = CoreductionAlgorithmFactory::createDefault(*complex);

    (*cored)();

	testReduceMS(*cored->getStrategy()->outputComplex);
    delete cored->getStrategy()->outputComplex;

	// testReduce(*complex);
    // cout << " --- generated simplicial complex --- \n cardinality: " << complex->size() << endl;
}

int main(int argc,char* argv[])
{
    try
    {
		CrHomS_torus(argc,argv);
    }
    catch (std::exception& e)
    {
        std::cout << "Caught exception: " << e.what() << endl;
    }
    catch (std::string& s)
    {
        std::cout << "Caught exception: " << s.c_str() << endl;
    }
    catch (const char* c)
    {
        std::cout << "Caught exception: " << c << endl;
    }
    catch (...)
    {
        std::cout << "Caught an unknown exception: " << endl;
    }
}
