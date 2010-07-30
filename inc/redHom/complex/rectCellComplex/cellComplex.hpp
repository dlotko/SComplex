#include"cellComplex.h"
using namespace std;


template <typename T>
cellComplex<T>::cellComplex( std::vector< cell<T>* > vect )
{
    /*
    //this constructor construct a cellComplex from a set of its maximal faces. The cell complex do not have to be pure.
    //Also in the vecctor not only the maximal faces have to be present.

    //first find the maximal dimension of cell in vect:
    int max_dim = -1;
    for ( unsigned int i = 0 ; i != vect.size() ; ++i )
    {
        if ( vect[i]->dim() > max_dim ){max_dim = vect[i]->dim();}
    }

    //create the std::vector< std::set< cell<T>* > > structure of cellComplex:
    for ( int i = 0 ; i <= max_dim ; ++i )
    {
        this->elements.push_back(std::set< cell<T>* >);
    }

    //put the elements fromvect to this->elements:
    for ( unsigned int i = 0 ; i != vect.size() ; ++i )
    {
        this->elements[ vect[i]->dim() ].insert( vect[i] );
    }

    //now, startig from the top dimension generate all the faces and set boundary and coboundary pointers in a right way.
    //The generation process used here is a lazy one. It means, that even if some element of the complex can be generated many times,
    //it is generated many times. Then, in each levelthe repetitions are removed with help of multimap data structure.
    for ( int i = max_dim ; i != 0 ; --i )
    {
        //having all i-dimensional cells construct (i-1)-dimensional cells
        for ( typename std::set< cell<T>* >::iterator it = this->elements[i].begin() ; it != this->elements[i].end() ; ++it )
        {
             //generate all the faces of (*it), set the bd and cbd pointers
             std::vector< std::pair<T,T> > ce;
             for ( unsigned int j = 0 ; j != (*it)->coef.size() ; ++j )
             {
                 ce.push_back( std::make_pair<T,T>( (*it)->coef[j]->first , (*it)->coef[j]->second ) );
             }

             for ( unsigned int j = 0 ; j != (*it)->coef.size() ; ++j )
             {
                  if ( (*it)->coef[j]->first != (*it)->coef[j]->second )
                  {
                       std::pair<T,T> buf = ce[j];
                       ce[j] = std::make_pair<T,T>((*it)->coef[j]->first,(*it)->coef[j]->first);
                       cell<T> * cell1 = new cell(ce);
                       ce[j] = std::make_pair<T,T>((*it)->coef[j]->first,(*it)->coef[j]->second);
                       cell<T> * cell2 = new cell(ce);
                       ce[j] = buf;
                       this->elements[i-1].insert( cell1 );
                       this->elements[i-1].insert( cell2 );
                       (*it)->bound.push_back( cell1 );
                       (*it)->bound.push_back( cell2 );
                       cell1->coBound.push_back(*it);
                       cell2->coBound.push_back(*it);
                  }
             }
        }

        //construct the map by using a method of cell called value().For the elements with the same value search for all
        //repetitions. Once the repetitions a re find merge bd and cbd lists and remove the repetitions:
        //TODO...
        //mapa intowe w liste powinna tutaj rozwiaac ten problem!!!
        typename std::map< int , std::list<  std::set< cell<T>* >::iterator   >  > hash;

        //for every cell in the this->elements[i-1] set:
        for ( typename std::set< cell<T>* >::iterator it = this->elements[i-1].begin() ; it != this->elements[i-1].end() ; ++it )
        {
             typename std::map< int , std::list<cell<T>*> >::iterator aa = hash.find( (*it)->value() );
             if ( aa == hash.end() )
             {
                  std::list< std::set< cell<T>* >::iterator > li;
                  li.push_back(it);
                  hash.insert( std::make_pair< int , std::list<cell<T>*> >( (*it)->value() , li ) );
             }
             else
             {
                  aa->second.push_back( it );
             }
        }

        //in every element of the map remove the repetitions:
        for ( typename std::map< int , std::list<cell<T>*> >::iterator it = hash.begin() ; it != hash.end() ; ++it )
        {
             //search for repetitions in (*it)->second, I suppose this would be small, so do it in quadratic time.
             typename std::list<std::set<cell<T>*>::iterator>::iterator ne, ne1;
             for ( ne = it->second.begin() ; ne != it->second.end() ; ++ne )
             {
                  ne1 = ne;
                  ++ne1;
                  while ( ne1 != it->second.end() )
                  {
                        if ( ***ne1 == ***ne )
                        {
                            (**ne1)->del();//lazy delete
                            //tutaj jeszcze trzeba zmergowac kobrzegi!!!
                            std::list<cell<T>*> elem2add;
                            for ( std::list<cell<T>*>::iterator cbd = (**ne1)->cbdBegin() ; cbd != (**ne1)->cbdEnd() ; ++ cbd )
                            {
                                bool isThere = false;
                                for ( std::list<cell<T>*>::iterator cbd1 = (**ne)->cbdBegin() ; cbd1 != (**ne)->cbdEnd() ; ++ cbd1 )
                                {
                                    if ( **cbd1 == **cbd )
                                    {
                                        isThere = true;
                                        break;
                                    }
                                }
                                if ( !isThere )
                                {
                                    //add cbd to elem2add
                                    elem2add.push_back(*cbd);
                                }
                            }
                            for ( std::list<cell<T>*>::iterator cbd = elem2add->begin() ; cbd != elem2add->end() ; ++ cbd )
                            {
                                 (**ne)->coBound.push_back( *cbd );
                            }
                            ++ne1;
                        }
                        else
                        {
                            ++ne1;
                        }
                  }
             }
        }


        //to samo trzeba zrobic w brzegu kazdej i-wymiarowej komorki!!! ale tutaj zrobimy to kwadratowo, bo te brzegi nie powinny
        //byc duze:
        for ( typename std::set< cell<T>* >::iterator it = this->elements[i].begin() ; it != this->elements[i].end() ; ++it )
        {
            //remove repetitions in (*it)->bound
            for ( typename std::list<cell<T>*>::iterator bd = (*it)->bdBegin() ; bd != (*it)->bdEnd() ; ++bd )
            {
                 typename std::list<cell<T>*>::iterator bd1 = bd;
                 ++bd1;
                 while ( bd1 != (*it)->bdEnd() )
                 {
                     if ( **bd == **bd1 )
                     {
                          typename std::list<cell<T>*>::iterator buf = bd1;
                          ++bd1;
                          (*it)->coBound.erase(buf);
                     }
                     else
                     {
                         ++bd1;
                     }
                 }
            }
        }

        //teraz jeszcze przechodzimy po wymiarze i-1 i usowamy wszystkie elementy z znacznikeim delete ustawionym na true:
        typename std::set< cell<T>* >::iterator it = this->elements[i-1].begin()
        while ( it != this->elements[i-1].end() )
        {
            if ( (*it)->deleted() )
            {
                typename std::set< cell<T>* >::iterator buf = it;
                ++it;
                this->elements[i-1].erase(buf);
            }
            else
            {
                ++it;
            }
        }
    }//for ( int i = max_dim ; i != 0 ; --i )
    */
}//cellComplex( std::vector< cell<T>* > )


