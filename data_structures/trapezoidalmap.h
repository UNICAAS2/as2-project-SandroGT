#ifndef TRAPEZOIDALMAP_H
#define TRAPEZOIDALMAP_H

#include <unordered_map>
#include <utility>
#include <vector>

#include "data_structures/trapezoid.h"

namespace gasprj {

class TrapezoidalMap
{
public:
    // TODO: define constructor
    TrapezoidalMap();

    gasprj::Trapezoid& getTrapezoid(size_t idx);

    std::vector<gasprj::Trapezoid> trapezoids;
};

} // End namespace gasprj

#endif // TRAPEZOIDALMAP_H
