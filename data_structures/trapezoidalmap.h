#ifndef TRAPEZOIDALMAP_H
#define TRAPEZOIDALMAP_H

#include <unordered_map>
#include <utility>
#include <vector>

#include <cg3/geometry/bounding_box2.h>

#include "data_structures/trapezoid.h"

namespace gasprj {

class TrapezoidalMap
{
public:
    TrapezoidalMap(cg3::Point2d boundingBoxCornerTL,
                   cg3::Point2d boundingBoxCornerBR);

    Trapezoid& getTrapezoid(size_t id);
    std::vector<gasprj::Trapezoid>& getTrapezoids();
    void addNewTrapezoid(gasprj::Trapezoid& newTrap);
    void overwriteTrapezoid(gasprj::Trapezoid& newTrap, size_t id);
    size_t getNumberTrapezoids();

    const cg3::BoundingBox2& getBoundingBox() const;

    void clear();

private:
    std::vector<gasprj::Trapezoid> trapezoids;

    cg3::BoundingBox2 boundingBox;
};

} // End namespace gasprj

#endif // TRAPEZOIDALMAP_H