template <typename T>
cellComplex<T>::cellComplex( std::vector< std::pair<T,T> > cube )
{
    //first make sure that cube is sorted in the following way:
    //cube[i].first <= cube[i].second
    for ( unsigned int i = 0 ; i != cube.size() ; ++i )
    {
        if ( cube[i].first > cube[i].second )
        {
             T buf = cube[i].first;
             cube[i].first = cube[i].second;
             cube[i].second = buf;
        }
    }

    //we start from a given cube:
    cell<T>* initialCube = new cell<T>( cube );

    //compute the dimension of the cube:
    int dim = 0;
    for ( unsigned int i = 0 ; i != cube.size() ; ++i )
    {
        //if the interval is not denenerated, then increase the dimension:
        if ( (cube[i].first-cube[i].second) != 0 )
        {
             ++dim;
        }
    }

    for ( int i = 0 ; i <= dim ; ++i )
    {
        std::set< cell<T>* > zbi;
        this->elements.push_back( zbi );
    }

    this->elements[dim].insert(initialCube);

    //for every dimension:
    for ( int i = dim-1 ; i >= 0 ; --i )
    {
        //for every cube in this dimension:
        for ( typename std::set<cell<T>*>::iterator li = this->elements[i+1].begin() ; li != this->elements[i+1].end() ; ++li )
        {
            //take a cube *li and generate all its boundary elements:
            for ( unsigned int num = 0 ; num != (*li)->coef.size() ; ++num )
            {
                //if (*li)->coef[num] is non degenerated:
                if ( (*li)->coef[num].first != (*li)->coef[num].second )
                {
                     //create 2 cells of dimension (i-1)
                     std::vector< std::pair<T,T> > first, second;
                     for ( unsigned int j = 0 ; j != (*li)->coef.size() ; ++j )
                     {
                         if ( num != j )
                         {
                             first.push_back( std::make_pair( (*li)->coef[j].first , (*li)->coef[j].second ) );
                             second.push_back( std::make_pair( (*li)->coef[j].first , (*li)->coef[j].second ) );
                         }
                         else
                         {
                             first.push_back( std::make_pair( (*li)->coef[j].first , (*li)->coef[j].first ) );
                             second.push_back( std::make_pair( (*li)->coef[j].second , (*li)->coef[j].second ) );
                         }
                     }

                     cell<T>* firstC = new cell<T>( first );
                     cell<T>* secondC = new cell<T>( second );

                     //set the boundary and coboundary elements:
                     (*li)->bound.push_back( firstC );
                     (*li)->bound.push_back( secondC );
                     firstC->coBound.push_back( *li );
                     secondC->coBound.push_back( *li );

                     //put firstC and secondC to the list vectorOfLists[i-1]
                     this->elements[i].insert( firstC );
                     this->elements[i].insert( secondC );
                }
            }
        }

        //remove all the repetitions in vectorOfLists[i-1] list and merge coboundary lists
        //tehere are just two repetitions of each cell in the list.
        typename std::set<cell<T>*>::iterator li = this->elements[i].begin();
        while ( li != this->elements[i].end() )
        {
              typename std::set<cell<T>*>::iterator li1 = li;
              ++li1;
              while ( li1 != this->elements[i].end() )
              {
                   if ( (**li) == (**li1) )
                   {
                        //join the lists of coboundary elements of *li1 to li
                        for ( typename cell<T>::CbdIterator cbd = (*li1)->cbdBegin() ; cbd != (*li1)->cbdEnd() ; ++cbd )
                        {
                            (*li)->coBound.push_back( *cbd );
                        }
                        typename std::set<cell<T>*>::iterator toErase = li1;
                        ++li1;

                        //tutaj jeszcze trzeba w kobrzegu toErase ustawic odpowiednio wskazniki!!!
                        for ( typename cell<T>::CbdIterator cbd = (*toErase)->cbdBegin() ; cbd != (*toErase)->cbdEnd() ; ++cbd )
                        {
                            //find toErase in (*cbd)->bound list:
                            typename cell<T>::BdIterator bd = (*cbd)->bdBegin();
                            while ( bd != (*cbd)->bdEnd() )
                            {
                                  if ( *bd == *toErase )
                                  {
                                        (*cbd)->bound.erase(bd);
                                        break;
                                  }
                                  ++bd;
                            }
                            (*cbd)->bound.push_back( *li );
                        }
                        this->elements[i].erase( toErase );
                   }
                   else
                   {
                        ++li1;
                   }
              }
              ++li;
        }//while ( li != this->elements[i].end() )
    }
}//cellComplex<T>::cellComplex( std::vector< std::pair<T,T> > cube )



