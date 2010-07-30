#include<iostream>
#include<list>
#include<vector>
#include<set>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/mpl/if.hpp>
using namespace std;


template <typename T>
class cellComplex
{
    public:
     typedef int Color;
      typedef int Dim;
      typedef int Id;
    public:

    template<typename CellImplT>
    class CellProxy {
    protected:
      mutable CellImplT* impl;
    public:
      typedef int Color;
      typedef int Dim;
      typedef int Id;

      typedef CellImplT Impl;

      CellProxy(CellImplT* _impl): impl(_impl) {}
      CellProxy(const CellImplT* const _impl): impl(const_cast<CellImplT*>(_impl)) {}

      CellProxy(const CellProxy& other) : impl(const_cast<CellProxy&>(other).impl){}

      CellProxy(cellComplex &c) : impl(0) {}

      Id getId() const
      {
         return impl->getId();
      }

      Color getColor() const{
         return impl->color();
      }

      template<Color color>
      void setColor() const {
         impl->color() = color;
      }

      void setColor(const Color& color) const {
         impl->color() = color;
      }

      Dim getDim() const {
         return impl->dim();
      }

      bool operator<(const CellProxy& b) const {
         return impl < b.impl;
      }

      CellImplT* getImpl() const {
         return const_cast<CellProxy*>(this)->impl;
      }
    };//CellProxy



    typedef CellProxy< cell<T> > Cell;

    int coincidenceIndex(const Cell& f,const Cell& s ) const
    {
        return computeIncidence<T>(f.getImpl() , s.getImpl() );
    }

    int getMaxDim()const
    {
        return (int)(this->elements.size())-1;
    }

    int getDim()const
    {
        return (int)(this->elements.size())-1;
    }

    class ToCell : public unary_function<const cell<T>* const, Cell>
    {
    public:
         Cell operator()(const cell<T> * const s) const
         {
              return Cell(s);
         }
    };//ToCell

    struct Iterators
    {
        const int color;
        cellComplex &comp;

        Iterators(cellComplex &cmplx, int c = -1) : color(c), comp(cmplx){}

        struct DimCells
        {
            typedef boost::transform_iterator<ToCell, typename cellComplex<T>::dimIterator> iterator;
            typedef boost::transform_iterator<ToCell, typename cellComplex<T>::constDimIterator> const_iterator;

            cellComplex &comp;
            int color;
            int dim;

            explicit DimCells(cellComplex &cmplx, int col, int d) : comp(cmplx), color(col), dim(d) {}

            iterator begin() const
            {
                return iterator(comp.dimIteratorBegin(dim), ToCell());
            }

            iterator end() const
            {
                return iterator(comp.dimIteratorEnd(dim), ToCell());
            }
        };

        DimCells dimCells(int dim)
        {
            return DimCells(comp, color, dim);
        }

        struct BdCells
        {
            typedef typename cell<T>::BdIterator inner_iterator;
            typedef typename cell<T>::constBdIterator const_inner_iterator; //tu moze sie sypac

            typedef boost::transform_iterator<ToCell, inner_iterator> iterator;
            typedef boost::transform_iterator<ToCell, const_inner_iterator> const_iterator;
            cellComplex &comp;
            const cell<T> &cellref; // ?? really const ??
            int color;

            explicit BdCells(cellComplex &cmplx, const cell<T> &c, int col) : comp(cmplx), cellref(c), color(col) {}

            iterator begin()
            {
                return iterator(const_cast<cell<T>&>(cellref).bdBegin(), ToCell());
            }

            iterator end()
            {
                return iterator(const_cast<cell<T>&>(cellref).bdEnd(), ToCell());
            }

            const_iterator begin() const
            {
                return const_iterator(cellref.constBdBegin(), ToCell());
            }

            const_iterator end() const
            {
                return const_iterator(cellref.constBdEnd(), ToCell());
            }
        };


        template<typename ImplT>
        BdCells bdCells(const CellProxy<ImplT> &c)
        {
            return BdCells(comp, *c.getImpl(), color);
        }

