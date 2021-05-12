#ifndef TRAPEZOID_H
#define TRAPEZOID_H

#include <vector>

namespace gasprj {

class Trapezoid
{
public:
    // TODO: define constructor and methods
    Trapezoid();

private:
    // Trapezoid identifiers
    size_t idx_edgeTop, idx_edgeBottom;
    size_t idx_pointLeft, idx_pointRight;
    // Adjacencies
    size_t idx_trapezoidTopRight, idx_trapezoidTopLeft,
           idx_trapezoidBottomLeft, idx_trapezoidBottomRight;
};

} // End namespace gasprj

#endif // TRAPEZOID_H
