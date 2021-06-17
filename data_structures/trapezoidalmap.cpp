#include "trapezoidalmap.h"

namespace gasprj {

/**
 * @brief The constructor of the trapezoidal map
 * @param[in] refTrapezoidalMapDataset The reference to the trapezoidal map dataset
 * @param[in] boundingBoxCornerBL The bottom-left corner of the bounding box
 * @param[in] boundingBoxCornerTR The top-right corner of the bounding box
 */
TrapezoidalMap::TrapezoidalMap(TrapezoidalMapDataset *const refTrapezoidalMapDataset,
                               const cg3::Point2d &boundingBoxCornerBL, const cg3::Point2d &boundingBoxCornerTR) :
    refTrapezoidalMapDataset(refTrapezoidalMapDataset),
    boundingBox(cg3::Point2d(boundingBoxCornerBL),cg3::Point2d(boundingBoxCornerTR))
{
}

/**
 * @brief Get a reference to the specified trapezoid from the trapezoidal map
 * @param[in] id The ID of the trapezoid
 * @return A reference to the specified trapezoid
 */
Trapezoid &TrapezoidalMap::getTrapezoid(size_t id)
{
    assert(id < trapezoids.size());
    return trapezoids[id];
}

/**
 * @brief Get a non-modifiable reference to the specified trapezoid from the trapezoidal map
 * @param[in] id The ID of the trapezoid
 * @return A costant reference to the specified trapezoid
 */
const Trapezoid &TrapezoidalMap::getTrapezoid(size_t id) const
{
    assert(id < trapezoids.size());
    return trapezoids[id];
}

/**
 * @brief Get the number of trapezoids in the trapezoidal map
 * @return The number of trapezoids in the trapezoidal map
 */
size_t TrapezoidalMap::size() const
{
    return trapezoids.size();
}

/**
 * @brief Add a new trapezoid to the trapezoidal map
 * @param[in] trapezoid The new trapezoid
 */
void TrapezoidalMap::addTrapezoid(const Trapezoid &trapezoid)
{
    trapezoids.push_back(trapezoid);
}

/**
 * @brief Overwrite the specified trapezoid in the trapezoidal map
 * @param[in] trapezoid The new trapezoid
 * @param[in] id The ID of the trapezoid to overwrite
 */
void TrapezoidalMap::overwriteTrapezoid(const Trapezoid &trapezoid, size_t id)
{
    assert(id < trapezoids.size());
    trapezoids[id] = trapezoid;
}

/**
 * @brief Get a reference to the trapezoidal map dataset
 * @return A reference to the trapezoidal map dataset
 */
TrapezoidalMapDataset *TrapezoidalMap::getRefTrapezoidalMapDataset()
{
    return refTrapezoidalMapDataset;
}

/**
 * @brief Get a non-modifiable reference to the trapezoidal map dataset
 * @return A constant reference to the trapezoidal map dataset
 */
const TrapezoidalMapDataset *TrapezoidalMap::getRefTrapezoidalMapDataset() const
{
    return refTrapezoidalMapDataset;
}

/**
 * @brief Get the bounding box
 * @return The bounding box
 */
const cg3::BoundingBox2 &TrapezoidalMap::getBoundingBox() const
{
    return boundingBox;
}

/**
 * @brief Delete all the trapezoids stored in the trapezoidal map
 */
void TrapezoidalMap::clear()
{
    trapezoids.clear();
}

} // End namespace gasprj