        struct AllCells
        {
            typedef typename cellComplex<T>::allCells innerIterator;

            typedef boost::transform_iterator<ToCell, innerIterator> iterator;
            typedef boost::transform_iterator<ToCell, typename cellComplex<T>::constAllCells> const_iterator;

            cellComplex &comp;
            int color;

            explicit AllCells(cellComplex &cmplx, int col) : comp(cmplx), color(col) {}

            iterator begin()
            {
                return iterator(comp.allCellsBegin(), ToCell());
            }

            iterator end()
            {
                return iterator(comp.allCellsEnd(), ToCell());
            }

            const_iterator begin() const
            {
                return const_iterator(comp.constAllCellsBegin(), ToCell());
            }

            const_iterator end() const
            {
                return const_iterator(comp.constAllCellsEnd(), ToCell());
            }
        };

        AllCells allCells()
        {
            return AllCells(comp, color);
        }

        struct CbdCells
        {
            typedef typename cell<T>::cbdIterator inner_iterator;
            typedef typename cell<T>::cconstCbdIterator const_inner_iterator;

            typedef boost::transform_iterator<ToCell, inner_iterator> iterator;
            typedef boost::transform_iterator<ToCell, const_inner_iterator> const_iterator;

            cellComplex &comp;
            const cell<T> &cellref; // ?? really const ??
            int color;

            explicit CbdCells(cellComplex &cmplx, const cell<T> &c, int col) : comp(cmplx), cellref(c), color(col) {}

            iterator begin()
            {
                return iterator(const_cast<cell<T>&>(cellref).cbdBegin(), ToCell());
            }

            iterator end()
            {
                return iterator(const_cast<cell<T>&>(cellref).cbdEnd(), ToCell());
            }

            const_iterator begin() const
            {
                return const_iterator(const_cast<cell<T>&>(cellref).constCbdBegin(), ToCell());
            }

            const_iterator end() const
            {
                return const_iterator(const_cast<cell<T>&>(cellref).constCbdEnd(), ToCell());
            }
        };

        template<typename ImplT>
        CbdCells cbdCells(const CellProxy<ImplT> &c)
        {
            return CbdCells(comp, *c.getImpl(), color);
        }

    };//struct iterators



//********************************************************************************
//kolorowe iteratory
struct ColoredIterators {
    struct Iterators
    {
        const int color;
        cellComplex &comp;

        Iterators(cellComplex &cmplx, int c) : color(c), comp(cmplx){}


        struct DimCells
        {
            typedef boost::transform_iterator<ToCell, typename cellComplex<T>::dimColoredCells> iterator;
            typedef boost::transform_iterator<ToCell, typename cellComplex<T>::constDimColoredCells> const_iterator;
            cellComplex &comp;
            int color;
            int dim;

            explicit DimCells(cellComplex &cmplx, int col, int d) : comp(cmplx), color(col), dim(d) {}

            iterator begin()
            {
                return iterator(comp.dimColoredCellsBegin(dim,color), ToCell());
            }

            iterator end()
            {
                return iterator(comp.dimColoredCellsEnd(dim,color), ToCell());
            }
        };

        DimCells dimCells(int dim)
        {
            return DimCells(comp, color, dim);
        }

        struct BdCells
        {
            typedef typename cell<T>::coloredBdIterator inner_iterator;
            typedef typename cell<T>::constColoredBdIterator const_inner_iterator; //tu moze sie sypac

            typedef boost::transform_iterator<ToCell, inner_iterator> iterator;
            typedef boost::transform_iterator<ToCell, const_inner_iterator> const_iterator;
            cellComplex &comp;
            const cell<T> &cellref;
            int color;

            explicit BdCells(cellComplex &cmplx, const cell<T> &c, int col) : comp(cmplx), cellref(c), color(col) {}
            //explicit BdCells(){}//AAA

            iterator begin()
            {
                return iterator(const_cast<cell<T>&>(cellref).coloredBdBegin(color), ToCell());
            }

            iterator end()
            {
                return iterator(const_cast<cell<T>&>(cellref).coloredBdEnd(color), ToCell());
            }

