#ifndef TRAPEZOID_H
#define TRAPEZOID_H

#include <vector>

namespace gasprj {

class Trapezoid
{
public:
    // TODO: define constructor and methods
    Trapezoid();

    // Trapezoid identifiers
    size_t idxEdgeTop, idxEdgeBottom;
    size_t idxPointLeft, idxPointRight;
    // Adjacencies
    size_t idxTrapezoidTopRight, idxTrapezoidTopLeft,
           idxTrapezoidBottomLeft, idxTrapezoidBottomRight;
};

} // End namespace gasprj

#endif // TRAPEZOID_H
