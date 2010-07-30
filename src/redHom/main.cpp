#include <boost/algorithm/string/join.hpp>
#include <RedHomOptions.hpp>

#include <iostream>

#include <boost/mpl/vector.hpp>
#include <boost/mpl/joint_view.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/algorithm/string.hpp>

#include <redHom/complex/scomplex/SComplex.hpp>
#include <redHom/complex/scomplex/SComplexReader.hpp>
#include <redHom/complex/scomplex/SComplexDefaultTraits.hpp>
#include <redHom/complex/scomplex/SComplexBuilderFromSimplices.hpp>

#include <redHom/complex/cubical/CubSComplex.hpp>
#include <redHom/complex/cubical/CubSComplexReader.hpp>
#include <redHom/algorithm/Algorithms.hpp>
#include <redHom/algorithm/strategy/DefaultReduceStrategy_CubSComplex.hpp>

#include <redHom/complex/simplicial/Simplex.hpp>
#include <redHom/complex/simplicial/SimplexIO.hpp>
#include <redHom/complex/simplicial/SimplexSComplex.hpp>


enum ComplexMajorId {SComplexMajorId = 10, 
		     SComplexSimplicesMajorId, 
		     SComplexCubesMajorId, 
		     CubSComplexMajorId, 
		     SimplexSComplexMajorId};

typedef boost::tuple<ComplexMajorId, int, boost::any> ComplexTuple;

template<typename Complex, ComplexMajorId MAJOR_ID, int MINOR_ID>
struct ComplexDescriptor {
  typedef Complex type;
  static const ComplexMajorId majorId = MAJOR_ID;
  static const int minorId = MINOR_ID;
};



typedef boost::mpl::vector<ComplexDescriptor<CubSComplex<2>, CubSComplexMajorId, 2>,
			   ComplexDescriptor<CubSComplex<3>, CubSComplexMajorId, 3>,
			   ComplexDescriptor<CubSComplex<4>, CubSComplexMajorId, 4>,
			   ComplexDescriptor<CubSComplex<5>, CubSComplexMajorId, 5>,
			   ComplexDescriptor<CubSComplex<6>, CubSComplexMajorId, 6>,
			   ComplexDescriptor<CubSComplex<7>, CubSComplexMajorId, 7>,
			   ComplexDescriptor<CubSComplex<8>, CubSComplexMajorId, 8>,
			   ComplexDescriptor<CubSComplex<9>, CubSComplexMajorId, 9>
			   > CubComplexDescriptors;
typedef boost::mpl::joint_view<CubComplexDescriptors,
			       boost::mpl::vector<
				 ComplexDescriptor<SComplex<SComplexDefaultTraits>, SComplexMajorId, 0>,
				 ComplexDescriptor<SComplex<SComplexDefaultTraits>, SComplexCubesMajorId, 0>,
				 ComplexDescriptor<SComplex<SComplexDefaultTraits>, SComplexSimplicesMajorId, 0>,
				 ComplexDescriptor<SimplexSComplex, SimplexSComplexMajorId, 0>				 
				 > > ComplexDescriptors;


class MultiDimCubSComplexReader {

  struct ComplexConstructor {
    const int dim;
    std::ifstream& file;
    ComplexTuple& complex;

    ComplexConstructor(int _dim,  std::ifstream& _file, ComplexTuple& _complex): 
      dim(_dim), file(_file), complex(_complex) {
      BOOST_ASSERT(boost::get<2>(complex).empty());
    }

    template<typename Complex, ComplexMajorId MajorId, int MinorId>
    void operator()(ComplexDescriptor<Complex, MajorId, MinorId>) {
      if (MajorId == CubSComplexMajorId && dim == MinorId) {
	BOOST_ASSERT(boost::get<2>(complex).empty());      

	typedef typename Complex::BCubCellSet CubCellSet;
	CRef<CubCellSet> cubCellSetCR(new CubCellSet());
	BmpCubCelSetBuilder<CubCellSet> csb(cubCellSetCR);
	readCubicalSet(file, csb);

	complex = ComplexTuple(MajorId, MinorId, 
			       boost::any(boost::shared_ptr<Complex>(new Complex(cubCellSetCR))));
      }
    }

  };

public:
  static ComplexTuple read(ifstream& file, const std::string& fileName) {
    ComplexTuple complex;    
    
    CubSetBuilder csb;
    readCubicalSet(file,csb,true); //true means only get dim and type
    file.seekg(0);

    unsigned int embDim=csb.getDim();

    ComplexConstructor constructor(embDim, file, complex);
    boost::mpl::for_each<CubComplexDescriptors>(constructor);

    return complex;
  }

};

class SComplexReaderFromCubes {

public:
  static ComplexTuple read(ifstream& file, const std::string& fileName) {
    SComplexReader<SComplexDefaultTraits> reader;
    
    return boost::make_tuple(SComplexCubesMajorId, 0, reader(file, 3, 1));
  }

};

