#include "trapezoidalmap.h"

namespace gasprj {

TrapezoidalMap::TrapezoidalMap(cg3::Point2d boundingBoxCornerTL,
                               cg3::Point2d boundingBoxCornerBR):
    boundingBox(cg3::Point2d(boundingBoxCornerTL),cg3::Point2d(boundingBoxCornerBR))
{
    // TODO: Create one leaf for the bounding box
}

Trapezoid& TrapezoidalMap::getTrapezoid(size_t id) {
    return trapezoids[id];
}

const std::vector<gasprj::Trapezoid>& TrapezoidalMap::getTrapezoids() const {
    return trapezoids;
}

const cg3::BoundingBox2& TrapezoidalMap::getBoundingBox() const {
    return boundingBox;
}

void TrapezoidalMap::clear() {
    // TODO: Restore the one-leaf bounding-box
}

} // End namespace gasprj
