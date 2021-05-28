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
std::vector<gasprj::Trapezoid>& TrapezoidalMap::getTrapezoids() {
    return trapezoids;
}

void TrapezoidalMap::addNewTrapezoid(gasprj::Trapezoid& newTrap) {
    trapezoids.push_back(newTrap);
}

void TrapezoidalMap::overwriteTrapezoid(gasprj::Trapezoid& newTrap, size_t id) {
    assert(id < trapezoids.size());
    trapezoids[id] = newTrap;
}

size_t TrapezoidalMap::getNumberTrapezoids() {
    return trapezoids.size();
}

const cg3::BoundingBox2& TrapezoidalMap::getBoundingBox() const {
    return boundingBox;
}

void TrapezoidalMap::clear() {
    // TODO: Restore the one-leaf bounding-box
}

} // End namespace gasprj
