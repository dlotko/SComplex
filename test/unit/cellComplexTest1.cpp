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

//for unit tests. This procedure write the coefs of all the cubes into one vector.
template <typename T>
typename std::vector< std::vector< std::pair<T,T> > > writeAllCubes( cellComplex<T>* cmplx )
{
      std::vector< std::vector< std::pair<T,T> > > result;

      for ( unsigned int i = 0 ; i != cmplx->elemen().size() ; ++i )
      {
           for ( typename std::set< cell<T>* >::iterator it = cmplx->elemen()[i].begin() ; it != cmplx->elemen()[i].end() ; ++it )
           {
               std::vector< std::pair<T,T> > coef;
               for ( unsigned int j = 0 ; j != (*it)->coords().size() ; ++j )
               {
                   coef.push_back( std::make_pair( (*it)->coords()[j].first , (*it)->coords()[j].second ) );
               }
               result.push_back( coef );
           }
      }
      return result;
}//writeAllCubes



template <typename T>
bool compateTwoVectors( std::vector< std::vector< std::pair<T,T> > > first, std::vector< std::vector< std::pair<T,T> > > second)
{
     if ( first.size() != second.size() )
     {
          return false;
     }

     //for every element in first search for the same element in second
     for ( unsigned int i = 0 ; i != first.size() ; ++i )
     {
         bool found = false;
         for ( unsigned int j = 0 ; j != second.size() ; ++j )
         {
             //if first[i] == second[j], then found = true and break;
             bool theSame = true;
             if ( first[i].size() == second[j].size() )
             {
                  for ( unsigned int k = 0 ; k != first[i].size() ; ++k )
                  {
                      if ( first[i][k].first != second[j][k].first )
                      {
                           theSame = false;
                      }
                      if ( first[i][k].second != second[j][k].second )
                      {
                           theSame = false;
                      }
                  }
             }
             else
             {
                 theSame = false;
             }
             if ( theSame )
             {
                  found = true;
                  break;
             }
         }
         if ( !found ){return false;}
     }
     return true;
}//compateTwoVectors




BOOST_AUTO_TEST_CASE(cell1)
{
    std::vector< std::vector< std::pair<double,double> > > fullRectangle;
    //vertices
    std::vector< std::pair<double,double> > leftBottom;
    std::pair<double,double> zerozero = std::make_pair(0,0);
    leftBottom.push_back( zerozero );
    leftBottom.push_back(  std::make_pair(0,0)  );
    std::vector< std::pair<double,double> > leftTop;
    leftTop.push_back( std::make_pair(0,0) );
    leftTop.push_back(  std::make_pair(1,1)  );
    std::vector< std::pair<double,double> > rightBottom;
    rightBottom.push_back( std::make_pair(1,1) );   rightBottom.push_back(  std::make_pair(0,0)  );
    std::vector< std::pair<double,double> > rightTop;
    rightTop.push_back( std::make_pair(1,1) );   rightTop.push_back(  std::make_pair(1,1)  );
    //edges
    std::vector< std::pair<double,double> > left;
    left.push_back( std::make_pair(0,0) );   left.push_back(  std::make_pair(0,1)  );
    std::vector< std::pair<double,double> > right;
    right.push_back( std::make_pair(1,1) );   right.push_back(  std::make_pair(0,1)  );
    std::vector< std::pair<double,double> > bottom;
    bottom.push_back( std::make_pair(0,1) );   bottom.push_back(  std::make_pair(0,0)  );
    std::vector< std::pair<double,double> > top;
    top.push_back( std::make_pair(0,1) );   top.push_back(  std::make_pair(1,1)  );
    //face
    std::vector< std::pair<double,double> > interior;
    interior.push_back( std::make_pair(0,1) );   interior.push_back(  std::make_pair(0,1)  );

    fullRectangle.push_back(leftBottom);
    fullRectangle.push_back(leftTop);
    fullRectangle.push_back(rightBottom);
    fullRectangle.push_back(rightTop);
    fullRectangle.push_back(left);
    fullRectangle.push_back(right);
    fullRectangle.push_back(bottom);
    fullRectangle.push_back(top);
    fullRectangle.push_back(interior);









    std::vector< std::pair<double,double> > point;
    point.push_back( std::make_pair(0,1) );
    point.push_back( std::make_pair(0,1) );
    cellComplex<double>* cmplx = new cellComplex<double>( point );

    std::vector< std::vector< std::pair<double,double> > > out = writeAllCubes<double>( cmplx );


    string out = "OK";
    if ( !compateTwoVectors(fullRectangle,out) )
    {
         out =


          "Blad w test1 przy tetowaniu rectangular cell complex!";
    }

    delete cmplx;
    BOOST_CHECK_EQUAL(get<0>(out,"OK"));

}

BOOST_AUTO_TEST_SUITE_END()
