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
class DrawableTrapezoid : public Trapezoid, public cg3::DrawableObject
{
public:
    /* Constructors */
    DrawableTrapezoid(Trapezoid trapezoid);

    /* Drawable */
    void draw() const;
    cg3::Point3d sceneCenter() const;
    double sceneRadius() const;

    const cg3::Color &getColor() const;
    double getColorR() const;
    double getColorG() const;
    double getColorB() const;
    void setColor(int h, int s, int v);
    bool isHighlighted() const;
    void setHighlighted(bool highlight);

    const cg3::BoundingBox2 &getBoundingBox() const;
    void setBoundingBox(const cg3::Point2d &boundingBoxCornerBL, const cg3::Point2d &boundingBoxCornerTR);

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
    bool highlighted;
    cg3::BoundingBox2 boundingBox;
};

} // End namespace gasprj

#endif // DRAWABLETRAPEZOID_H