            const_iterator begin() const
            {
                return const_iterator(cellref.constColoredBdBegin(color), ToCell());
            }

            const_iterator end() const
            {
                return const_iterator(cellref.constColoredBdEnd(color), ToCell());
            }
        };


        template<typename ImplT>
        BdCells bdCells(const CellProxy<ImplT> &c)
        {
            return BdCells(comp, *c.getImpl(), color);
        }

        struct AllCells
        {

            typedef typename cellComplex<T>::coloredCells innerIterator;

            typedef boost::transform_iterator<ToCell, innerIterator> iterator;

            cellComplex &comp;
            int color;


            explicit AllCells(cellComplex &cmplx, int col) : comp(cmplx), color(col){}

            iterator begin()
            {
                return iterator(comp.coloredCellsBegin(this->color), ToCell());
            }

            iterator end()
            {
                return iterator(comp.coloredCellsEnd(this->color), ToCell());
            }
        };

        AllCells allCells()
        {
            return AllCells(comp, this->color);
        }


        struct CbdCells
        {
            typedef typename cell<T>::coloredCbdIterator inner_iterator;
            typedef typename cell<T>::constColoredCbdIterator const_inner_iterator;

            typedef boost::transform_iterator<ToCell, inner_iterator> iterator;
            typedef boost::transform_iterator<ToCell, const_inner_iterator> const_iterator;

            cellComplex &comp;
            const cell<T> &cellref; // ?? really const ??
            int color;

            explicit CbdCells(cellComplex &cmplx, const cell<T> &c, int col) : comp(cmplx), cellref(c), color(col) {}
            //explicit CbdCells(){}///AAA

            iterator begin()
            {
                return iterator(const_cast<cell<T>&>(cellref).coloredCbdBegin(color), ToCell());
            }

            iterator end()
            {
                return iterator(const_cast<cell<T>&>(cellref).coloredCbdEnd(color), ToCell());
            }

            const_iterator begin() const
            {
                //return cell.coborder_begin(color);
                return const_iterator(const_cast<cell<T>&>(cellref).constColoredCbdBegin(color), ToCell());
            }

            const_iterator end() const
            {
                //return cell.coborder_end(color);
                return const_iterator(const_cast<cell<T>&>(cellref).constColoredCbdEnd(color), ToCell());
            }
        };

        template<typename ImplT>
        CbdCells cbdCells(const CellProxy<ImplT> &c)
        {
            return CbdCells(comp, *c.getImpl(), color);
        }

    };//struct Iterators
}; //ColoredIterators

    Iterators iterators()
    {
        return Iterators(*this);
    }

    typedef typename ColoredIterators::Iterators CIterators;

    CIterators iterators(int color)
    {
        return typename ColoredIterators::Iterators(*this,color);
    }


//********************************************************************************




       int cardinality() const
       {
           int res = 0;
           for ( unsigned int i = 0 ; i != this->elements.size() ; ++i )
           {
               for ( typename std::set< cell<T>* >::iterator it = this->elements[i].begin() ; it != this->elements[i].end() ; ++it )
               {
                   if ( (*it)->color() == 1 )++res;
               }
           }
           return res;
       }

       int maxId() const
       {
           int res = 0;
           for ( unsigned int i = 0 ; i != this->elements.size() ; ++i )
           {
               for ( typename std::set< cell<T>* >::iterator it = this->elements[i].begin() ; it != this->elements[i].end() ; ++it )
               {
                   res = max(res, (*it)->getId());
               }
           }
           return res + 1;
       }

       int size() const
       {
           int res = 0;
           for ( unsigned int i = 0 ; i != this->elements.size() ; ++i )
           {
               res += this->elements[i].size();
           }
           return res;
       }

       cellComplex();
       cellComplex( std::vector< std::pair<T,T> > cube);

       //construction from the maximal dimensional cells
       cellComplex( std::vector< cell<T>* > );


       //for division of the cube. Returns true, if the division was succesfull.
       typename std::vector< cell<T>* > divideCube( cell<T>* cube , T x , int cord );


