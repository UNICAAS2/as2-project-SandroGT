#ifndef TRAPEZOIDALMAP_H
#define TRAPEZOIDALMAP_H

#include <vector>

#include <cg3/geometry/bounding_box2.h>

#include "data_structures/trapezoid.h"
#include "data_structures/trapezoidalmap_dataset.h"

namespace gasprj {

/**
 * @brief The trapezoidal map data structure
 *
 * This class defines the trapezoidal map data structures, storing all the trapezoids defined by a set of segments in
 * general position. Stores a reference to the dataset of points and segments.
 */
class TrapezoidalMap
{
public:
    /* Constructors */
    TrapezoidalMap(TrapezoidalMapDataset *const trapezoidalMapDataset,
                   const cg3::Point2d &boundingBoxCornerBL, const cg3::Point2d &boundingBoxCornerTR);


    /* Public methods */
    virtual Trapezoid &getTrapezoid(size_t id);
    virtual const Trapezoid &getTrapezoid(size_t id) const;
    virtual size_t size() const;

    virtual void addTrapezoid(const Trapezoid &trapezoid);
    virtual void overwriteTrapezoid(const Trapezoid &trapezoid, size_t id);

    TrapezoidalMapDataset *getRefTrapezoidalMapDataset();
    const TrapezoidalMapDataset *getRefTrapezoidalMapDataset() const;
    const cg3::BoundingBox2 &getBoundingBox() const;

    void clear();

protected:
    /* Attributes */
    std::vector<Trapezoid> trapezoids;
    TrapezoidalMapDataset *const refTrapezoidalMapDataset;
    cg3::BoundingBox2 boundingBox;
};

} // End namespace gasprj

#include "trapezoidalmap.tpp"

#endif // TRAPEZOIDALMAP_H
