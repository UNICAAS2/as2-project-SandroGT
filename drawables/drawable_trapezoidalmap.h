#ifndef DRAWABLE_TRAPEZOIDALMAP_H
#define DRAWABLE_TRAPEZOIDALMAP_H

#include <cg3/viewer/interfaces/drawable_object.h>

#include "data_structures/trapezoidalmap.h"
#include "drawables/drawable_trapezoid.h"

namespace gasprj {

class DrawableTrapezoidalMap : public TrapezoidalMap, public cg3::DrawableObject
{
public:

    DrawableTrapezoidalMap(TrapezoidalMapDataset *const trapezoidalMapDataset,
                           const cg3::Point2d &boundingBoxCornerBL, const cg3::Point2d &boundingBoxCornerTR);

    void draw() const;
    cg3::Point3d sceneCenter() const;
    double sceneRadius() const;
    size_t getIdHighlightedTrapezoid() const;
    void setIdHighlightedTrapezoid(size_t id);

    virtual Trapezoid &getTrapezoid(size_t id);
    virtual const Trapezoid &getTrapezoid(size_t id) const;
    virtual size_t size() const;

    virtual void addTrapezoid(const Trapezoid &trapezoid);
    virtual void overwriteTrapezoid(const Trapezoid &trapezoid, size_t id);

    void clear();

private:
    size_t highlightedTrapezoidId;

    const long RANDOM_COLORS_SEED = 0;
    const float TRANSPARENCY_SELECTED = 0.75;
    const float TRANSPARENCY_UNSELECTED = 0.25;

    std::vector<DrawableTrapezoid> trapezoids;
    size_t idHighlightedTrapezoid;
};

} // End namespace gasprj

#endif // DRAWABLE_TRAPEZOIDALMAP_H