       ~cellComplex()
       {
            for ( unsigned int i = 0 ; i != this->elements.size() ; ++i )
            {
                typename std::set< cell<T>* >::iterator it;
                for ( it = this->elements[i].begin() ; it != this->elements[i].end() ; ++it )
                {
                    delete (*it);
                }
            }
       }


        template <class K>
        friend std::ostream& operator<<(std::ostream& out, const cellComplex<K>& cmplx)
        {
             for ( unsigned int i = 0 ; i < cmplx.elements.size() ; ++i )
             {
                 out << "\n dimension : " << i <<"\n";
                 typename std::set< cell<K>* >::iterator it;

                 for ( it = cmplx.elements[i].begin() ; it != cmplx.elements[i].end() ; ++it )
                 {
                     out << "\n" ;
                     out << "element: "<< (*(*it));
                     out <<"\n";
                 }
             }
             return out;
        }

        typename std::vector< std::set< cell<T>* > >& elemen(){return this->elements;}

        typename std::vector< cell<T>* > cubesInTheComplex()
        {
            std::vector< cell<T>* > result;
            for ( typename std::list< cell<T>* >::iterator it = this->elements[2].begin() ; it != this->elements[2].end() ; ++it )
            {
                if ( !(*it)->deleted() )
                {
                     result.push_back( *it );
                }
            }
            return result;
        };

        //this procedure is used to divide the cube 'cub' to n*n cubes having the same size
        void divideTheCubeByGivenFactor( cell<T>* cub , int n );

        int dim(){return this->elements.size();};



       //my implementation of iterators

       typedef typename std::set< cell<T>* >::iterator dimIterator;
       dimIterator dimIteratorBegin(int i){return this->elements[i].begin();};
       dimIterator dimIteratorEnd(int i){return this->elements[i].end();};


        template<bool constIterators>
        class allCellsBase : public iterator<bidirectional_iterator_tag, cell<T>* >
        {
        protected:
            friend class cellComplex<T>;
            const cellComplex* const cel;
            int gradationLevel;
            typename boost::mpl::if_c<constIterators,
                                      typename std::set< cell<T>* >::const_iterator,
                                      typename std::set< cell<T>* >::iterator
                                      >::type pos;
        public:
            explicit allCellsBase(const cellComplex* const cmplx , int gradationLevel):cel(cmplx),gradationLevel(gradationLevel)
            {
                this->pos = cmplx->elements[0].begin();
            }
            explicit allCellsBase(const cellComplex* const cmplx):cel(cmplx),gradationLevel(0)
            {
                this->pos = cmplx->elements[0].begin();
            }
            explicit allCellsBase(){}


            allCellsBase& operator++()
            {
                 ++this->pos;
                 if ( this->pos == this->cel->elements[ this->gradationLevel ].end() )
                 {
                     if ( this->gradationLevel != this->cel->elements.size()-1 )
                     {
                          ++this->gradationLevel;
                          this->pos = this->cel->elements[this->gradationLevel].begin();
                     }
                 }
                 return *this;
            }//operator++


            allCellsBase operator++(int)
            {
                ++this->pos;
                 if ( this->pos == this->cel->elements[ this->gradationLevel ].end() )
                 {
                     if ( this->gradationLevel != this->cel->elements.size()-1 )
                     {
                          ++this->gradationLevel;
                          this->pos = this->cel->elements[this->gradationLevel].begin();
                     }
                 }
                return *this;
            }


            allCellsBase& operator--()
            {
                if (this->pos != this->cel->elements[this->gradationLevel].begin())
                {
                      --this->pos;
                }
                else
                {
                    if ( this->gradationLevel )
                    {
                         --this->gradationLevel;
                         this->pos = this->cel->elements[this->gradationLevel].end();
                         --this->pos;
                    }
                }
                return *this;
            }

            allCellsBase operator--(int)
            {
                //std::cerr << "operator--(int) \n";
                if (this->pos != this->cel->elements[this->gradationLevel].begin())
                {
                      --this->pos;
                }
                else
                {
                    if ( this->gradationLevel != 0 )
                    {
                         --this->gradationLevel;
                         this->pos = this->cel->elements[this->gradationLevel].end();
                         --this->pos;
                    }
                }
                return *this;
            }

