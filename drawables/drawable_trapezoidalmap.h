#ifndef DRAWABLE_TRAPEZOIDALMAP_H
#define DRAWABLE_TRAPEZOIDALMAP_H

#include <cg3/viewer/interfaces/drawable_object.h>

#include "data_structures/trapezoidalmap.h"
#include "drawables/drawable_trapezoid.h"

namespace gasprj {

/**
 * @brief The drawable version of the trapezoidal map data structure
 *
 * This class defines the drawable version of trapezoidal map, storing the ID of a selected trapezoid and redefining
 * the methods to add and overwrite a trapezoid, so that they create drawable trapezoids.
 */
class DrawableTrapezoidalMap : public TrapezoidalMap, public cg3::DrawableObject
{
public:
    /* Constructors */
    DrawableTrapezoidalMap(TrapezoidalMapDataset *const trapezoidalMapDataset,
                           const cg3::Point2d &boundingBoxCornerBL, const cg3::Point2d &boundingBoxCornerTR);

    /* Drawable */
    void draw() const;
    cg3::Point3d sceneCenter() const;
    double sceneRadius() const;
    size_t getIdHighlightedTrapezoid() const;
    void setIdHighlightedTrapezoid(size_t id);

    /* Public methods */
    virtual Trapezoid &getTrapezoid(size_t id);
    virtual const Trapezoid &getTrapezoid(size_t id) const;
    virtual size_t size() const;

    virtual void addTrapezoid(const Trapezoid &trapezoid);
    virtual void overwriteTrapezoid(const Trapezoid &trapezoid, size_t id);

    void clear();

private:
    /* Internal methods declaration */
    void setDrawableTrapezoidVertices(DrawableTrapezoid &dTrap);
    void setDrawableTrapezoidColor(DrawableTrapezoid &dTrap);

    /* Attributes */
    std::vector<DrawableTrapezoid> trapezoids;
    size_t idHighlightedTrapezoid;
};

} // End namespace gasprj

#endif // DRAWABLE_TRAPEZOIDALMAP_H
