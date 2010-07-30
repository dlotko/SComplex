#include "redHom/complex/simplicial/SimplexSComplex.hpp"

SimplexSComplex::Simplex* SimplexSComplex::createSimplexHierarchy(vertex_set &s)
{
    assert(s.size() > 0);

    if (s.size() == 1)
    {
        return makeBaseSimplex(s);
    }

    Simplex* rootSimplex = new Simplex(s.begin(), s.end(), nextId++);
    simplexAddedEvent(*rootSimplex);

    // set<int> probably can't be used as a drop-in replacement...
    for (vertex_set::iterator it = s.begin(), end = s.end(); it != end; )
    {
        const int &val = *it;
        vertex_set::iterator new_it = it;
        ++new_it;

        s.erase(it);
        Simplex *subSimplex = addSimplex(s);
        s.insert(it, val);

        rootSimplex->addToBorder(*subSimplex);
        subSimplex->addToCoborder(*rootSimplex, val);
        // {val} = root \ sub

        it = new_it;
    }

    return rootSimplex;
}