class SComplexReaderFromSimplices {

public:
  static ComplexTuple read(ifstream& file, const std::string& fileName) {
    std::vector<std::set<int> > simplices;
    if (boost::iends_with(fileName, ".dat")) {
      simplices = parseDat(file);
    } else if (boost::iends_with(fileName, ".obj")) {
      simplices = parseObj(file);
    } else {
      throw std::logic_error("Unknown file type: " + fileName + " (*.obj and *.dat supported");
    }

    SComplexBuilderFromSimplices<long, SComplexDefaultTraits> builder(3);
    boost::shared_ptr<SComplex<SComplexDefaultTraits> > complex = builder(simplices, 3, 1);
    
    return boost::make_tuple(SComplexSimplicesMajorId, 0, complex);
  }

};

class SimplexSComplexReader {

public:
  static ComplexTuple read(ifstream& file, const std::string& fileName) {
    boost::shared_ptr<SimplexSComplex> complex(new SimplexSComplex());
    if (boost::iends_with(fileName, ".dat")) {
      parseDat(file, *complex);
    } else if (boost::iends_with(fileName, ".obj")) {
      parseObj(file, *complex);
    } else {
      throw std::logic_error("Unknown file type: " + fileName + " (*.obj and *.dat supported");
    }

    return boost::make_tuple(SimplexSComplexMajorId, 0, complex);
  }

};


template<typename ComplexReader, int MAJOR_ID>
struct ComplexReaderDescriptor {
  typedef ComplexReader type;
  static const int majorId = MAJOR_ID;
};

typedef boost::mpl::vector<ComplexReaderDescriptor<MultiDimCubSComplexReader, CubSComplexMajorId>,
			   ComplexReaderDescriptor<SComplexReaderFromCubes, SComplexCubesMajorId>,
			   ComplexReaderDescriptor<SComplexReaderFromSimplices, SComplexSimplicesMajorId>,
			   ComplexReaderDescriptor<SimplexSComplexReader, SimplexSComplexMajorId>
			   > ComplexReaderDescriptors;


struct ComplexReader {
  ComplexTuple& complex;
  const ComplexMajorId majorId;
  const std::string fileName;

  ComplexReader(ComplexMajorId _majorId, ComplexTuple& _complex, const std::string _fileName):
    majorId(_majorId), complex(_complex), fileName(_fileName) {
    BOOST_ASSERT(boost::get<2>(complex).empty());
  }

  template<typename Reader, int MajorId>
  void operator()(ComplexReaderDescriptor<Reader, MajorId>) {
    if (MajorId == majorId) {
      BOOST_ASSERT(boost::get<2>(complex).empty());      
      ifstream file;

      file.open(fileName.c_str());
      if (!file) {
	throw std::logic_error("File not found: " + fileName);
      }

      complex = Reader::read(file, fileName);

      file.close();
    }
  }

};

template<typename Derive>
class AlgorithmExecutor {
  std::string name;

protected:
  ComplexTuple& complex;

  AlgorithmExecutor(ComplexTuple& _complex, const std::string _name):
    complex(_complex), name(_name) {  
  }

public:

  template<typename Complex, ComplexMajorId MajorId, int MinorId>
  void operator()(ComplexDescriptor<Complex, MajorId, MinorId>) {
    if (boost::get<0>(complex) == MajorId && boost::get<1>(complex) == MinorId) {
      Stopwatch stopwatch;
      boost::shared_ptr<Complex>& complexPtr = boost::any_cast<boost::shared_ptr<Complex>&>(boost::get<2>(complex));
      
      std::cout << "Starting: " << name << " for (" << MajorId << ", " << MinorId << ")" << std::endl;
      ((Derive*)this)->run(*complexPtr);
      std::cout << "Finished: " << name << " in: " << stopwatch << std::endl;
    }
  }

};

struct ShaveExecutor: AlgorithmExecutor<ShaveExecutor> {

  ShaveExecutor(ComplexTuple& _complex): AlgorithmExecutor<ShaveExecutor>(_complex, "Shave") {}

  template<typename Complex>
  void run(Complex& complex) {
    size_t reduced = (ShaveAlgorithmFactory::createDefault(complex))();
    std::cout << "Algorithm reduced: " << reduced << std::endl;
  }

};

struct CoreductionExecutor: AlgorithmExecutor<CoreductionExecutor> {

  CoreductionExecutor(ComplexTuple& _complex): AlgorithmExecutor<CoreductionExecutor>(_complex, "Coreduction") {}

  template<typename Complex>
  void run(Complex& complex) {
    size_t reduced = (*CoreductionAlgorithmFactory::createDefault(complex))();
    std::cout << "Algorithm reduced: " << reduced << std::endl;
  }

};

struct RFCExecutor: AlgorithmExecutor<RFCExecutor> {

  RFCExecutor(ComplexTuple& _complex): AlgorithmExecutor<RFCExecutor>(_complex, "RFC") {}

