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
    const std::vector<gasprj::Trapezoid>& getTrapezoids() const;

    const cg3::BoundingBox2& getBoundingBox() const;

    void clear();

private:
    std::vector<gasprj::Trapezoid> trapezoids;

    cg3::BoundingBox2 boundingBox;
};

} // End namespace gasprj

#endif // TRAPEZOIDALMAP_H
