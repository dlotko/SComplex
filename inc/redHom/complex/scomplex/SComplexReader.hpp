#ifndef _SCOMPLEX_READER_HPP
#define _SCOMPLEX_READER_HPP

#include "SComplex.hpp"
#include "../../RedHomCAPD.h"


template<typename Traits>
class SComplexReader
{

public:
    typedef SComplex<Traits> Complex;

private:

    struct ElementaryCubeComparator
    {
        bool operator()(const ElementaryCube& ec1, const ElementaryCube& ec2) const
        {

        }
    };

    class BmpSComplexBuilder
    {

    public:
        BmpSComplexBuilder(): fullCubes(false), embDim(0) {}

        bool isBoundingBoxNeeded() const
        {
            //  zwraca informacje czy odbiorca potrzebuje bounding box
            // jesli informacja ta jest niepotrzebna to zwracamy false
            return false;
        }

        void setBoundingBox(int A_cmin[],int A_cmax[]) const
        {
            // Dostarcza wspolrzedne  boundingBox do odbiorcy (wywolywane
            // tylko jesli na wczesniejsze pytanie
            //   odpowiedz byla pozytywna)
            // Jesli wspolrzedne te nie sa potrzebne, wystarcza pusta implementacja
        }

        void setFullCubes(bool b)
        {
            // Informuje o typie kostek. Jesli b==true,
            // dostarczane sa wylacznie kostki wymiaru rownego wymiarowi wlozenia
            fullCubes=b;
        }
        void setDim(int A_embDim)
        {
            // Dostarcza wymiar wlozenia
            embDim=A_embDim;
        }

	void finalize() {}

         void setFileType(FileType A_fileType){
	// provides embedding dimension
       	fileType=A_fileType;
	  }

      FileType getFileType(){
	return fileType;
	  }

        void addCell(int coords[])
        {
            // Dostarcza n wspolrzednych kostki, gdzie n to wymiar wlozenia
            // W przypadku gdy setFullCubes dostarczylo false wspolrzedne sa parzyste
            // dla przedzialow zdegenerowanych, a w przeciwnym razie sa nieparzyste
            // W przypadku gdy setFullCubes dostarczylo true dostarczane sa wspolrzedne
            // lewego dolnego rogu pelnej kostki
            if (fullCubes)
            {
                std::vector<int> data;
                data.reserve(embDim);
                for (int i=0;i<embDim;++i)
                {
                    data.push_back(coords[i]);
                }
                getId(ElementaryCube(data));
            }
            else
            {
                std::vector<int> data;
                bool* parity=new bool[embDim];
                data.reserve(embDim);
                for (int i=0;i<embDim;++i)
                {
                    data.push_back(coords[i]/2);

                    parity[i]=(coords[i] % 2);
                }
                getId(ElementaryCube(&data[0],parity,embDim));
            }
        }

        boost::shared_ptr<Complex> create(int colors, int defaultColor)
        {

            kappaMap.clear();
            computedBoundaries.clear();

            CubeIds tmpCubeIds = cubeIds;
            for (CubeIds::iterator it = tmpCubeIds.begin(), end = tmpCubeIds.end(); it != end; ++it)
            {
                computeKappa(it->first, it->second);
            }

            const int size = cubeIds.size();
            typename Complex::Dims dims(size);

            for (CubeIds::iterator it = cubeIds.begin(), end = cubeIds.end(); it != end; ++it)
            {
                dims[it->second] = it->first.dimension();
            }

            return boost::shared_ptr<Complex>(new Complex(colors, dims, kappaMap, defaultColor));
        }

    private:

        void computeKappa(const ElementaryCube& cube, int id)
        {
            if (computedBoundaries.find(id) != computedBoundaries.end())
            {
                return;
            }

            computedBoundaries.insert(id);

            typedef std::map<ElementaryCube, int> Boundary;
            Boundary boundary;
            cube.boundary(boundary);

            for (Boundary::iterator bIt = boundary.begin(), bEnd = boundary.end(); bIt != bEnd; ++bIt)
            {
                const int bId = getId(bIt->first);
                kappaMap.push_back(boost::make_tuple(id, bId, bIt->second));
                computeKappa(bIt->first, bId);
            }

        }

        int getId(ElementaryCube cube)
        {
            CubeIds::iterator it = cubeIds.find(cube);
            if (it == cubeIds.end())
            {
                it = cubeIds.insert(std::make_pair(cube, cubeIds.size())).first;
            }
            return it->second;
        }

        typedef std::map<ElementaryCube, int> CubeIds;
        std::set<int> computedBoundaries;

	FileType fileType;
        CubeIds cubeIds;
        typename Complex::KappaMap kappaMap;
        bool fullCubes;
        int embDim;
    };

public:

  boost::shared_ptr<Complex> operator()(ifstream& file, int colors, int defaultColor)
  {
    BmpSComplexBuilder builder;
    readCubicalSet(file, builder);
    
    return builder.create(colors, defaultColor);
  }

    boost::shared_ptr<Complex> operator()(std::string fileName, int colors, int defaultColor)
    {
        ifstream file;
        file.open(fileName.c_str());
        if (!file)
        {
            std::cerr << "File not found: " << fileName << std::endl;
            return boost::shared_ptr<Complex>();
        }

	return (*this)(file, colors, defaultColor);
    }


};


#endif //  _SCOMPLEX_READER_HPP