bool divideTheCubeByGivenFactorDBG = false;
template <typename T>
/*typename std::vector< cell<T>* >*/void cellComplex<T>::divideTheCubeByGivenFactor( cell<T>* cub , int n )
{
     std::list< cell<T>* > cubes2subdiv;
     cubes2subdiv.push_back( cub );

     std::vector< std::pair<T,T> > coefCords;
     for ( unsigned int i = 0 ; i != cub->coef.size() ; ++i )
     {
         coefCords.push_back( std::make_pair( cub->coef[i].first , cub->coef[i].second ) );
     }



     //in each dimension divide to n cubes:
     for ( unsigned int i = 0 ; i != coefCords.size() ; ++i )
     {
         if ( divideTheCubeByGivenFactorDBG )
         {
             std::cerr << "i: " << i << "\n";
         }

         //if only cube is not degenerated in this dimension:
         if ( coefCords[i].first != coefCords[i].second )
         {
              std::list< cell<T>* > newCubes;

              if ( divideTheCubeByGivenFactorDBG )
              {
                  std::cerr << "cubes2subdiv : \n";
                  for ( typename std::list< cell<T>* >::iterator it = cubes2subdiv.begin() ; it != cubes2subdiv.end() ; ++it )
                  {
                      std::cerr << **it << ", ";
                  }
                  std::cerr << "\n\n\n";
              }

              for ( int div = 1 ; div != n ; ++div )
              {
                  if ( divideTheCubeByGivenFactorDBG )
                  {
                      std::cerr << "div : " << div << "\n";
                  }

                  std::list< cell<T>* > aa;

                  //divide every cube in cubes2subdiv in cub->coef[div].first+(cub->coef[div].second - cub->coef[div].first)*div/n
                  for ( typename std::list< cell<T>* >::iterator it = cubes2subdiv.begin() ; it != cubes2subdiv.end() ; ++it )
                  {
                      //and add the new elements to aa and newCubes list respectivelly
                      if ( divideTheCubeByGivenFactorDBG )
                      {
                          std::cerr << "punkt podzialu : " << (coefCords[i].first+(coefCords[i].second - coefCords[i].first)*div/n) << "\n";
                          std::cerr << "dziele : " << **it << "\n";
                      }

                      std::vector< cell<T>* > vect = this->divideCube( (*it) , coefCords[i].first+(coefCords[i].second - coefCords[i].first)*div/n , i );
                      if ( div != (n-1) )
                      {
                          newCubes.push_back(vect[0]);
                          aa.push_back( vect[1] );
                      }
                      else
                      {
                          newCubes.push_back(vect[0]);newCubes.push_back(vect[1]);
                      }

                      if ( divideTheCubeByGivenFactorDBG )
                      {
                          std::cerr << "Powstale kosttki : " << *vect[0] << "\n";
                          std::cerr << "Do aa wrzucam  : " << *vect[1] << "\n";
                      }
                  }
                  cubes2subdiv.clear();
                  for ( typename std::list< cell<T>* >::iterator bla = aa.begin() ; bla != aa.end() ; ++bla )
                  {
                      cubes2subdiv.push_back(*bla);
                  }
              }
              cubes2subdiv.clear();
              //add elements from newCubes to cubes2subdiv lists
              for ( typename std::list< cell<T>* >::iterator it = newCubes.begin() ; it != newCubes.end() ; ++it )
              {
                  cubes2subdiv.push_back( *it );
              }
         }
     }
}

