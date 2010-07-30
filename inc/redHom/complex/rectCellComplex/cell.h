/************************************************************************/
#include<iostream>
#include<list>
#include<vector>
#include<set>

#include <boost/iterator/filter_iterator.hpp>

using namespace std;

template <typename T>
class cell
{
public:
    //s complex
    typedef int Dim;
    typedef int Id;
    typedef char Color;

    //constructors:
    cell();
    cell( const cell& original);
    cell( typename std::vector< std::pair<T,T> > coef)
    {
         for ( unsigned int i = 0 ; i != coef.size() ; ++i )
         {
             this->coef.push_back( std::make_pair(coef[i].first , coef[i].second) );
         }
         this->delet = true;
         this->colo = 1;
         this->id = this->number;
         ++this->number;
    }

    int dim()
    {
        //counting the number of non degenerated intervals in coef:
        int dim = 0;
        for ( unsigned int i = 0 ; i != this->coef.size() ; ++i )
        {
            if ( this->coef[i].first != this->coef[i].second )
            {
                 ++dim;
            }
        }
        return dim;
    }

    int getId(){return this->id;}

    //for a hash table
    T value()
    {
        //std::vector< std::pair<T,T> > coef;
        T val = 0;
        for ( unsigned int i = 0 ; i != this->coef.size() ; ++i )
        {
              val +=this->coef[i]->second - this->coef[i]->first;
        }
        return val;
    }


    std::list< cell* >& bd() {return this->bound;};
    std::list< cell* > bd() const {return this->bound;};
    std::list< cell* >& cbd(){return this->coBound;};
    std::list< cell* > cbd()const{return this->coBound;};


    unsigned int bdSize();
    unsigned int cbdSize();
    inline void del(){this->delet = true;};

    inline void undel(){this->delet = false;};

    void undelWithAllBdElem()
    {
         this->undel();

         typename cell::BdIterator bd1, bd2;
         for ( bd1 = this->bdBegin() ; bd1 != this->bdEnd() ; ++bd1 )
         {
             (*bd1)->undel();
             for ( bd2 = (*bd1)->bdBegin() ; bd2 != (*bd1)->bdEnd() ; ++bd2 )
             {
                 (*bd2)->undel();
             }
         }
    }

    inline bool& deleted(){return this->delet;};

    template < typename A >
    friend std::ostream& operator<<(std::ostream& out, const cell<A>& sim)
    {
         out << "[ "  ;
         for ( unsigned int i = 0 ; i != sim.coef.size() ; ++i )
         {
              out << "[" << sim.coef[i].first << "," << sim.coef[i].second << "]";
              if ( i != sim.coef.size()-1 ) out << "x";
         }
         out << " ]";
         return out;
    };

    template < typename A >
    inline friend bool operator == ( const cell<A>& t1, const cell<A>& t2 )
    {
           if ( t1.coef.size() == t2.coef.size() )
           {
                for ( unsigned int i = 0 ; i != t1.coef.size() ; ++i )
                {
                    if ( t1.coef[i].first != t2.coef[i].first )
                    {
                         return false;
                    }
                    if ( t1.coef[i].second != t2.coef[i].second )
                    {
                         return false;
                    }
                }
                return true;
           }
           return false;
    }

    template <typename A>
    friend class cellComplex;

    template <typename A>
    friend int computeIncidence( cell<A>* first , cell<A>* second );

    template <typename A>
    friend bool checkIfIsASubset( cell<A>* coface , cell<A>* face );

    typename std::vector< std::pair<T,T> > coords(){return this->coef;}



    struct HasColor
    {
        const int color;

        HasColor(const int c) : color(c) {}

        bool operator()(const cell<T>* const cell) const
        {
            return color == cell->color();
        }
    };

    //iterators
    typedef typename std::list< cell* >::iterator BdIterator;
    typedef typename std::list< cell* >::iterator CbdIterator;

    BdIterator bdBegin(){return this->bound.begin();};
    BdIterator bdEnd(){return this->bound.end();};
    CbdIterator cbdBegin(){return this->coBound.begin();};
    CbdIterator cbdEnd(){return this->coBound.end();};







