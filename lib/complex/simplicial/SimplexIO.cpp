#include <string>
#include <sstream>
#include <set>

#include "redHom/complex/simplicial/SimplexSComplex.hpp"
#include "redHom/complex/simplicial/SimplexIO.hpp"
#include "redHom/complex/simplicial/SimplexSubdivision.hpp"

using namespace std;

vector<set<int> > parseDat(istream &stream)
{
    vector<set<int> > simplices;

    for (string s; std::getline(stream, s); )
    {
        if (s.size() == 0 || s[0] == '#')
            continue;

        stringstream ss(s);

        set<int> simp;

        for (int v; ss >> v;)
            simp.insert(v);

        simplices.push_back(simp);
    }

    return simplices;
}

void parseDat(istream &stream, SimplexSComplex &comp, int subdivs)
{
    vector<set<int> > simplices = parseDat(stream);

    for (int i = 0; i < subdivs; i++)
        simplices = subdivide3(simplices);

    for (int i = 0; i < simplices.size(); i++)
        comp.addSimplex(simplices[i]);
}

vector<set<int> > parseObj(istream &stream)
{
    using std::string;
    using std::stringstream;

    vector<set<int> > simplices;

    for (string s; getline(stream, s); )
    {
        stringstream ss(s);

        string what;
        ss >> what;

        if (what != "f")
            continue;

        set<int> simp;

        for (int v; ss >> v;)
            simp.insert(v);

        simplices.push_back(simp);
    }

    return simplices;
}

void parseObj(istream &stream, SimplexSComplex &comp, int subdivs)
{
    vector<set<int> > simplices = parseObj(stream);

    for (int i = 0; i < subdivs; i++)
        simplices = subdivide3(simplices);

    for (int i = 0; i < simplices.size(); i++)
        comp.addSimplex(simplices[i]);
}