            cell<T>* operator*()
            {
                  if ( this->pos != this->cel->elements[this->gradationLevel].end() )
                  {
                       return *(this->pos);
                  }
                  throw "Iterator out of range";
            }

            cell<T>* operator->() const
            {
                  if ( this->pos != this->cel->elements[this->gragationLevel].end() )
                  {
                       return *this->pos;
                  }
                  throw "Iterator out of range";
             }

             bool operator==(const allCellsBase& it) const
             {
                  if ( this->gradationLevel == it.gradationLevel )
                  {
                       //std::cerr << "W porownaniau zgadzaja sie poziomy gradacji \n";
                       if ( this->pos == it.pos )
                       {
                            //std::cerr << "W [porownaniu zgadzaja sie iteratory \n";
                            return true;
                       }
                  }
                  return false;
             }

             bool operator!=(const allCellsBase& it) const
             {
                  return ( !((*this)==it) );
             }

        };//allCells

        typedef allCellsBase<false> allCells;

        allCells allCellsBegin()
        {
             allCells res(this,0);
             res.pos = this->elements[0].begin();
             return res;
        }//allCellsBegin()

        allCells allCellsEnd()
        {
             allCells res(this, this->elements.size()-1 );
             res.pos = this->elements[this->elements.size()-1].end();
             return res;
        }//allCellsEnd()



        class constAllCells : public allCellsBase<true>
        {
        public:
            explicit constAllCells(const cellComplex* const cmplx , int gradationLevel):allCellsBase<true>(cmplx,gradationLevel)
            {
                this->pos = cmplx->elements[0].begin();
            }
            explicit constAllCells(const cellComplex* const cmplx):allCellsBase<true>(cmplx)
            {
                this->pos = cmplx->elements[0].begin();
            }

            const cell<T>* operator*() const
            {
                  if ( this->pos != this->cel->elements[this->gradationLevel].end() )
                  {
                       return *(this->pos);
                  }
                  throw "Iterator out of range";
            }

            const cell<T>* operator->() const
            {
                  if ( this->pos != this->cel->elements[this->gragationLevel].end() )
                  {
                       return *this->pos;
                  }
                  throw "Iterator out of range";
             }

        };//allCells
        constAllCells constAllCellsBegin() const
        {
             constAllCells res(this,0);
             res.pos = this->elements[0].begin();
             return res;
        }//allCellsBegin()

        constAllCells constAllCellsEnd() const
        {
             constAllCells res(this, this->elements.size()-1 );
             res.pos = this->elements[this->elements.size()-1].end();
             return res;
        }//allCellsEnd()




        class coloredCells
        {
        protected:
            friend class cellComplex<T>;
            cellComplex* cel;
            int gradationLevel;
            typename std::set< cell<T>* >::iterator pos;
            int color;
        public:
            explicit coloredCells(cellComplex* cmplx , int gradationLevel, int color):cel(cmplx),gradationLevel(gradationLevel), color(color)
            {
                this->pos = cmplx->elements[0].begin();
            }
            explicit coloredCells(cellComplex* cmplx, int color):cel(cmplx),gradationLevel(0),color(color)
            {
                this->pos = cmplx->elements[0].begin();
            }
            explicit coloredCells(){}


            coloredCells& operator++()
            {
                 if (this->pos != this->cel->elements[ this->gradationLevel ].end())
                 {
                       ++this->pos;
                 }

                 while ( true )
                 {
                     if ( (this->pos!=this->cel->elements[ this->gradationLevel  ].end())  )
                     {
                          if ((*this->pos)->color() != this->color)
                          {
                              ++this->pos;
                          }
                          else
                          {
                              break;
                          }
                     }
                     else
                     {
                         if (this->pos == this->cel->elements[ this->cel->elements.size()-1  ].end())
                         {
                             break;
                         }
                     }

                     if ( this->pos == this->cel->elements[ this->gradationLevel ].end() )
                     {
                         if ( this->gradationLevel != this->cel->elements.size()-1 )
                         {
                              ++this->gradationLevel;
                              this->pos = this->cel->elements[this->gradationLevel].begin();
                         }
                     }
                 }
                 return *this;
            }//operator++


