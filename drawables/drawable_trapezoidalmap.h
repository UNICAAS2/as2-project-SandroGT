#ifndef DRAWABLE_TRAPEZOIDALMAP_H
#define DRAWABLE_TRAPEZOIDALMAP_H

#include <cg3/viewer/interfaces/drawable_object.h>

#include "data_structures/trapezoidalmap.h"

namespace gasprj {

class DrawableTrapezoidalMap : public TrapezoidalMap, public cg3::DrawableObject
{
public:

    DrawableTrapezoidalMap(cg3::Point2d boundingBoxCornerTL,
                           cg3::Point2d boundingBoxCornerBR);

    void draw() const;
    cg3::Point3d sceneCenter() const;
    double sceneRadius() const;

    size_t getHighlightedTrapezoidId() const;
    void setHighlightedTrapezoidId(size_t id);

private:
    size_t highlightedTrapezoidId;

    const long RANDOM_COLORS_SEED = 0;
    const float TRANSPARENCY_SELECTED = 0.75;
    const float TRANSPARENCY_UNSELECTED = 0.25;
};

} // End namespace gasprj

#endif // DRAWABLE_TRAPEZOIDALMAP_H
