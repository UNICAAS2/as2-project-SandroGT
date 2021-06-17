#ifndef DRAWABLETRAPEZOID_H
#define DRAWABLETRAPEZOID_H

#include <cg3/geometry/bounding_box2.h>
#include <cg3/viewer/interfaces/drawable_object.h>

#include "data_structures/trapezoid.h"
#include "data_structures/trapezoidalmap_dataset.h"

namespace gasprj {

/**
 * @brief The drawable version of the trapezoid data structure
 *
 * This class defines the drawable version of a trapezoid, storing also its pre-computed vertices and its color.
 */
class DrawableTrapezoid : public Trapezoid
{
public:
    /* Constructors */
    DrawableTrapezoid(Trapezoid trapezoid);

    /* Drawable */
    const cg3::Color &getColor() const;
    double getColorR() const;
    double getColorG() const;
    double getColorB() const;
    void setColor(int h, int s, int v);

    /* Geometry */
    const cg3::Point2d &getVertexTL() const;
    void setVertexTL(const cg3::Point2d &vertex);
    const cg3::Point2d &getVertexTR() const;
    void setVertexTR(const cg3::Point2d &vertex);
    const cg3::Point2d &getVertexBL() const;
    void setVertexBL(const cg3::Point2d &vertex);
    const cg3::Point2d &getVertexBR() const;
    void setVertexBR(const cg3::Point2d &vertex);

private:
    /* Attributes */
    cg3::Point2d vertexTL, vertexTR, vertexBR, vertexBL;
    cg3::Color color;
};

} // End namespace gasprj

#endif // DRAWABLETRAPEZOID_H
