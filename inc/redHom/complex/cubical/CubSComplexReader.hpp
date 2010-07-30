#ifndef _CUBSCOMPLEX_READER_HPP
#define _CUBSCOMPLEX_READER_HPP

#include "CubSComplex.hpp"
#include "../../RedHomCAPD.h"

#include <boost/shared_ptr.hpp>


template<int DIM>
class CubSComplexReader
{
class BmpCubSetBuilder
    {
        typedef RepSet<ElementaryCube> CubicalSet;
    public:
        BmpCubSetBuilder():
                fullCubes(true),
                embDim(0),
                cubicalSet(new CubicalSet())
        {}

        bool isBoundingBoxNeeded() const
        {
            //  zwraca informacje czy odbiorca potrzebuje bounding box
            // jesli informacja ta jest niepotrzebna to zwracamy false
            return false;
        }
        void setBoundingBox(int A_cmin[],int A_cmax[]) const
        {
            // Dostarcza wspolrzedne  boundingBox do odbiorcy (wywolywane tylko jesli na wczesniejsze pytanie
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
            if (embDim != DIM)
            {
                throw std::runtime_error("Wrong dimension!");
            }
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
		  //data.push_back(2*coords[i]+1);
		  data.push_back(coords[i]);
                }
                cubicalSet->insert(ElementaryCube(data));
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
                cubicalSet->insert(ElementaryCube(&data[0],parity,embDim));
            }
        }

        //boost::shared_ptr<CubicalSet> getRepSet() {
        CubicalSet* getRepSet()
        {
            return cubicalSet.get();
        }

	void finalize() {}

      void setFileType(FileType A_fileType){
      // provides embedding dimension
      fileType=A_fileType;
    }

      FileType getFileType(){
      return fileType;
      }

    private:
        bool fullCubes;
        int embDim;
        boost::shared_ptr<CubicalSet> cubicalSet;
	FileType fileType;
    };

public:

    boost::shared_ptr<CubSComplex<DIM> > operator()(std::string fileName)
    {
        ifstream file;
        file.open(fileName.c_str());
        if (!file)
        {
            std::cerr << "File not found: " << fileName << std::endl;
            return boost::shared_ptr<CubSComplex<DIM> >();
        }

        BmpCubSetBuilder csb;
        readCubicalSet(file,csb);

        return boost::shared_ptr<CubSComplex<DIM> >(new CubSComplex<DIM>(*csb.getRepSet()));
    }


};


#endif //  _CUBSCOMPLEX_READER_HPP

