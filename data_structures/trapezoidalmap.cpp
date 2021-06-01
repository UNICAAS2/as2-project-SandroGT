#include "trapezoidalmap.h"

namespace gasprj {

TrapezoidalMap::TrapezoidalMap(cg3::Point2d boundingBoxCornerBL,
                               cg3::Point2d boundingBoxCornerTR):
    boundingBox(cg3::Point2d(boundingBoxCornerBL),cg3::Point2d(boundingBoxCornerTR))
{
    gasprj::Trapezoid boundingBoxTrap = gasprj::Trapezoid();
    boundingBoxTrap.updateVertices(boundingBox);

    trapezoids.push_back(boundingBoxTrap);
}

Trapezoid& TrapezoidalMap::getTrapezoid(size_t id) {
    return trapezoids[id];

}

const std::vector<gasprj::Trapezoid>& TrapezoidalMap::getTrapezoids() const {
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
    trapezoids.clear();

    gasprj::Trapezoid boundingBoxTrap = gasprj::Trapezoid();
    boundingBoxTrap.updateVertices(boundingBox);

    trapezoids.push_back(boundingBoxTrap);
}

} // End namespace gasprj