            coloredCells operator++(int)
            {
                if ((this->pos != this->cel->elements[ this->gradationLevel ].end())) ++this->pos;
                while ( true )
                {
                    if ( (this->pos != this->cel->elements[ this->gradationLevel ].end()) )
                    {
                         if  ((*this->pos)->color() != this->color)
                         {
                             ++this->pos;
                         }
                         else
                         {
                             break;
                         }
                    }
                    else
                    {
                        if ( this->pos == this->cel->elements[ this->cel->elements.size()-1 ].end() )
                        {
                            break;
                        }
                    }

                    if ( this->pos == this->cel->elements[ this->gradationLevel ].end() )
                    {
                        if ( this->gradationLevel != this->cel->elements.size()-1 )
                        {
                             ++this->gradationLevel;
                             this->pos = this->cel->elements[this->gradationLevel].begin();
                        }
                    }
                }
                return *this;
            }


            coloredCells& operator--()
            {
                while ( true )
                {
                    if (this->pos != this->cel->elements[this->gradationLevel].begin())
                    {
                          --this->pos;
                    }
                    else
                    {
                        if ( this->gradationLevel )
                        {
                             --this->gradationLevel;
                             this->pos = this->cel->elements[this->gradationLevel].end();
                             --this->pos;
                        }
                    }
                    if ( (this->pos == this->cel->elements[0].begin() ) || ((*this->pos)->color() == this->color ) )
                    {
                         break;
                    }
                }
                return *this;
            }

            coloredCells operator--(int)
            {
                while (true)
                {
                    if (this->pos != this->cel->elements[this->gradationLevel].begin())
                    {
                          --this->pos;
                    }
                    else
                    {
                        if ( this->gradationLevel != 0 )
                        {
                             --this->gradationLevel;
                             this->pos = this->cel->elements[this->gradationLevel].end();
                             --this->pos;
                        }
                    }
                    if ( (this->pos == this->cel->elements[0].begin() )||((*this->pos)->color() == this->color ) )
                    {
                         break;
                    }
                }
                return *this;
            }

            cell<T>* operator*() const
            {
                  if ( this->pos != this->cel->elements[this->gradationLevel].end() )
                  {
                       return *(this->pos);
                  }
                  throw "Iterator out of range";
            }

            cell<T>* operator->() const
            {
                  if ( this->pos != this->cel->elements[this->gragationLevel].end() )
                  {
                       return *this->pos;
                  }
                  throw "Iterator out of range";
             }

             bool operator==(const coloredCells& it) const
             {
                  if ( (this->gradationLevel == it.gradationLevel)&&(this->color == it.color) )
                  {
                       if ( this->pos == it.pos )
                       {
                            return true;
                       }
                  }
                  return false;
             }

             bool operator!=(const coloredCells& it) const
             {
                  return ( !((*this)==it) );
             }


             typedef std::bidirectional_iterator_tag iterator_category;
             typedef cell<T>* value_type;
             typedef ptrdiff_t difference_type;
             typedef cell<T>* pointer;
             typedef cell<T>& reference;
        };//allCells
        coloredCells coloredCellsBegin(int color)
        {
             coloredCells res(this,0,color);
             res.pos = this->elements[0].begin();
             while ( (res.pos != this->elements[ this->elements.size()-1 ].end()) && ((*res)->color() != color) )
             {
                   ++res;
             }
             return res;
        }//coloredCellsBegin

        coloredCells coloredCellsEnd(int color)
        {
             coloredCells res(this, this->elements.size()-1, color);
             res.pos = this->elements[this->elements.size()-1].end();
             return res;
        }//coloredCellsEnd()



