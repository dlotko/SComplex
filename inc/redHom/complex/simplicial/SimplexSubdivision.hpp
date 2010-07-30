#ifndef SIMPLEX_SUBDIVISION_HPP
#define SIMPLEX_SUBDIVISION_HPP

#include <iterator>
#include <iostream>
#include <vector>
#include <set>
#include <cmath>

using namespace std;

/*
0 1 2 3
4 5 6 7
8 9 A B
C D E F

torus:
0 1 2 0
4 5 6 4
8 9 A 8
0 1 2 0

klein bottle:
0 1 2 0
4 5 6 4
8 9 A 8
0 2 1 0

projective space:
0 1 2 3
4 5 6 7
7 9 A 4
3 2 1 0
*/

std::set<int> make_int_set(int a = -1, int b = -1, int c = -1, int d = -1);

void print_set(const std::set<int> &s);

int find_set(const std::vector<int> &v, int id);

std::vector<int> makeTorusWelds();

std::vector<int> makeKleinWelds();

std::vector<int> makeProjectiveSpaceWelds();

std::vector<std::set<int> > makeSpaceFromWelds(const std::vector<int> &welds);

// barycentric
std::vector<std::set<int> > subdivide6(const std::vector<set<int> > &v);

std::vector<std::set<int> > subdivide3(const std::vector<set<int> > &v);

#endif