  template<typename Complex>
  void run(Complex& complex) {
    typedef FreeModule<int,capd::vectalg::Matrix<int,0,0> > FreeModuleType;
    typedef ReducibleFreeChainComplex<FreeModuleType,int> ReducibleFreeChainComplexType;
    
    Stopwatch stopwatchConstruct;
    CRef<ReducibleFreeChainComplexType> RFCComplexCR = 
      (ReducibleFreeChainComplexOverZFromSComplexAlgorithm<Complex, ReducibleFreeChainComplexType>(complex))();
    std::cout << "Constructed Free Chain Compklex in: " << stopwatchConstruct << std::endl;

    Stopwatch stopwatchSignature;
    CRef<HomologySignature<int> > homSignCR=HomAlgFunctors<FreeModuleType>::homSignViaAR_Random(RFCComplexCR);
    std::cout << "Signature calculated in: " << stopwatchSignature << std::endl;

    std::cout << homSignCR() << std::endl;
  }

};

struct AKQExecutor: AlgorithmExecutor<AKQExecutor> {
  ComplexTuple& outputComplex;

  AKQExecutor(ComplexTuple& _complex, ComplexTuple& _outputComplex): 
    AlgorithmExecutor<AKQExecutor>(_complex, "AKQ"), outputComplex(_outputComplex) {}

  template<typename Complex>
  void run(Complex& complex) {
    CoreductionAlgorithm<AKQReduceStrategy<Complex> > algorithm(new AKQReduceStrategy<Complex>(complex));
    algorithm();

    SComplex<SComplexDefaultTraits>* AKQ = algorithm.getStrategy()->getOutputComplex();
 
    outputComplex = boost::make_tuple(SComplexMajorId, 0, boost::shared_ptr<SComplex<SComplexDefaultTraits> >(AKQ));
  }

};

namespace {

  void validateOptions(const RedHomOptions& options) throw(std::logic_error) {
    switch (options.getFileType()) {
    case RedHomOptions::cubical : break;
    case RedHomOptions::simplicial : break;
    default: throw std::logic_error("Unknown file type");
    }

    if (options.getInputFilesCount() != 1) {
      throw std::logic_error("Support exactly one input file");
    }
  }

  ComplexMajorId determineComplexMajorId(const RedHomOptions& options) throw(std::logic_error) {    
    if (options.getFileType() == RedHomOptions::cubical && ! options.isGeneralSComplex()) {
      std::cout << "Using CubSComplex" << std::endl;
      return CubSComplexMajorId;
    } else if (options.getFileType() == RedHomOptions::cubical && options.isGeneralSComplex()) {
      std::cout << "Using SComplex for cubes" << std::endl;
      return SComplexCubesMajorId;
    } else if (options.getFileType() == RedHomOptions::simplicial && !options.isGeneralSComplex()) {
      std::cout << "Using SimplexSComplex" << std::endl;
      return SimplexSComplexMajorId;
    } else if (options.getFileType() == RedHomOptions::simplicial && options.isGeneralSComplex()) {
      std::cout << "Using SComplex for simplices" << std::endl;
      return SComplexSimplicesMajorId;
    }


    throw logic_error("Unsupported file reader type");
  }

}

int main(int argc, char** argv) {

  try {
    fcout.open("capd-report.txt");
    fcout.turnOn();

    Stopwatch totalStopwatch;

    RedHomOptions options(argc, argv);

    if (options.isHelp()) {
      std::cout << options.getHelp() << std::endl;
      return 0;
    }

    validateOptions(options);

    ComplexTuple complex;
    ComplexMajorId complexMajorId = determineComplexMajorId(options);
    ComplexReader complexReader(complexMajorId, complex, options.getInputFile(0));

    Stopwatch readStopwatch;
    boost::mpl::for_each<ComplexReaderDescriptors>(complexReader);
    std::cout << "Complex read in: " << readStopwatch << std::endl;


    if (options.isRfcAtStart()) {
      boost::mpl::for_each<ComplexDescriptors>(RFCExecutor(complex));
    }

    if (options.isShave()) {
      boost::mpl::for_each<ComplexDescriptors>(ShaveExecutor(complex));
    }

    if (options.isCoreduction()) {
      boost::mpl::for_each<ComplexDescriptors>(CoreductionExecutor(complex));
    }

    if (options.isAKQ()) {
      ComplexTuple outputComplex;
      boost::mpl::for_each<ComplexDescriptors>(AKQExecutor(complex, outputComplex));
      complex = outputComplex;
    }

    if (options.isRfcAtEnd()) {
      boost::mpl::for_each<ComplexDescriptors>(RFCExecutor(complex));
    }
    
    std::cout << "Total execution: " << totalStopwatch << std::endl;
  } catch (std::exception& ex) {
    std::cerr << "Error: " << ex.what() << std::endl;
    return 1;
  } catch(...) {
    std::cerr << "Unknown exception" << std::endl;
    return 1;
  }

  return 0;
}