    //const iterators:
    typedef typename std::list< cell* >::const_iterator constBdIterator;
    typedef typename std::list< cell* >::const_iterator constCbdIterator;
    constBdIterator constBdBegin() const {return this->bound.begin();};
    constBdIterator constBdEnd() const {return this->bound.end();};
    constCbdIterator constCbdBegin() const {return this->coBound.begin();};
    constCbdIterator constCbdEnd() const {return this->coBound.end();};









    typedef boost::filter_iterator<HasColor, BdIterator> coloredBdIterator;
    typedef boost::filter_iterator<HasColor, constBdIterator> constColoredBdIterator;

    typedef boost::filter_iterator<HasColor, CbdIterator> coloredCbdIterator;
    typedef boost::filter_iterator<HasColor, constCbdIterator> constColoredCbdIterator;

    //S-complex, metoda zwracajaca kolor, kolorowane iteratory brzegow i kobrzegow.
    int& color(){return this->colo;}
    int color() const {return this->colo;}

    coloredBdIterator coloredBdBegin(int color)
    {
        return coloredBdIterator(HasColor(color), this->bound.begin(), this->bound.end());
    }//begin()
    coloredBdIterator coloredBdEnd(int color)
    {
        return coloredBdIterator(HasColor(color), this->bound.end(), this->bound.end());
    }
    constColoredBdIterator constColoredBdBegin(int color) const
    {
        return constColoredBdIterator(HasColor(color), this->bound.begin(), this->bound.end());
    }

    constColoredBdIterator constColoredBdEnd(int color) const
    {
        return constColoredBdIterator(HasColor(color), this->bound.end(), this->bound.end());
    }
    coloredCbdIterator coloredCbdBegin(int color)
    {
        return coloredCbdIterator(HasColor(color), this->coBound.begin(), this->coBound.end());
    }//begin()
    coloredCbdIterator coloredCbdEnd(int color)
    {
        return coloredCbdIterator(HasColor(color), this->coBound.end(), this->coBound.end());
    }
    constColoredCbdIterator constColoredCbdBegin(int color) const
    {
        return constColoredCbdIterator(HasColor(color), this->coBound.begin(), this->coBound.end());
    }
    constColoredCbdIterator constColoredCbdEnd(int color) const
    {
        return constColoredBdIterator(HasColor(color), this->coBound.end(), this->coBound.end());
    }


     static int number;
protected:
    std::list<cell*> bound;
    std::list<cell*> coBound;
    std::vector< std::pair<T,T> > coef;
    bool delet;
    int colo;
    int id;
};//cell


template<typename T> int cell<T>::number = 0;

template <typename A>
bool checkIfIsASubset( cell<A>* coface , cell<A>* face )
{
     if ( coface->coef.size() != face->coef.size() )
     {
          return false;
     }
     for ( unsigned int i = 0 ; i != coface->coef.size() ; ++i )
     {
         if ( !(
             (coface->coef[i].first <= face->coef[i].first)
             &&
             (coface->coef[i].second >= face->coef[i].second)
             )
            )
            {
                return false;
            }
     }
     return true;
}//checkIfIsASubset

template <typename A>
int computeIncidence( cell<A>* coface , cell<A>* face )
{
    if ( checkIfIsASubset(coface, face)==false ){return 0;}

    int sumOfDim = 0;
    unsigned int i = 0;
    while ( i != coface->coef.size() )
    {
        if ( (coface->coef[i].first != coface->coef[i].second)
             &&
             ( face->coef[i].first != face->coef[i].second )
           )
        {
             ++sumOfDim;
        }
        else
        {
            if ( coface->coef[i].first != coface->coef[i].second )
            {
                 int minusOneToPowSum = 1;
                 if ( sumOfDim%2 == 1 ){minusOneToPowSum = -1;}
                 //in this case we are sure, that face is degenerated at this cord
                 if ( coface->coef[i].first == face->coef[i].first )
                 {
                      return (-1)*minusOneToPowSum;
                 }
                 if ( coface->coef[i].second == face->coef[i].first )
                 {
                      return minusOneToPowSum;
                 }
            }
        }
        ++i;
    }
    return 0;
}