template <typename T>
bool containedInBdry( const std::vector< std::pair<T,T> > coface , const std::vector< std::pair<T,T> > face )
{
     if ( coface.size() == face.size() )
     {
          for ( unsigned int i = 0 ; i != coface.size() ; ++i )
          {
              //if face[i] \subset coface[i]
              if ( (face[i].first < coface[i].first)||(face[i].first>coface[i].second) )
              {
                   return false;
              }
              if ( (face[i].second < coface[i].first)||(face[i].second>coface[i].second) )
              {
                   return false;
              }
          }
          return true;
     }
     return false;
}//containedInBdry

template <typename T>
void cellComplex<T>::tie( cell<T>* coface, cell<T>* face )
{
     coface->bound.push_back( face );
     face->coBound.push_back( coface );
}

bool debugeDivCube = false;
template <typename T>
typename std::vector< cell<T>* > cellComplex<T>::divideCube( cell<T>* cube , T x , int coef )
{
      std::vector< cell<T>* > result;

      //if cube has zero thickness in the direction of coef, then there is nothink to subdivide:
      if ( cube->coef[coef].first == cube->coef[coef].second )
      {
           return result;
      }

      //if x \not \in [cube->coef[coef].first , cube->coef[coef].second], then there is nothink to subdivide:
      if ( (x <= cube->coef[coef].first) || ( x>=cube->coef[coef].second ) )
      {
           return result;
      }

      //Here I should create tree new cells. A and B should have the same dimension as cube, and C should
      //have dim(cube)-1. Then after all the changes in the structure of boundary and coboundary I should
      //put A, B and C to the complex and remove cube from the complex.
      //However removing cube may be costly, therefore the following trick is used instead:
      //cube will not be removed, it will be changed to A. Then only B and C needs to be created and added
      //to the complex.

      //first we save the original boundary and coboundary of cube:
      std::vector< cell<T>* > boundaryOfCube;
      std::vector< cell<T>* > coboundaryOfCube;

      for ( typename cell<T>::BdIterator bd = cube->bdBegin() ; bd != cube->bdEnd() ; ++bd )
      {
          boundaryOfCube.push_back( *bd );
      }
      for ( typename cell<T>::CbdIterator cbd = cube->cbdBegin() ; cbd != cube->cbdEnd() ; ++cbd )
      {
          coboundaryOfCube.push_back( *cbd );
      }


      //now we create A, B and C:
      std::vector< std::pair<T,T> > CCoef;
      for ( unsigned int i = 0 ; i != cube->coef.size() ; ++i )
      {
          if ( i != coef )
          {
              CCoef.push_back( std::make_pair( cube->coef[i].first , cube->coef[i].second ) );
          }
          else
          {
              CCoef.push_back( std::make_pair( x , x ) );
          }
      }

      std::vector< std::pair<T,T> > BCoef;
      for ( unsigned int i = 0 ; i != cube->coef.size() ; ++i )
      {
          if ( i != coef )
          {
              BCoef.push_back( std::make_pair( cube->coef[i].first , cube->coef[i].second ) );
          }
          else
          {
              BCoef.push_back( std::make_pair( x , cube->coef[i].second ) );
          }
      }

      std::vector< std::pair<T,T> > ACoef;
      for ( unsigned int i = 0 ; i != cube->coef.size() ; ++i )
      {
          if ( i != coef )
          {
              ACoef.push_back( std::make_pair( cube->coef[i].first , cube->coef[i].second ) );
          }
          else
          {
              ACoef.push_back( std::make_pair( cube->coef[i].first , x ) );
          }
      }
      //create A, B and C:
      cell<T>* A = new cell<T>(ACoef);
      cell<T>* B = new cell<T>(BCoef);
      cell<T>* C = new cell<T>(CCoef);
      result.push_back(A);
      result.push_back(B);
      result.push_back(C);

      if ( debugeDivCube )
      {
          std::cerr << "A, B i C utworzone \n";
          std::cerr << "A  : " << *A << "\n";
          std::cerr << "B  : " << *B << "\n";
          std::cerr << "C  : " << *C << "\n";
      }

      //remove cube from the complex:
      int cubeDim = cube->dim();
      this->elements[ cubeDim ].erase( this->elements[ cubeDim ].find(cube) );

      if ( debugeDivCube )
      {
          std::cerr << "Wywalam : " << *cube << " z listy o wymiarze : " << cubeDim << "\n";
      }

      //add A, B and C to complex:
      this->elements[ cubeDim ].insert( A );
      this->elements[ cubeDim ].insert( B );
      this->elements[ cubeDim-1 ].insert( C );

      if ( debugeDivCube )
      {
          std::cerr << "Wsadzenie A, B, C i usuniecie cube z kompleksu przeprowadzone \n";
      }

      //Put C to the boundary of cube and B. Put cube and B to coboundary of C:
      A->bound.push_back( C );
      B->bound.push_back( C );
      C->coBound.push_back( A );
      C->coBound.push_back( B );


      //let us now iterate through the coboundary of cube and for each coboundary element E substitute
      //cube to A and B in E.boundary, and add E to A.coBound and B.cobound:
      for ( typename std::vector<cell<T>*>::iterator gamma = coboundaryOfCube.begin() ; gamma != coboundaryOfCube.end() ; ++gamma )
      {
          if ( debugeDivCube )
          {
              std::cerr << "Idziemy po kobrzegach i wszystko ustawiamy jak trzeba : " << **gamma << "\n";
          }

          typename cell<T>::BdIterator bd = (*gamma)->bdBegin();
          while ( bd != (*gamma)->bdEnd() )
          {
                if ( *bd == cube )
                {
                     break;
                }
                ++bd;
          }
          (*gamma)->bound.erase( bd );

          if ( debugeDivCube )
          {
              std::cerr << "AAAWywalam z brzegu : " << **gamma << " kostke : " << **bd << "\n";
          }

          //put A and B to boundary of (*gamma):
          if ( debugeDivCube )
          {
              std::cerr << "Dodaje : " << *A << " i " << *B << " do brzegu " << **gamma << "\n";
          }

          (*gamma)->bound.push_back(A);
          (*gamma)->bound.push_back(B);

          if ( debugeDivCube )
          {
              std::cerr << "Dodaje : " << **gamma << " od kobrzegu " << *A << " i " << *B << "\n";
          }
          //put (*gamma) to coboundary of A and B:
          A->coBound.push_back( *gamma );
          B->coBound.push_back( *gamma );
      }

      //now let us iterate through the boundaryOfCube and let us remove cube from the coboundary of beta:
      for ( typename std::vector<cell<T>*>::iterator beta = boundaryOfCube.begin() ; beta != boundaryOfCube.end() ; ++beta )
      {

          if ( debugeDivCube )
          {
              std::cerr << "Patrze na kobrzeg : "<< **beta << "\n";
              std::cerr << "(*beta)->coBound.size() : " << (*beta)->coBound.size() << "\n";
          }

          typename cell<T>::CbdIterator cbd = (*beta)->cbdBegin();
          while ( cbd != (*beta)->cbdEnd() )
          {
                if ( (*cbd) == cube )
                {
                     break;
                }
                ++cbd;
          }

          if ( debugeDivCube )
          {
              std::cerr << "Przed skasowaniem : \n";
              for ( typename cell<T>::CbdIterator gg = (*beta)->cbdBegin(); gg != (*beta)->cbdEnd(); ++gg )
              {
                  std::cerr << **gg << " , ";
              }
              std::cerr << "\n Wywalam : " << **cbd << " z kobregu : " << **beta << "\n";
          }

          (*beta)->coBound.erase( cbd );

          if ( debugeDivCube )
          {
              std::cerr << "po \n";
              for ( typename cell<T>::CbdIterator gg = (*beta)->cbdBegin(); gg != (*beta)->cbdEnd(); ++gg )
              {
                  std::cerr << **gg << " , ";
              }
              std::cerr << "\n";
//              getchar();
          }

      }

      //let us now fulfill the bdryElementsToSubdiv list and set the boundary elements of A and B:
      for ( typename std::vector<cell<T>*>::iterator beta = boundaryOfCube.begin() ; beta != boundaryOfCube.end() ; ++beta )
      {
          if ( debugeDivCube )
          {
              std::cerr << "idziemy po brzegu orginalnej kostki, jestesy na : " << **beta << "\n";
          }

          //first check if there is no element in the boundary of (*beta) that can be direcly
          //used as a boundary of C:
          for ( typename cell<T>::BdIterator bd = (*beta)->bdBegin()  ; bd != (*beta)->bdEnd() ; ++bd )
          {
              //if (*bd) is not already in the boundary of C:
              bool isBdAlreadyInBoundaryOfC = false;
              for ( typename cell<T>::BdIterator bdOfC = C->bdBegin() ; bdOfC != C->bdEnd() ; ++bdOfC )
              {
                  if ( (*bdOfC) == (*bd) )
                  {
                       isBdAlreadyInBoundaryOfC = true;
                  }
              }
              if ( isBdAlreadyInBoundaryOfC ){continue;}

              if ( containedInBdry( C->coef , (*bd)->coef ) )
              {
                   if ( debugeDivCube )
                   {
                       std::cerr << "tie(" <<  *C  << " , " <<  (**bd) << ") : \n";
                   }
                   tie( C , (*bd) );
              }
          }


          //if *beta is in the boundary of only cube or only B, then add it to suitable boundary lists
          //and modify (*beta)->coBound.
          if ( containedInBdry( A->coef , (*beta)->coef ) )
          {
               if ( debugeDivCube )
               {
                   std::cerr << "tie ( " << *A << " , "<<**beta << " ) \n";
               }
               tie( A , *beta );
          }
          else
          {
              if ( containedInBdry( B->coef , (*beta)->coef ) )
              {
                   if ( debugeDivCube )
                   {
                        std::cerr << "tie ( " << *B << " , "<<**beta << " ) \n";
                   }
                   tie( B , (*beta) );
              }
              else
              {
                  typename std::vector< cell<T>* > faceSubdivision = divideCube( (*beta) , x , coef );
                  if ( faceSubdivision.size() )
                  {
                       int currentDim = A->dim();
                       for ( unsigned int i = 0 ; i != faceSubdivision.size() ; ++i )
                       {
                           if ( faceSubdivision[i]->dim() == (currentDim-1) )
                           {
                               if ( containedInBdry( A->coef , faceSubdivision[i]->coef ) )
                               {
                                   if ( debugeDivCube )
                                   {
                                       std::cerr << " tie ( " << *A << " , " << *faceSubdivision[i] << "\n";
                                   }
                                   tie( A , faceSubdivision[i] );
                               }
                               else
                               {
                                   //containedInBdry( B->coef , faceSubdivision[i]->coef ) == true
                                   if ( debugeDivCube )
                                   {
                                       std::cerr << " tie ( " << *B << " , " << *faceSubdivision[i] << "\n";
                                   }
                                   tie( B , faceSubdivision[i] );
                               }
                           }
                           else
                           {
                               //faceSubdivision[i]->dim() == (currentDim-2)
                               if ( debugeDivCube )
                               {
                                   std::cerr << " tie ( " << *C << " , " << *faceSubdivision[i] << "\n";
                               }
                               tie( C , faceSubdivision[i] );
                           }
                       }
                  }
              }
          }
      }

      delete cube;

      if ( debugeDivCube )
      {
          std::cerr << "Wychodzimy z procedury \n";
      }


      return result;
}//divideCube
