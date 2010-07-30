#ifndef SIMPLEX_IO_HPP
#define SIMPLEX_IO_HPP


#include <string>
#include <sstream>
#include <set>
#include <vector>



class SimplexSComplex;

std::vector<std::set<int> > parseDat(istream &stream);

void parseDat(std::istream &stream, SimplexSComplex &comp, int subdivs = 0);

std::vector<std::set<int> > parseObj(std::istream &stream);

void parseObj(std::istream &stream, SimplexSComplex &comp, int subdivs = 0);

#endif