        class dimColoredCells
        {
        protected:
            friend class cellComplex<T>;
            cellComplex* cel;
            int dim;
            typename std::set< cell<T>* >::iterator pos;
            int color;
        public:
            explicit dimColoredCells(cellComplex* cmplx , int dim, int color):cel(cmplx),dim(dim), color(color)
            {
                if ( dim < cmplx->elements.size() )
                {
                    this->pos = cmplx->elements[dim].begin();
                }
                else
                {
                    throw "Dim out of range in iterator";
                }
            }
            explicit dimColoredCells(){}


            dimColoredCells& operator++()
            {
                 if (this->pos != this->cel->elements[ dim ].end())
                 {
                       ++this->pos;
                 }
                 while ( (this->pos != this->cel->elements[ dim ].end()) && ((*this->pos)->color() != this->color ) )
                 {
                       ++this->pos;
                 }
                 return *this;
            }//operator++


            dimColoredCells operator++(int)
            {
                if (this->pos != this->cel->elements[ dim ].end())
                {
                    ++this->pos;
                }
                while ( (this->pos != this->cel->elements[ dim ].end()) && ((*this->pos)->color() != this->color) )
                {
                    ++this->pos;
                }
                return *this;
            }


            dimColoredCells& operator--()
            {
                if ( this->pos != this->cel->elements[this->dim].begin() ){--this->pos;}
                while ( (this->pos != this->cel->elements[this->dim].begin()) && ((*this->pos)->color() != this->color) )
                {
                      --this->pos;
                }
                return *this;
            }

            dimColoredCells operator--(int)
            {
                if ( this->pos != this->cel->elements[this->dim].begin() ){--this->pos;}
                while ( (this->pos != this->cel->elements[this->dim].begin()) && ((*this->pos)->color() != this->color) )
                {
                      --this->pos;
                }
                return *this;
            }

            cell<T>* operator*() const
            {
                  if ( this->pos != this->cel->elements[this->dim].end() )
                  {
                       return *(this->pos);
                  }
                  throw "Iterator out of range";
            }

            cell<T>* operator->() const
            {
                  if ( this->pos != this->cel->elements[this->gragationLevel].end() )
                  {
                       return *this->pos;
                  }
                  throw "Iterator out of range";
             }

             bool operator==(const dimColoredCells& it) const
             {
                  if ( (this->dim == it.dim)&&(this->color == it.color) )
                  {
                       if ( this->pos == it.pos )
                       {
                            return true;
                       }
                  }
                  return false;
             }

             bool operator!=(const dimColoredCells& it) const
             {
                  return ( !((*this)==it) );
             }


             typedef std::bidirectional_iterator_tag iterator_category;
             typedef cell<T>* value_type;
             typedef ptrdiff_t difference_type;
             typedef cell<T>* pointer;
             typedef cell<T>& reference;
        };//allCells

        dimColoredCells dimColoredCellsBegin(int dim, int color)
        {
             if ( dim >= this->elements.size() )
             {
                  throw "Dim of iterator out of range for this complex.";
             }
             dimColoredCells res(this,dim,color);
             res.pos = this->elements[dim].begin();
             while ( (res.pos != this->elements[dim].end()) && ((*(res.pos))->color() != color) )
             {
                   ++res;
             }
             return res;
        };//dimColoredCells

        dimColoredCells dimColoredCellsEnd(int dim, int color)
        {
             if ( dim >= this->elements.size() )
             {
                  throw "Dim of iterator out of range for this complex.";
             }
             dimColoredCells res(this, dim, color);
             res.pos = this->elements[dim].end();
             return res;
        }//coloredCellsEnd()



private:
       void tie( cell<T>* coface, cell<T>* face );

       void substitute( cell<T>* source , cell<T>* target )
       {
             typename std::list< cell<T>* >::iterator it;
             for ( it = source->bd().begin() ; it != source->bd().end() ; ++it )
             {
                 target->bound.push_back( (*it) );
             }

             for ( it = source->cbd().begin() ; it != source->cbd().end() ; ++it )
             {
                 target->coBound.push_back( (*it) );
             }
             target->delet = source->delet;

             delete source;
       }

protected:
       typename std::vector< std::set< cell<T>* > > elements;
};//simplicalComplex
