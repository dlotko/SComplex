#include"cell.h"
using namespace std;

template <typename T>
inline unsigned int cell<T>::bdSize()
{
     unsigned int num = 0;
     BdIterator holder;
     for (BdIterator it = this->bound.begin();
     it!=this->bound.end();++it)
     {
         if (!(*it)->delet){num++;}
     }
     return num;
}//bdSize

template <typename T>
inline unsigned int cell<T>::cbdSize()
{
     unsigned int num = 0;
     CbdIterator holder;
     for (CbdIterator it = this->coBound.begin();
     it!=this->coBound.end();++it)
     {
           if (!(*it)->delet){num++;}
     }
     return num;
}//cbdSize


template <typename T>
cell<T>::cell()
{
     this->toRemove = false;
     this->delet = true;
     this->colo = 1;
     this->id = 0;
}

template <typename T>
cell<T>::cell( const cell& original)
{
     typename std::list< cell<T>* >::iterator it;
     for ( it = original.bd().begin() ; it != original.bd().end() ; ++it )
     {
         this->bound.push_back( (*it) );
     }

     for ( it = original.cbd().begin() ; it != original.cbd().end() ; ++it )
     {
         this->coBound.push_back( (*it) );
     }
     this->delet = original.delet;
     this->toRemove = original.toRemove;
     this->colo = original.colo;
     this->id = original.id;
}//copy constructor


